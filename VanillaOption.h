#pragma once

#include <algorithm>

#include "Option.h"

namespace SiriusFM {
	class EurCallOption final: public Option {
		private:
			double const m_K;
		public:
			EurCallOption(double a_K, int a_Tdays):
			Option(false, a_Tdays),
			m_K(a_K) {
				if (m_K <= 0)
					throw std::invalid_argument("K must be positive");
			}
			~EurCallOption() override {}
			double Payoff(long a_l, double const* a_t, double const* a_S) const override {
				assert(a_l > 0 && a_S != nullptr); // TODO: && a_T != nullptr
				return std::max<double>(a_S[a_l - 1] - m_K, 0.0);
			}
	};
	
	class EurPutOption final: public Option {
		private:
			double const m_K;
		public:
			EurPutOption(double a_K, int a_Tdays):
			Option(false, a_Tdays),
			m_K(a_K) {
				if (m_K <= 0)
					throw std::invalid_argument("K must be positive");
			}
			~EurPutOption() override {}
			double Payoff(long a_l, double const* a_t, double const* a_S) const override {
				assert(a_l > 0 && a_S != nullptr);
				return std::max<double>(m_K - a_S[a_l - 1], 0.0);
			}
	};
}
