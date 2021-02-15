#pragma once

#include "MCOptionHedger1D.h"
#include "MCEngine1D.hpp"

namespace SiriusFM {

	//========================================================================//
	// MCOptionHedger1D::SimulateHedging:                                     //
	//========================================================================//	

	template
	<
		typename Diffusion1D, typename AProvider, typename BProvider,
		typename AssetClassA, typename AssetClassB	
	>
	std::tuple<double, double, double, double> 
	MCOptionHedger1D<Diffusion1D, AProvider, BProvider,
							AssetClassA, AssetClassB>::
	SimulateHedging
	(
		Option<AssetClassA, AssetClassB> const* a_option,
		time_t a_t0,
		double a_C0,
		DeltaFunc const* a_deltaFunc,
		double a_deltaAcc,
		int a_tauMins,
		long a_P
	)
	{
		assert(a_option != nullptr && a_tauMins > 0 && a_P > 0 
										&& a_deltaFunc != nullptr && a_deltaAcc > 0);
		
		// Path Evaluator:
		OHPathEval pathEval(a_option, &m_irpA, &m_irpB, a_C0,
																									a_deltaFunc, a_deltaAcc);

		// run MC in REAL measure and return the stats:
		m_mce.template Simulate<false> // isRN = false
		(a_t0, a_option->m_expirTime, a_tauMins, a_P, m_useTimerSeed, m_diff,
				&m_irpA, &m_irpB, a_option->m_assetA, a_option->m_assetB, &pathEval);
		
		// get PnL stats from Path Eval and return:
		return pathEval.GetStats();
	}
}
