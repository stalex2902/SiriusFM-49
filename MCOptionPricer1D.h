/* testing vanilla option pricing with Monte-Carlo */

#include <iostream>

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "VanillaOption.h"

namespace SiriusFM {

//============================================================================================//
// "MCOptionPricer1D:"                                                                        //
//============================================================================================//
	template
	<
		typename Diffusion1D, typename AProvider, typename BProvider,
		typename AssetClassA, typename AssetClassB
	>
	class MCOptionPricer1D {
		private:			
			// Path Evaluator for option pricing
			class OPPathEval {
				private:
					Option<AssetClassA, AssetClassB> const* const m_option;
					long 				m_P;   // Total path evaluator
					double 				m_sum; // sum of payoffs
					double 				m_sum2; // sum of payoffs^2
					double				m_minPO; // min PayOff
					double				m_maxPO; // max PayOff
 
				public:
					OPPathEval(Option<AssetClassA, AssetClassB> const* a_option)
					: m_option(a_option),
					  m_P(0),
					  m_sum(0),
					  m_sum2(0),
					  m_minPO( INFINITY),
					  m_maxPO(-INFINITY)

					{assert(m_option != nullptr);}
					
					// overload operator "()"
					void operator() (long a_L, long a_PM,
									double const* a_paths, double const* a_ts) {
						for (long p = 0; p < a_PM; ++p) {
							double const* path = a_paths + p * a_L;
							double payOff = m_option->Payoff(a_L, path, a_ts);
							m_sum += payOff;
							m_sum2 += payOff * payOff;
							m_minPO = std::min<double>(m_minPO, payOff);
							m_maxPO = std::max<double>(m_maxPO, payOff);
						}
					m_P += a_PM;
					}

					// GetPx return E[Px]
					double GetPx() const {
					if (m_P < 2)
						throw std::runtime_error("empty OPPathEval");
					return m_sum / double(m_P);
					}
				
					// GetStats returns StD[Px], Min[PayOff], Max[PayOff]
					std::tuple<double, double, double> GetStats() const {
						if (m_P < 2)
							throw std::runtime_error("empty OPPathEval");
						double px = m_sum / double(m_P);
						double var = (m_sum2 - double(m_P) * px * px) / double(m_P - 1);
						assert(var >= 0);
						return std::make_tuple(sqrt(var), m_minPO, m_maxPO);
					}
			};
			
			Diffusion1D const* const  m_diff;
    		AProvider                 m_irpA;
    		BProvider                 m_irpB;
    		MCEngine1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB,
               		   OPPathEval>    m_mce;
    		bool                      m_useTimerSeed;

		public:
			// non-default constructor:
			MCOptionPricer1D
			(
				Diffusion1D const* a_diff, 
				const char* 	   a_irsFileA, 
				const char* 	   a_irsFileB,
				bool 			   a_useTimerSeed
			)			
			: m_diff(a_diff),
			  m_irpA(a_irsFileA),
			  m_irpB(a_irsFileB),
			  m_mce (102'271, 4096), // (5-min points in 1y) * 4k pats in-memory
			  m_useTimerSeed(a_useTimerSeed)
			{}
			
			// The pricing function
			double Px
			(	
				// Instrument spec:
				Option<AssetClassA, AssetClassB> const* a_option,
				//pricing time
				time_t a_t0,
				//MC params
				int a_tauMins = 15, // by default
				long a_P = 100'000
			);
	};
}
