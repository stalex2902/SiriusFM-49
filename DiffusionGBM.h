#pragma once

#include <stdexcept>

namespace SiriusFM
{
	class DiffusionGBM {
		private:
			double const m_mu;
			double const m_sigma;
		public:
			double mu(double a_S, double t) const 
			{
				return (a_S < 0)? 0.0: m_mu * a_S;
			}
			double sigma(double a_S, double t) const
			{
				return (a_S < 0)? 0.0: m_sigma * a_S;
			}
			DiffusionGBM(double a_mu, double a_sigma)
				: m_mu(a_mu),
				  m_sigma(a_sigma)
				{
					if (m_sigma < 0) throw std::invalid_argument("invalid sigma");
				}					
	}
}
