/* testing GBM diffusion with Monte-Carlo */

#include <iostream>

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"

using namespace SiriusFM;
using namespace std; // only in main

int main(int argc, char* argv[]) {
	if (argc != 7) {
		cerr << "PARAMS: mu, sigma, S0, T_days, tau_min, P" << endl;	
		return 1;
	}

	double mu = atof(argv[1]);
	double sigma = atof(argv[2]);
	double S0 = atof(argv[3]);
	long T_days = atol(argv[4]);
	int tau_min = atoi(argv[5]);
	long P = atol(argv[6]);

	if (sigma <= 0 || S0 <= 0 || T_days <= 0, tau_min <= 0 || P <= 0) {
		cerr << "Invalid parameters" << endl;
		return 2;
	}

	CcyE ccyA = CcyE::USD;
	
	IRProvider<IRModeE::Const> irp(nullptr);

	DiffusionGBM diff(mu, sigma);

	MCEngine1D<decltype(diff), decltype(irp), decltype(irp), CcyE, CcyE> mce(20000, 20000);
	
	time_t t0 = time(nullptr);
	time_t T = t0 + T_days * 86000;
	double T_years = double(T_days) / 365.25;	

	// run MC:
	mce.Simulate<false>(t0, T, tau_min, P, S0, &diff, &irp, &irp, ccyA, ccyA);

	// analyze results:
	auto res = mce.GetPaths();

	long L1 = get<0>(res);
	long P1 = get<1>(res);
	double const* paths = get<2>(res);

	// compute expected value and variance of log ST:
	double E_ST = 0.0;
	double E_ST2 = 0.0; // expected value of (log ST)^2
	int N_VP = 0; // # of valid paths

	for (long p = 0; p < P1; ++p) {
		double const* path = paths + P * L1; // current path
		double ST = path[L1 - 1]; // end-point of current path
		// in practise, may get ST <= 0
		if (ST <= 0.0)
			continue;
		++N_VP;
		double RT = log(ST / S0); // log-result
		E_ST += RT;
		E_ST2 += RT * RT;
	}
	assert(N_VP > 1); // at least 1 valid path
	E_ST /= double(N_VP); // est of (mu - sigma^2/2) * T
	double Var_ST = (E_ST2 - double(N_VP) * E_ST * E_ST) / double(N_VP - 1); // est of sigma^2 * T
	double sigma2E = Var_ST / T_years;
	double muE = (E_ST + Var_ST / 2.0) / T_years;
	cout << "mu = " << mu << ", muE = " << muE << endl;
	cout << "sigma^2 = " << sigma * sigma << ", sigma^2E = " << sigma2E << endl;
	return 0;
}
