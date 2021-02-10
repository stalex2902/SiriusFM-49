/* testing vanilla option pricing with Monte-Carlo */

#include <iostream>

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "VanillaOption.h"

using namespace SiriusFM;
using namespace std; 

int main(int argc, char* argv[]) {
	if (argc != 9) {
		cerr << "PARAMS: mu, sigma, S0, OptType, K, T_days, tau_min, P" << endl;	
		return 1;
	}

	double mu = atof(argv[1]);
	double sigma = atof(argv[2]);
	double S0 = atof(argv[3]);
	char* OptType = argv[4];
	double K = atof(argv[5]);
	long T_days = atol(argv[6]);
	int tau_min = atoi(argv[7]);
	long P = atol(argv[8]);

	if (sigma <= 0 || S0 <= 0 || OptType == nullptr || K <= 0 || T_days <= 0, tau_min <= 0 || P <= 0) {
		cerr << "Invalid parameters" << endl;
		return 2;
	}

	Option const* option;
	if (strcmp(OptType, "Call") == 0)
		option = new EurCallOption(K, T_days);
	else if (strcmp(OptType, "Put") == 0)
		option = new EurPutOption(K, T_days);
	else
		throw invalid_argument("Invalid OptType");
		
	CcyE ccyA = CcyE::USD;
	
	IRProvider<IRModeE::Const> irp(nullptr);

	DiffusionGBM diff(mu, sigma, S0);

	MCEngine1D<decltype(diff), decltype(irp), decltype(irp), CcyE, CcyE> mce(20'000, 20'000);
	
	time_t t0 = time(nullptr);
	time_t T = t0 + T_days * 86'400;
	double T_years = double(T_days) / 365.25;

	// run MC:
	mce.Simulate<false>(t0, T, tau_min, P, &diff, &irp, &irp, ccyA, ccyA);

	// analyze results:
	auto res = mce.GetPaths();

	long L1 = get<0>(res);
	long P1 = get<1>(res);
	double const* paths = get<2>(res);

	// compute expected value of ST
	double E_ST = 0.0;

	for (long p = 0; p < P1; ++p) {
		double const* path = paths + p * L1; // current path
		double RT = option->Payoff(L1, nullptr, path); // ts - TODO
		E_ST += RT;
	}
	E_ST /= double(P1); // est of (mu - sigma^2/2) * T
	E_ST *= exp((-1) * irp.r(ccyA, 0) * T_years);
	cout << "Option price is " << E_ST << endl;
	return 0;
}
