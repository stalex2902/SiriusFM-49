//==========================================================================//
//                                "BSM.hpp"                                 //
// Black-Scholes-Merton formulas for vanilla options                        //
//==========================================================================//

#pragma once

#include "VanillaOption.h"

using namespace SiriusFM;
using namespace std;

namespace SiriusFM {
	
	//------------------------------------------------------------------------//
	// CDF of the Standard Normal:                                            //
	//------------------------------------------------------------------------//
	inline double Phi(double x) {
		return 0.5 * (1 + erf(x / M_SQRT2));
	}

	//------------------------------------------------------------------------//
	// BSM Pricer (analytical solution with GBM):                             //
	//------------------------------------------------------------------------//
	inline double BSMPxCall(double a_S0, double a_K, double a_TTE,
						double a_rateA, double a_rateB, double a_sigma) {
		assert(a_S0 > 0 && a_K > 0 && a_sigma > 0);

		if (a_TTE <= 0)
			// return payoff:
			return std::max<double>(a_S0 - a_K, 0);

		double xd = a_sigma * sqrt(a_TTE);
		double x1 = (log(a_S0 / a_K) + 
			(a_rateB - a_rateA + a_sigma * a_sigma / 2.0) * a_TTE) / xd;
		double x2 = x1 - xd; // ???
		double px = a_S0 * exp(- a_rateA * a_TTE) * Phi(x1) 
								- a_K * exp(- a_rateB * a_TTE) * Phi(x2);
		return px;
	}

	inline double BSMPxPut(double a_S0, double a_K, double a_TTE,
						double a_rateA, double a_rateB, double a_sigma) {
		
		// using Call-Put parity
		double px = BSMPxCall(a_S0, a_K, a_TTE, a_rateA, a_rateB, a_sigma) - a_S0 
													+ exp(- a_rateB * a_TTE) * a_K;
		assert(px > 0.0);
		return px;
	}

	// Deltas for Call and Put:
	inline double BSMDeltaCall(double a_S0, double a_K, double a_TTE,
							double a_rateA, double a_rateB, double a_sigma) {
		assert(a_S0 > 0 && a_K > 0 && a_sigma > 0);
		
		if (a_TTE <= 0)
			return  (a_S0 < a_K) ? 0 : (a_S0 > a_K) ? 1 : 0.5;

		double xd = a_sigma * sqrt(a_TTE);
		double x1 = (log(a_S0 / a_K) + 
					(a_rateB - a_rateA + a_sigma * a_sigma / 2.0) * a_TTE) / xd;
    	return Phi(x1);
	}

	inline double BSMDeltaPut (double a_S0, double a_K, double a_TTE,
								double a_rateA, double a_rateB, double a_sigma) {

		// from Call-Put parity:
		return BSMDeltaCall(a_S0, a_K, a_TTE, a_rateA, a_rateB, a_sigma) - 1;
	}

}
