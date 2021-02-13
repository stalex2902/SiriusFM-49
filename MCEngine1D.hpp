#pragma once

#include <random>
#include <cassert>

#include "MCEngine1D.h"

namespace SiriusFM {
	template
	<
		typename Diffusion1D,	typename AProvider,	typename BProvider,
		typename AssetClassA,	typename AssetClassB,	typename PathEvaluator
	>
	template<bool IsRN>
	inline void MCEngine1D
	<
		Diffusion1D, AProvider,	BProvider,
		AssetClassA, AssetClassB,	PathEvaluator
	>::
	Simulate
	(
		time_t a_t0, 
		time_t a_T, 
		int 	 a_tauMins, 
		long 	 a_P,
		bool 	 a_useTimerSeed,
		Diffusion1D const* a_diff, 
		AProvider 	const* a_rateA, 
		BProvider 	const* a_rateB, 
		AssetClassA a_assetA, 
		AssetClassB a_assetB,
		PathEvaluator* a_PathEval
	)
	{
		// check parameters` validity:
		assert(
			a_diff 	   	 	!= nullptr 
			&& a_rateA 	 	!= nullptr 
			&& a_rateB 	 	!= nullptr 
			&& a_assetA  	!= AssetClassA::UNDEFINED 
			&& a_assetB  	!= AssetClassB::UNDEFINED 
			&& a_t0 		 	<= a_T 
			&& a_tau_min 	> 0 
			&& a_P				> 0
			&& a_PathEval != nullptr);
		
		time_t T_sec = a_T - a_t0;
		time_t tau_sec = a_tauMins * SEC_IN_MIN;
		long L_ints = 
			(T_sec % tau_sec == 0) 
			? T_sec / tau_sec 
			: T_sec / tau_sec + 1; // number of intervals

		double tau = YearFracInt(tau_sec);

		double tlast = 
			(T_sec % tau_sec == 0) 
			? tau 
			: YearFracInt(T_sec - (L_ints - 1) * tau_sec);

		assert(tlast > 0 && tlast <= tau);
		long L = L_ints + 1; // number of points
		double stau = sqrt(tau);
		double slast = sqrt(tlast);
		assert(slast > 0 && slast <= stau);		
		double y0 = YearFrac(a_t0);
		assert(L >= 2); // at least 2 points
		long P = 2 * a_P; // antithetic variables
		
		if (L > m_MaxL)
			throw std::invalid_argument("invalid path parameters");

		std::normal_distribution<> N01(0.0, 1.0); 
																			// create standard normal distribution
		std::mt19937_64 U(a_useTimerSeed ? time(nullptr) : 0);
																			// uniform random number generator
		
		long PM = (m_MaxL * m_MaxPM) / L; // PM: # of paths stored in memory

		if (PM % 2 != 0)
			--PM;

		assert(PM > 0 && PM % 2 == 0);
		
		long PMh = PM / 2;

		// PI: # of outer P iterations:
		long PI = (P  % PM == 0) ? P / PM : (P / PM) + 1;
		
		// Construct the TimeLine:
		for (long l = 0; l < L - 1; ++l)
			m_ts[l] = y0 + double(L) * tau;

		m_ts[L - 1] = m_ts[L - 2] + tlast;

		// main simulation loop:
		for (long i = 0; i < PI; ++i) {

			// generate in-memory paths
			for (long p = 0; p < PMh; ++p) {
				
				double* path0 = m_paths + 2 * p * L;
				double* path1 = path0 + L;

				path0[0] = a_diff->GetS0(); // starting points
				path1[0] = a_diff->GetS0();

				double S_p0 = a_diff->GetS0(); // previous points
				double S_p1 = a_diff->GetS0();

				for (long l = 1; l < L; ++l) {
					// compute the trend:
					double mu0 = 0.0; // 2 paths
					double mu1 = 0.0;
					double y = m_ts[l - 1]; // l is the next point

					if (IsRN) { // risk-neutral case
						double delta_r = a_rateB->r(a_assetB, y) - a_rateA->r(a_assetA, y);
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
					double S_n0 = 0;
					double  S_n1 = 0;;

					if (l == L - 1) { // last interval
						S_n0 = S_p0 + mu0 * tlast + sigma0 * slast * Z;
						S_n1 = S_p1 + mu1 * tlast - sigma1 * slast * Z;
					}

					else { // generic case
						S_n0 = S_p0 + mu0 * tau + sigma0 * stau * Z;
						S_n1 = S_p1 + mu1 * tau - sigma1 * stau * Z;
					}

					path0[l] = S_n0;
					path1[l] = S_n1;
					S_p0 = S_n0;
					S_p1 = S_n1;
				} // end of l-loop
			} // end of p-loop

			// Evaluate the in-memory paths
			(*a_PathEval)(L, PM, m_paths, m_ts);
		} // end of i-loop
	}
}
