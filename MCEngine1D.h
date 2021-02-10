#pragma once

#include <cmath>
#include <stdexcept>
#include <ctime>
#include <new>
#include <tuple>

namespace SiriusFM {
	template<typename Diffusion1D, typename AProvider, typename BProvider, typename AssetClassA, typename AssetClassB>
	class MCEngine1D {
		private:
			long const m_MaxL; // max path length
			long const m_MaxP; // max # of paths
			double* const m_paths;
			long m_L; // real path length
			long m_P; // real # of paths

		public:
			MCEngine1D(long a_MaxL, long a_MaxP):
			m_MaxL(a_MaxL),
			m_MaxP(a_MaxP),
			m_paths(new double[m_MaxL * m_MaxP]),
			m_L(0),
			m_P(0) 
			{
				if (m_MaxL <= 0 || m_MaxP <= 0)
					throw std::invalid_argument("invalid max path parameters");
			}

			~MCEngine1D() {
				delete[] m_paths;
			}

			MCEngine1D(MCEngine1D const&) = delete; // no copy-constructor

			MCEngine1D& operator=(MCEngine1D const&) = delete; // no operator=
			
			template<bool IsRN>
			void Simulate(time_t a_t0, time_t a_T, int a_tau_min, long a_P, Diffusion1D const* a_diff, AProvider const* a_rateA, BProvider const* a_rateB, AssetClassA a_A, AssetClassB a_B);

			std::tuple<long, long, double const*> GetPaths() const { // getter
				return (m_L <= 0 || m_P <= 0)
				? std::make_tuple(0, 0, nullptr)
				: std::make_tuple(m_L, m_P, m_paths);
			}
	};
}
