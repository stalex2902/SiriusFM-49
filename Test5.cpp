//==========================================================================//
//                               "Test5.cpp"                                //
// Testing GridNOP1D_S3_RKC1                                                //
//==========================================================================//

#include "DiffusionGBM.h"
#include "VanillaOption.h"
#include "GridOP1D_S3_RKC1.hpp"

using namespace SiriusFM;
using namespace std;

int main(int argc, char const* argv[]) {

	if (argc != 8) {
		cerr << "PARAMS:\nsigma, S0,\n{Call/Put}, K, Tdays,\nNS, tauMins\n";
	}

	double sigma 				= atof(argv[1]);
	double S0 					= atof(argv[2]);
	const char* OptType = argv[3];
	double K 						= atof(argv[4]);
	long Tdays 					= atol(argv[5]);
	int NS 							= atof(argv[6]);
	int tauMins 				= atoi(argv[7]);

	assert(sigma > 0 && S0 > 0 && Tdays > 0 
						&& tau_mins > 0 && NS > 0);

	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::RUB;

	char const* ratesFileA = nullptr;
	char const* ratesFileB = nullptr;

	DiffusionGBM diff(0.0, sigma, S0); // Trend is irrelevamt here

	// create the option spec:
	time_t t0 = time(nullptr);				  // ans start time
	time_t T = t0 + SEC_IN_DAY * Tdays; // abs expir time

	Option const* opt = nullptr;
	
	if (strcmp(OptType, "Call") == 0)
		opt = new EurCallOptionFX(ccyA, ccyB, K, T);

	else if (strcmp(OptType, "Put")  == 0)
		opt = new EurPutOptionFX (ccyA, ccyB, K, T);

	else
		throw invalid_argument("Bad option type");

	// Construct the Grid Pricer (with default Max Geometry):
	GridNOP1D_S3_RKC1<decltype(diff), IRPConst, IRPConst, CcyE, CcyE>
		grid(ratesFileA, ratesFileB);

  // Presto! Run Backward Induction on the Grid (with default BFactor):
  grid.RunBI(opt, &diff, S0, t0, NS, tauMins);

  delete opt;

	return 0;
}
