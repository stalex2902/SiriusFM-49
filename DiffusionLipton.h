//==========================================================================//
//                             "DiffusionLipton.h"                          //
// Diffusion model with linear trend and Lipton-type vol                    //
//==========================================================================//

#pragma once

#include <stdexcept>
#include <cmath>

namespace SiriusFM {
	class DiffusionLipton {
		private:
			double const m_mu;
			double const m_sigma0;
			double const m_sigma1;
			double const m_sigma2;
			double const m_S0;

		public:
			DiffusionLipton(double a_mu, double a_sigma0, double a_sigma1, 
							double a_sigma2, double a_S0)
			: m_mu(a_mu),
				m_sigma0(a_sigma0),
				m_sigma1(a_sigma1),
				m_sigma2(a_sigma2),
				m_S0(a_S0)
				{
					if (m_sigma0 < 0 || m_sigma2 < 0 
						|| pow(m_sigma1, 2) - 4 * m_sigma0 * m_sigma2 > 0) 
						throw std::invalid_argument("invalid structure of vol");
					if (m_S0 < 0) throw std::invalid_argument("invalid S0");
				}

			double mu(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_mu * a_S;
			}

			double sigma(double a_S, double t) const {
				return (a_S < 0)? 0.0: m_sigma0 + m_sigma1 * a_S + m_sigma2 * pow(a_S, 2);
			}

			double GetS0() const {
				return m_S0;
			}
	};
}
