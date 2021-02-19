//==========================================================================//
//                         "GridNOP1D_S3_RKC1.hpp"                          //
// Implementation of "Run" and "GetPxDeltaGamma0" methods                   //
//==========================================================================//

#pragma once                                                                    
                                                                                 
#include "GridNOP1D_S3_RKC1.h"
#include "Time.h"

#include <stdexcept>

namespace SiriusFM {

	//------------------------------------------------------------------------//
	// "Run" implemetation:                                                   //
	//------------------------------------------------------------------------//
	template                                                                      
	<                                                                             
		typename Diffusion1D, typename AProvider, typename BProvider,               
		typename AssetClassA, typename AssetClassB                                  
	>
	template<bool IsFwd>
	void GridNOP1D_S3_RKC1<Diffusion1D, AProvider,
															BProvider, AssetClassA, AssetClassB>::
	Run
	(
		Option<AssetClassA, AssetClassB> const* a_option, // option spec
		Diffusion1D const* a_diff,

		// grid params:
		double a_S0,	 	  // S(t0): may differ from Diffusion1D starting point
		time_t a_t0,	 	  // abs starting time
		long 	 a_Nints,  	// # of S-intervals
		int 	 a_tauMins, // TimeStep in mins
		double a_BFactor  // # of StDs for upper boundary
	)
	{
		//----------------------------------------------------------------------//
		// Construct the grid:                                                  //
		//----------------------------------------------------------------------//
		assert(a_option != nullptr && a_diff != nullptr && a_Nints > 0 
																						&& a_tauMins > && a_BFactor > 0);
		
		if (IsFwd && a_option->m_isAmerican)
			throw std::invalid_argument("American options are not supported in Fwd");

		
		//S0 must be positive for non-IR options:
		assert(a_S0 > 0);

		if (a_option->m_isAsian)
			throw std::invalid_argument("Asian options aren`t supported by 1D-grid");
		
		m_isFwd = IsFwd;

		// time to option expir as Year Frac:
		double TTE = YearFracInt(a_option->m_expirTime - a_t0);

		// fill in the timeline:
		long tauSec = a_tauMins * 60;
		long Mints  = (a_option->m_expirTime - a_t0) / tauSec;
																										// number of t-intervals
		if (TTE <= 0 || Mints <= 0)                                                               
			throw std::invalid_argument("Option has already expired or too close");
		
		m_M = Mints + 1; // # of t-points
	
		if (m_M > m_maxM)
			throw std::invalid_argument("too many t-points");

		double tau = TTE / double(Mints); // time step

		double integrAB = 0.0;
		m_ES	[0]				= a_S0;
		m_VarS[0] 			= 0;

		for (int j = 0; j < m_M; ++j) {
			// Advance the timeline:
			double t = YearFrac(a_t0 + j * tauSec);
			m_ts[j]  = t;
		
			// Integrate E[S](t) and Var[S](t) curves:
			// take rB(t) - rA(t) and cut the negative values to nake sure the grid 
			// upper boundary is expanding with time:

			double rA = m_irpA.r(a_option->m_assetA, t);
			double rB = m_irpB.r(a_option->m_assetB, t);
			double rateDiff = std::max<double>(rB - rA, 0.0);
			
			// integrated rates:
			if (j < m_M - 1) {
				integrAB += rateDiff * tau;

				// E[St]:
				m_ES[j+1] = a_S0 * exp(integrAB); 
			
				// Var[St]:
				double sigma = a_diff->sigma(m_ES[j], t);
				m_VarS[j+1]  = m_VarS[j] + sigma * sigma * tau;
			}
		}

		double StDS = sqrt(m_VarS[m_M - 1]); // Estimated StD  at the end:
		
		double B = m_ES[m_M - 1] + a_BFactor * StDS; // Upper bound for S:

		// Generate the S-line:
		double h = B / double(a_Nints); // S-step

		// S0 should be exactly on the grid:
		m_i0 = int(round(a_S0 / h));
		h = a_S0 / double(m_i0);
		
		if (!std::isfinite(h))
			throw std::invalid_argument("S0 is too small, try increasing N");
		
		B = h * double(a_Nints); // adjust the upper bound B
			
		m_N = a_Nints + 1; // # of S-points
		
		if (m_N > m_maxN)
			throw std::invalid_argument("Nints is too large");

		// NB: the Grid is stored by-column (S-continious) for better locality
		// payOff is used in Bwd Induction only:
		double* payOff = !IsFwd ? (m_grid + m_N * (m_M - 1)) : nullptr; 
																															// last column
	
		for (int i = 0; i < m_N; ++i) {
			m_S[i] = double(i) * h;
			
			// Create the payoff at t=T on the grid. The grid is stored by-column:
			if (!IsFwd)
				payOff[i] = a_option->Payoff(1, m_S + i, m_ts + (m_M - 1));
		}
		
		// initial condition for Fwd:
		if (IsFwd) {
			// the initial condition is delta(S-S0):
			for (int i = 0; i < m_N; ++i)
				m_grid[i] = 0;

			m_grid[m_i0] = 1 / h;
		}
		
		// At low bound (S = a = 0) we always have a const boundaty condition,
		// continious with payoff
		double fa = IsFwd ? 0 : payOff[0];

		// At the upper bound we use a const bounadry condition if it is 0,
		// otherwise we fix df/dS (a Neumann-type condition)
		
		bool isNeumann = false;
		double UBC 		 = 0.0;

		if (!IsFwd) {
			isNeumann = (payOff[m_N - 1] != 0);
			UBC				= isNeumann ? (payOff[m_N - 1] - payOff[m_N - 2]) : 0;
		}
		
		// Lower bound is const in any case, in particular 0s for Fwd:
		for (int j = 0; j < m_M - 1; ++j)
			m_grid[j * m_N] = fa; // low bound

		// Time Marshalling:
		double D2 = 2 * h * h; // denum in the diffusive term

		for (int j = IsFwd ? 0 :  m_M - 1;
				IsFwd ? (j <= m_M - 2) : (j >= 1);
				j += (IsFwd ? 1 : -1)) 
		{
			double const*  fj = m_grid + j * m_N; // prev time layer (j)
			double* 		  fj1 = const_cast<double*>(IsFwd ? (fj + m_N) : (fj - m_N));
																	// curr time layer to be filled in (j+-1)
			double tj 		= m_ts[j];
			double rateAj = m_irpA.r(a_option->m_assetA, tj);
			double rateBj = m_irpB.r(a_option->m_assetB, tj);
			double C1 		= (rateBj - rateAj) / (2 * h); 
																						// coeff in the convective term
			fj1[0] = fa; // low bound

//#			pragma omp parallel for
			for (int i = 1; i <= m_N - 2; ++i) {
				double Si   = m_S[i];
				double fjiM = fj [i - 1];
				double fji  = fj [i];
				double fjiP = fj [i + 1];

				double sigma = a_diff->sigma(Si, tj); // vol

				double DfDt = 0;

				if (IsFwd) {
					// Fokker-Planch:
					double SiM 		= m_S[i - 1];
					double SiP 		= m_S[i + 1];
					double sigmaP = a_diff->sigma(SiP, tj);
					double sigmaM = a_diff->sigma(SiM, tj);

					DfDt = - C1 * (SiP * fjiP - SiM * fjiM)
								 + (sigmaP * sigmaP * fjiP - 2 * sigma * sigma * fji
								 + sigmaM * sigmaM * fjiM) / D2;
					}

				else {
					// Black-Scholes-Merton:
					double DfDt = rateBj * fji 							// reactive term
											- C1 * Si * (fjiP - fjiM) 	// convective term
											- sigma * sigma / D2 * (fjiP - 2 * fji + fjiM);
				}

				//FIXME: we use Euler`s method insted of RKC1
				fj1[i] = fji - tau * DfDt;
			}

			fj1[m_N - 1] = (!IsFwd && isNeumann) ? (fj1[m_N - 2] + UBC) : UBC; 
																														// uppper bound

			// Bwd run allows us to price American options as well:
			if (a_option->m_isAmerican) {
				assert(!IsFwd);
				for (int i = 0; i < m_N; ++i) {

					// Intrinsic value of the option is the payoff evaluated under the
					// curr inderlying price Si:
					double intrVal = a_option->Payoff(1, m_S + i, &tj);

					fj1[i] = std::max<double>(fj1[i], intrVal);
				}
			}
		} // end of Time Marshalling
	}

	//------------------------------------------------------------------------//
	// GetPxDeltaGamma0 implementation                                        //
	//------------------------------------------------------------------------//
	template                                                                      
	<                                                                             
		typename Diffusion1D, typename AProvider, typename BProvider,               
		typename AssetClassA, typename AssetClassB                                  
	>
	std::tuple<double, double, double> GridNOP1D_S3_RKC1<Diffusion1D, AProvider,
															BProvider,  AssetClassA, AssetClassB>::
	GetPxDeltaGamma0() const {

		if (m_M == 0 || m_N == 0)
			throw std::runtime_error("Run BI first");

		assert(0 <= m_i0 && m_i0 < m_N);
		
		double h = m_S[1] = m_S[0];
		double px = m_grid[m_i0]; // j=0
		double delta = 0;
		double gamma = 0;

		if (0 < m_i0 && m_i0 <= m_N - 2) {
			delta = (m_grid[m_i0 + 1] - m_grid[m_i0 + 1]) / (2 * h);
			gamma = (m_grid[m_i0 + 1] - 2 * m_grid[m_i0]
																						+ m_grid[m_i0 - 1]) / (h * h);
		}
		else if (m_i0 == 0)
			delta = (m_grid[1] - m_grid[0]) / h; // gamma remains 0
		
		else {
			assert(m_i0 = m_N - 1);
			delta = (m_grid[m_N - 1] - m_grid[m_N - 2]) / h; // gamma remains 0
		}

		return std::make_tuple(px, delta, gamma);
	}
}
