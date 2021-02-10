#pragma once

#include <stdexcept>

namespace SiriusFM
{
	class DiffusionGBM {
		private:
			double const m_mu;
			double const m_sigma;
			double m_S0;
		public:
			DiffusionGBM(double a_mu, double a_sigma, double a_S0):
			m_mu(a_mu),
			m_sigma(a_sigma),
			m_S0(a_S0) {
				if (m_sigma < 0 || m_S0 < 0) throw std::invalid_argument("invalid params");
			}

			double mu(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_mu * a_S;
			}

			double sigma(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_sigma * a_S;
			}
			
			double GetS0() const {
				return m_S0;
			}
	};
}
