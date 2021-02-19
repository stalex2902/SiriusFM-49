//==========================================================================//
//                                "Test4.cpp"                               //
// Testing MCOptionHedger1D: evaluating portfolio with hedged options       //
//==========================================================================//

#include "DiffusionGBM.h"
#include "BSM.hpp"
#include "MCOptionHedger1D.hpp"

using namespace SiriusFM;
using namespace std;

int main(int argc, char** argv) {
	if(argc != 10) {
		cerr << "params: mu, sigma, S0,\nCall/Put, K, Tdays,\ndeltaAcc" 
																													"\ntau_mins, P\n";
		return 1;
	}

	double 			mu 			 = atof(argv[1]);
	double 			sigma 	 = atof(argv[2]);
	double 			S0 			 = atof(argv[3]);
	const char* OptType  = 			argv[4];
	double 			K 			 = atof(argv[5]);
	long 				T_days 	 = atol(argv[6]);
	double 			deltaAcc = atof(argv[7]);
	int 				tau_mins = atoi(argv[8]);
	long 				P 			 = atol(argv[9]);

	assert(sigma > 0 && S0 > 0 && T_days > 0 
						&& tau_mins > 0 && P > 0 && K > 0);

	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::USD;

	char const* ratesFileA = nullptr;
	char const* ratesFileB = nullptr;
	bool useTimerSeed = true;

	DiffusionGBM diff(mu, sigma, S0);

	// The following Hedger is for FX (CcyE / CcyE):
	MCOptionHedger1D<decltype(diff), IRPConst, IRPConst, CcyE, CcyE> 
		hedger(&diff, ratesFileA, ratesFileB, useTimerSeed);

	// Create the Option spec:
	time_t t0 = time(nullptr);   		  		// abs start time
	time_t T  = t0 + SEC_IN_DAY * T_days; // abs expir time in sec since epoch
	double TTE = YearFracInt(T - t0);		  // time to expir in sec
	double Ty = EPOCH_BEGIN + double(T) / AVG_SEC_IN_YEAR;
																				// expir time as YYYY.YearFrac

	OptionFX const* opt = nullptr;
	decltype(hedger)::DeltaFunc const* deltaFunc = nullptr;

	double C0 = 0.0;

	// rates are const here:
	double rateA = hedger.GetRateA(ccyA, 0.0); // any t
	double rateB = hedger.GetRateB(ccyB, 0.0);

	// Closures for deltas:
	function<double(double, double)> deltaCall
	(
		[K, Ty, rateA, rateB, sigma]
		(double a_St, double a_t) -> double {
			double currTTE = Ty - a_t;
			return BSMDeltaCall(a_St, K, currTTE, rateA, rateB, sigma);
		}
	);

	function<double(double, double)> deltaPut
	(
		[K, Ty, rateA, rateB, sigma]
		(double a_St, double a_t) -> double {
			double currTTE = Ty - a_t;
			return BSMDeltaPut(a_St, K, currTTE, rateA, rateB, sigma);
		}
	);

  	if (strcmp(OptType, "Call") == 0) {
		opt = new CallOptionFX(ccyA, ccyB, K, T, false); // isAmerican=false
		deltaFunc = &deltaCall;
		C0 = BSMPxCall(S0, K, TTE, rateA, rateB, sigma);
	}

	else if (strcmp(OptType, "Put") == 0) {
		opt = new PutOptionFX (ccyA, ccyB, K, T, false);
	  	deltaFunc = &deltaPut;
	  	C0 = BSMPxPut(S0, K, TTE, rateA, rateB, sigma);
	}

	else
		throw invalid_argument("Invalid option type");

	// Presto! Run the Hedger!
	auto res = hedger.SimulateHedging(opt, t0, C0, deltaFunc, 
																						deltaAcc, tau_mins, P);

	double EPnL   = get<0>(res);
	double StDPnL = get<1>(res);
	double minPnL = get<2>(res);
	double maxPnL = get<3>(res);
  
	cout << "E[PnL] = " << EPnL << ", StD[PnL] = " << StDPnL << ", Max[Pnl] = " 
									<< maxPnL << ", Min[PnL] = " << minPnL << endl; 
	delete opt;
	return 0;
}
