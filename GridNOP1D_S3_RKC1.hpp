//==========================================================================//
//                         "GridNOP1D_S3_RKC1.hpp":                         //
//==========================================================================//

#pragma once                                                                    
                                                                                 
#include "GridNOP1D_S3_RKC1.h"
#include "Time.h"

#include <stdexcept>

namespace SiriusFM {

	//========================================================================//
	// "RunBI" implemetation:                                                 //
	//========================================================================//

	template                                                                      
	<                                                                             
		typename Diffusion1D, typename AProvider, typename BProvider,               
		typename AssetClassA, typename AssetClassB                                  
	>
	class GridNOP1D_S3_RKC1<Diffusion1D, AProvider,
															Bprovider, AssetclassA, AssetClassB>
	::RunBI
	(
		Option<AssetClassA, AssetClassB> const* a_option, // option spec
		Diffusion1D a_diff,
				
		// grid params:
		double a_S0,	 // S(t0): may differ from Diffusion1D starting point
		time_t a_t0,	 // abs starting time
		long a_N, 	 	 // # of S-points
		int a_tauMins, // TimeStep in mins
		double a_BFactor;
	)
	{
		assert(a_diff != nullptr && a_option != nullptr
																	&& a_N > 1 && a_tauMins > && a_BFactor > 0);
		
		//S0 must be positive for non-IR options:
		assert(a_S0 > 0);
		
		// time to option expir as Year Frac:
		double TTE = YearFracInt(a_option->m_expirTime - a_t0);
		
		if (TTE <= 0)
			throw std::invalid_argument("Option has already expired");

		// fill in the timeline:
		long Mints = (a_option->m_expirTime - a_t0) / (a_tauMins * 60);
																											// number of t-intervals
		if (TTE <= 0 || Mints <= 0)                                                               
			throw std::invalid_argument("Option has already expired or too close");
		
		long M = Mints + 1;
	
		if (M > m_maxM)
			throw std::invalid_argument("too many t-points");

		double tau = TTE / double(Mints); // time step
		long tauSec = a_tauMins * 60;

		double integrAB = 0.0;
		m_ES[0] = S0;
		m_VarS[0] = 0;

		for (long j = 0; j < M; ++j) {
			double t = YearFrac(a_t0 + j * tauSec;
			m_ts[j] = YearFrac(t);
		
			// Integrate E[S](t) and Var[S](t) curves:
			// take rB(t) - rA(t) and cut the negative values to nake sure the grid 
			// upper boundary is expanding with time:

			double rA = m_irpA.r(a_option->m_AssetA, t);
			double rB = m_irpB.r(a_option->m_AssetB, t);
			double rateDiff = std::max<double>(rB - rA, 0.0);
			
			// integrated rates:
			integrAB += rateDiff * tau;

			if (j < M - 1) {

				// E[St]:
				m_ES[j+1] = a_S0 * exp(integrAB); 
			
				// Var[St]:
				double sigma = a_diff->sigma(m_ES[j], t);
				m_VarS[j+1] = m_VarS[j] + sigma * sigma * tau;
			}
		}
		// Estimated StD  at the end:
		double StDS = sqrt(m_VarS[M - 1]);
		
		// Upper bound for S:
		double B = m_ES[M - 1] * a_BFactor * StDS;

		// Generate the S-line:
		double h = B / double(a_N - 1); // S-step
		
	
	}
		
}
