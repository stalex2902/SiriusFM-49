#pragma once

#include <random>
#include <cassert>

#include "MCEngine1D.h"
#include "Time.h"

namespace SiriusFM {
	template<typename Diffusion1D, typename AProvider, typename BProvider, typename AssetClassA, typename AssetClassB>
	template<bool IsRN>
	inline void MCEngine1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::
	Simulate(time_t a_t0, time_t a_T, int a_tau_min, long a_P, Diffusion1D const* a_diff, AProvider const* a_rateA, BProvider const* a_rateB, AssetClassA a_A, AssetClassB a_B) {
		// check parameters` validity:
		assert(a_diff != nullptr && a_rateA != nullptr && a_rateB != nullptr && a_A != AssetClassA::UNDEFINED && a_B != AssetClassB::UNDEFINED && a_t0 <= a_T && a_tau_min > 0 && a_P > 0);
		
		time_t T_sec = a_T - a_t0;
		time_t tau_sec = a_tau_min * 60;
		long L_ints = (T_sec % tau_sec == 0) ? T_sec / tau_sec : T_sec / tau_sec + 1; // number of intervals
		double tau = YearFracInt(tau_sec);
		double tlast = (T_sec % tau_sec == 0) ? tau : YearFracInt(T_sec - (L_ints - 1) * tau_sec);
		assert(tlast > 0 && tlast <= tau);
		long L = L_ints + 1; // number of points
		double stau = sqrt(tau);
		double slast = sqrt(tlast);
		assert(slast > 0 && slast <= stau);		
		double y0 = YearFrac(a_t0);
		assert(L >= 2); // at least 2 points
		long P = 2 * a_P; // antithetic variables

		if (L * P > m_MaxL * m_MaxP)
			throw std::invalid_argument("invalid path parameters");

		std::normal_distribution<> N01(0.0, 1.0); // create standard normal distribution
		std::mt19937_64 U; // uniform random number generator

		// main loop:
		for (long p = 0; p < a_P; ++p) {
			double* path0 = m_paths + 2 * p * L;
			double* path1 = path0 + L;
			path0[0] = a_diff->GetS0();
			path1[0] = a_diff->GetS0();
			double y = y0;
			double S_p0 = a_diff->GetS0(); // previous points
			double S_p1 = a_diff->GetS0();

			for (long l = 1; l < L; ++l) {
				// compute the trend:
				double mu0 = 0.0; // 2 paths
				double mu1 = 0.0;

				if (IsRN) { // risk-neutral case
					double delta_r = a_rateB->r(a_B, y) - a_rateA->r(a_A, y);
					mu0 = delta_r * S_p0;
					mu1 = delta_r * S_p1;
				}
				else {
					mu0 = a_diff->mu(S_p0, y);
					mu1 = a_diff->mu(S_p1, y);
				}
				// compute volatility
				double sigma0 = a_diff->sigma(S_p0, y);
				double sigma1 = a_diff->sigma(S_p1, y);
				
				// generate points
				double Z = N01(U);
				double S_n0, S_n1;

				if (l == L - 1) { // last interval
					S_n0 = S_p0 + mu0 * tlast + sigma0 * slast * Z;
					S_n1 = S_p1 + mu1 * tlast - sigma1 * slast * Z;
					y += tlast;
				}
				else { // generic case
					S_n0 = S_p0 + mu0 * tau + sigma0 * stau * Z;
					S_n1 = S_p1 + mu1 * tau - sigma1 * stau * Z;
					y += tau;
				}
				path0[l] = S_n0;
				path1[l] = S_n1;
				S_p0 = S_n0;
				S_p1 = S_n1;
				
			} // end of l-loop
		} // end of p-loop
		m_L = L;
		m_P = P;
	}
}
