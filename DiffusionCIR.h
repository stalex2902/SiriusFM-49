#pragma once

#include <stdexcept>
#include <cmath>

namespace SiriusFM
{
	class DiffusionOU {
		private:
			double const m_kappa;
			double const m_theta;
			double const m_sigma;
		public:
			double mu(double a_S, double t) const
			{
				return (a_S < 0)? 0.0: m_kappa * (m_theta - a_S);
			}
			double sigma(double a_S, double t) const
			{
				return (a_S < 0)? 0.0: m_sigma * sqrt(a_S);
			}
			DiffusionGBM(double a_kappa, double a_theta, double a_sigma)
				: m_kappa(a_kappa),
				  m_theta(a_theta),
				  m_sigma(a_sigma);
				{
					if (m_sigma < 0) throw std::invalid_argument("invalid sigma");
					if (m_kappa <= 0) throw std::invalid_argument("invalid beta");
					if (m_theta <= 0) throw std::invalid_argument("invalid theta");
				}					
	}
}
