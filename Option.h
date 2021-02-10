#pragma once

#include <stdexcept>

namespace SiriusFM {
	class Option {
		protected:
			bool const m_isAmerican;
			int const m_Tdays; // expiration time > 0
			Option(bool a_isAmerican, int a_Tdays): 
				m_isAmerican(a_isAmerican), 
				m_Tdays(a_Tdays) {
				if (m_Tdays <= 0)
					throw std::invalid_argument("Tdays must be positive");
			}
		public:
			virtual double Payoff(long a_L, double const* a_t, double const* a_S) const = 0;
			bool IsAmerican() const {
				return m_isAmerican;
			}
			virtual ~Option() {}
	};
}
