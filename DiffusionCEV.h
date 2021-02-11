#pragma once

#include <stdexcept>
#include <cmath>

namespace SiriusFM
{
	class DiffusionCEV {
		private:
			double const m_mu;
			double const m_sigma;
			double const m_beta;
			double const m_S0;
		public:
			DiffusionCEV(double a_mu, double a_sigma, double a_beta, double a_S0):
				m_mu(a_mu),
				m_sigma(a_sigma),
				m_beta(a_beta),
				m_S0(a_S0)
				{
					if (m_sigma < 0) throw std::invalid_argument("invalid sigma");
					if (m_beta < 0) throw std::invalid_argument("invalid beta");
					if (m_S0 < 0) throw std::invalid_argument("invalid S0");
				}

			double mu(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_mu * a_S;
			}

			double sigma(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_sigma * pow(a_S, m_beta);
			}
			double GetS0() const {
				return m_S0;
			}
	};
}
