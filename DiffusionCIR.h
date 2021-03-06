//==========================================================================//
//                               "DiffusionCIR.h"                           //
// Diffusion model with mean-reverting trend and CIR vol (constant params)  //
//==========================================================================//

#pragma once

#include <stdexcept>
#include <cmath>

namespace SiriusFM {
	class DiffusionCIR {
		private:
			double const m_kappa;
			double const m_theta;
			double const m_sigma;
			double const m_S0;

		public:
			DiffusionCIR(double a_kappa, double a_theta, 
																					double a_sigma, double a_S0)
			:	m_kappa(a_kappa),
				m_theta(a_theta),
				m_sigma(a_sigma),
				m_S0(a_S0)
				{
					if (m_sigma < 0) throw std::invalid_argument("invalid sigma");
					if (m_kappa <= 0) throw std::invalid_argument("invalid beta");
					if (m_theta <= 0) throw std::invalid_argument("invalid theta");
					if (m_S0 < 0) throw std::invalid_argument("invalid S0");
				}

			double mu(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_kappa * (m_theta - a_S);
			}

			double sigma(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_sigma * sqrt(a_S);
			}
			
			double GetS0() const {
				return m_S0;
			}
	};
}
