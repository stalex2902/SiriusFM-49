#include <iostream>

#include "DiffusionGBM.h"

int main() {
	SiriusFM::DiffusionGBM Diffusion(1.0, 1.0);
	std::cout << Diffusion.mu(-1.0, 1.0) << std::endl;
	return 0;
}
