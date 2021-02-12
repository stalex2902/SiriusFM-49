#pragma once

#include <cmath>
#include <stdexcept>
#include <new>
#include <tuple>

#include "Time.h"

namespace SiriusFM {
	template
	<
		typename Diffusion1D, typename AProvider, typename BProvider, 
		typename AssetClassA, typename AssetClassB,	typename PathEvaluator
	>
	class MCEngine1D {
		private:
			long 	const m_MaxL; // max path length
			long 	const m_MaxPM; // max # of paths stored in memory
			double* const m_paths;
			double* const m_ts;

		public:
			MCEngine1D(long a_MaxL, long a_MaxPM):
				m_MaxL(a_MaxL),
				m_MaxPM(a_MaxPM),
				m_paths(new double[m_MaxL * m_MaxPM]),
				m_ts(new double[m_MaxL])
			{
				if (m_MaxL <= 0 || m_MaxPM <= 0)
					throw std::invalid_argument("invalid max path size");

				for (long l = 0; l < m_MaxL; ++l) {
					m_ts[l] = 0;
					long lp = l * m_MaxPM;
					for (long p = 0; p < m_MaxPM; ++p)
						m_paths[lp + p] = 0;
				}
			}

			~MCEngine1D() {
				delete[] m_paths;
				delete[] m_ts;
			}

			MCEngine1D(MCEngine1D const&) = delete; // no copy-constructor

			MCEngine1D& operator=(MCEngine1D const&) = delete; // no operator=
			
			template<bool IsRN>
			void Simulate
			(
				time_t a_t0,
				time_t a_T,
				int    a_tauMins,
				long   a_P,
				bool   a_useTimerSeed,
				Diffusion1D const* a_diff,
				AProvider 	const* a_rateA,
				BProvider 	const* a_rateB,
				AssetClassA 	 a_assetA,
				AssetClassB		 a_assetB,
				PathEvaluator* a_PathEval
			);
	};
}
