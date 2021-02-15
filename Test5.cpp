//==========================================================================//
//                               "Test5.cpp"                                //
// Testing GridNOP1D_S3_RKC1                                                //
//==========================================================================//

#include "DiffusionGBM.h"
#include "VanillaOption.h"
#include "GridNOP1D_S3_RKC1.hpp"
#include "IRProviderConst.h"

#include <iostream>

using namespace SiriusFM;
using namespace std;

int main(int argc, char** argv) {

	if (argc != 10) {
		cerr << "PARAMS:\nsigma, S0,\n{Call/Put}, K, Tdays," 
						"\nisAmerican, ratesFile \nNS, tauMins\n";
		return 1;
	}

	double sigma 					= atof(argv[1]);
	double S0 						= atof(argv[2]);
	const char* OptType 	= argv[3];
	double K 							= atof(argv[4]);
	long Tdays 						= atol(argv[5]);
	bool isAmerican 			= bool(atoi(argv[6]));
	const char* ratesFile = argv[7];
	int NS 								= atof(argv[8]);
	int tauMins 					= atoi(argv[9]);

	assert(sigma > 0 && S0 > 0 && Tdays > 0 
						&& tau_mins > 0 && NS > 0);

	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::RUB;

	DiffusionGBM diff(0.0, sigma, S0); // Trend is irrelevamt here

	// create the option spec:
	time_t t0 = time(nullptr);				  // abs start time
	time_t T = t0 + SEC_IN_DAY * Tdays; // abs expir time

	OptionFX const* opt = nullptr;
	
	if (strcmp(OptType, "Call") == 0)
		opt = new CallOptionFX(ccyA, ccyB, K, T, isAmerican);

	else if (strcmp(OptType, "Put")  == 0)
		opt = new PutOptionFX(ccyA, ccyB, K, T, isAmerican);

	else
		throw invalid_argument("Bad option type");

	// Construct the Grid Pricer (with default Max Geometry):
	GridNOP1D_S3_RKC1<decltype(diff), IRPConst, IRPConst, CcyE, CcyE>
		grid(ratesFile, ratesFile);

  // Presto! Run Backward Induction on the Grid (with default BFactor):
  grid.Run<true>(opt, &diff, S0, t0, NS, tauMins);

	// get the (px, delta, gamma) at t = 0
	auto res = grid.GetPxDeltaGamma0();
	
	double px 	 = get<0>(res);
	double delta = get<1>(res);
	double gamma = get<2>(res);

	cout << "Px = " << px << " , delta = " << delta
																				<< ", gamma = " << gamma << endl;
  delete opt;
	return 0;
}
