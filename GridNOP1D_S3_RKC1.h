//==========================================================================//
//                           "GridNOP1D_S3_RKC1.h":                         //
// Grid Pricer for Non-IR Options with 1D diffusions, using 3-point stencils//
//                and Runge-Kutta-Chebyshev time marshalling                //
//--------------------------------------------------------------------------//
// Non-IRP means that:                                                      //
// * S >= 0 always;                                                         //
// * rates don`t depend on S;                                               //
// * not suitable for very long TTE (no boynding box scaling with time)     //
//==========================================================================//

#pragma once

#include "IRProvider.h"                                                         
#include "Option.h"

#include <tuple>

namespace SiriusFM {

	//========================================================================//
	// GridNOP1D class:                                                       //
	//========================================================================//

	template                                                                      
	<                                                                             
		typename Diffusion1D, typename AProvider, typename BProvider,               
		typename AssetClassA, typename AssetClassB                                  
	>
	class GridNOP1D_S3_RKC1 {
		//----------------------------------------------------------------------//
		// Data fields:                                                         //
		//----------------------------------------------------------------------//
		private:
			AProvider 		m_irpA;
			BProvider 		m_irpB;
			long 					m_maxM;  // max # of t points
			long 					m_maxN;  // max # of S points
			double* const m_grid;  // 2D grid as 1D array
			double* const m_ts; 	 // timeline
			double* const m_S;		 // S-line
			double* const m_ES; 	 // E[S](t)
			double* const m_VarS;  // Var[S](t)
			int 					m_N;		 // actual # of S-point
			int 					m_M;		 // actual #  of t-points
			int 					m_i0;		 // S(i0) = S0
			bool					m_isFwd; // last run was Fwd

		public:
			// non-default Ctor:
			GridNOP1D_S3_RKC1
			(
				char const* a_ratesFileA,
				char const* a_ratesFileB,
				long a_maxN = 2048,
				long a_maxM = 210'384
			)
			: m_irpA (a_ratesFileA),
				m_irpB (a_ratesFileB),
				m_maxN (a_maxN),
				m_maxM (a_maxM),
				m_grid (new double[m_maxN * m_maxN]),
				m_ts	 (new double[m_maxM]),
				m_S		 (new double[m_maxN]),
				m_ES	 (new double[m_maxM]),
				m_VarS (new double[m_maxM]),
				m_N		 (0),
				m_M		 (0),
				m_i0	 (0),
				m_isFwd(false)
			{
				// zero-out all arrays:
				memset(m_grid, 0, m_maxN * m_maxM * sizeof(double));
				memset(m_S, 	 0, m_maxN 					* sizeof(double));
				memset(m_ts, 	 0, m_maxM 					* sizeof(double));	
				memset(m_ES, 	 0, m_maxM 					* sizeof(double));		
				memset(m_VarS, 0, m_maxM 					* sizeof(double));
			}

			// non-default Dtor:
			~GridNOP1D_S3_RKC1() {
				delete[] (m_grid);
				delete[] (m_ts);
				delete[] (m_S);
				delete[] (m_ES);
				delete[] (m_VarS);
				const_cast<double*&>(m_grid) = nullptr;
				const_cast<double*&>(m_S) 	 = nullptr;
				const_cast<double*&>(m_ts) 	 = nullptr;
				const_cast<double*&>(m_ES) 	 = nullptr;
				const_cast<double*&>(m_VarS) = nullptr;
			}

			//--------------------------------------------------------------------//
			// "Run": performs Backward or Forward-Induction                      //			
			//--------------------------------------------------------------------//
			template<bool IsFwd> // =true for Fwd induction
			void Run
			(
				Option<AssetClassA, AssetClassB> const* a_option, // option spec
				Diffusion1D const* a_diff,
				// grid params:
				double a_S0,	 					// S(t0): may differ from Diffusion1D S0
				time_t a_t0, 						// abs starting time
				long a_Nints		 = 500, // # of S-intervals
				int a_tauMins 	 = 30, 	// TimeStep in mins
				double a_BFactor = 4.5 	// # of StDs for upper bound
			);

			//--------------------------------------------------------------------//
			// GetPxDeltaGamma0: return Px, Delta and Gamma at t=0                //
			//--------------------------------------------------------------------//
			std::tuple<double, double, double> GetPxDeltaGamma0() const;
	};
}
