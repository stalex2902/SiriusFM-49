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

namespace SiriusFM {}

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
			long 					m_maxM; // max # of t points
			long 					m_maxN; // max # of S points
			double* const m_grid; // 2D grid as 1D array
			double* const m_ts; 	// timeline
			double* const m_S;		// S-line
			double* const m_ES; 	// E[S](t)
			double* const m_VarS; // Var[S](t)

		public:
			// non-default Ctor:
			GridNOP1D_S3_RKC1
			(
				char const* a_ratesFileA,
				char const* a_ratesFileB,
				long a_maxN = 2048,
				long a_maxM = 210'384
			)
			: m_irpA(a_ratesFileA),
				m_irpB(a_ratesFileB),
				m_maxN(a_maxN),
				m_maxM(a_maxM),
				m_grid(new double[m_maxN * m_maxN]),
				m_ts	(new double[m_maxM]),
				m_S		(new double[m_maxN]),
				m_ES	(new double[m_maxM]),
				m_VarS(new double[m_maxM])
			{}
			
			// non-default Dtor:
			~GridNOP1D_S#_RKC1() {
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
			// "RunBI": performs Backward-Induction                               //			
			//--------------------------------------------------------------------//
			
			void RunBI
			(
				Option<AssetClassA, AssetClassB> const* a_option, // option spec
				Diffusion1D const* a_diff,
				// grid params:
				double a_S0,	 					// S(t0): may differ from Diffusion1D starting point
				time_t t0, 		 					// abs pricing time 
				long a_N 				 = 500, // # of S-points
				int a_tauMins 	 = 30, 	// TimeStep in mins
				double a_BFactor = 4.5, // # of StDs for upper bound
			);
	};
}
