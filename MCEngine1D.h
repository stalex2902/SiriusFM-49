#pragma once

#include <cmath>
#include <stdexcept>
#include <ctime>

namespace SiriusFM {
	template<typename Diffusion1D, typename AProvider, typename BProvider, typename AssetClassA, typename AssetClassB>
	class MCEngine1D {
		private:
			long const m_MaxL; // max path length
			long const m_MaxP; // max # of paths
			double* const m_paths;
/*
			long m_L; // m_L <= m_MaxL
			long m_P; // m_P <= m_MaxP
			double m_tau; // time step as year fraction
			double m_t0; // 2021.xxxx (year fraction)
			Diffusion1D const* m_diff;
			AProvider const* m_rateA;
			BProvider const* m_rateB;
			AssetClassA m_A; // asset A
			AssetClassB m_B; // asset B
			bool m_is_RN; // risk-neutral trend (TRUE for option-pricing)
*/
		public:
			MCEngine1D(long a_MaxL, long a_MaxP):
			m_MaxL(a_MaxL),
			m_MaxP(a_MaxP),
			m_paths(new double[m_MaxL * m_MaxP])
/*
			m_L(0),
			m_P(0),
			m_tau(nan),
			m_t0(nan),
			m_diff(nullptr),
			m_rateA(nullptr),
			m_rateB(nullptr),
			m_a(AssetClassA::UNDEFINED),
			m_b(AssetClassB::UNDEFINED),
			m_RN(false)
*/
			{
				if (m_MaxL <= 0 || m_MaxP <= 0)
					throw std::invalid_argument("invalid max path parameters");
			}
			void Simulate(time_t a_t0, time_t a_T, int a_tau_min, long a_P, double a_S0, Diffusion1D const* a_diff, AProvider const* a_rateA, BProvider const* a_rateB, AssetClassA a_A, AssetClassB a_B, bool m_is_RN);
	};
}
