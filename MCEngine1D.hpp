#pragma once

#include <random>
#include <cassert>

#include "MCEngine1D.h"
#include <Time.h>

namespace SiriusFM {
	template<typename Diffusion1D, typename AProvider, typename BProvider, typename AssetClassA, typename AssetClassB>
	inline void MCEngine1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::
	simulate(time_t a_t0, time_t a_T, int a_tau_min, long a_P, double a_S0, Diffusion1D const* a_diff, AProvider const* a_rateA, BProvider const* a_rateB, AssetClassA a_A, AssetClassB a_B, bool a_is_RN) {
		// check parameters` validity:
		assert(a_diff != nullptr && a_rateA != nullptr && a_rateB != nullptr && a_A != AssetClassA::UNDEFINED && a_B != AssetClassB::UNDEFINED && a_t0 <= a_T && a_tau_min > 0 && a_P > 0);		

		double y0 = YearFrac(a_t0);
		double yT = YearFrac(a_T);
		double tau = double(a_tau_min) / (365.25 * 1440.0);
		long L = long(ceil((yT - y0) / tau)) + 1; // path length (number of points)
		assert(L >= 2); // at least 2 points
		long P = 2 * a_P; // antithetic variables

		if (L * P > m_MaxL * m_MaxP)
			throw std::invalid_argument("invalid path parameters");

		std::normal_distribution<> N01(0.0, 1.0); // create standard normal distribution
		std::mt19937_64 U; // uniform random number generator

		double stau = sqrt(tau);
		double tlast = yT - y0 - double(l - 2) * tau; // last time interval
		double slast = sqrt(tlast);
		assert(slast <= stau && slast > 0);

		// main loop:
		for (long p = 0; p < a_P; ++p) {
			double* path0 = m_paths + 2 * p * L;
			double* path1 = path0 + L;
			path0[0] = a_S0;
			path1[0] = a_S0;
			double y = y0;
			double S_p0 = a_S0; // previous points
			double S_p1 = a_S0;

			for (long l = 1; l < L; ++l) {
				// compute the trend:
				double mu0 = 0.0; // 2 paths
				double mu1 = 0.0;

				if (a_is_RN) { // risk-neutral case
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
					S_n1 = S_p1 + mu1 * tlast - sigma1 + slast * Z;
					y += tlast;
				}
				else { // generic case
					S_n0 = S_p0 + mu0 * tau + sigma0 * stau * Z;
					S_n1 = S_p1 + mu1 * tau + sigma1 * stau * Z;
					y += tau;
				}
				path0[l] = S_n0;
				path1[l] = S_n1;
				S_p0 = S_n0;
				S_p1 = S_n1;
				
			} // end of l-loop
		} // end of p-loop
	}
}
