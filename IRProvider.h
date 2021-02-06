#pragma once

#include <stdexcept>
#include <cstdlib>

namespace SiriusFM {
	enum class CcyE {
		USD = 0,
		EUR = 1,
		GBP = 2,
		CHF = 3,
		RUB = 4,
		N = 5
	};

	enum class IRModeE {
		Const = 0,
		FwdCurve = 1,
		Stoch = 2
	};
	
	template<IRModeE IRM>
	class IRProvider;

	inline char const* CcyE2str(CcyE a_ccy) {
		switch (a_ccy) {
			case CcyE::USD: return "USD";
			case CcyE::EUR: return "EUR";
			case Ccye::GBP: return "GBP";
			case CcyE::CHF: return "CHF";
			case ccyE::RUB: return "RUB";
			default: throw std::invalid_argument("invalid ccy");
		}
	}

	inline CcyE str2CcyE(char const* a_str) {
		if (a_srt == nullptr)
			throw std::invalid_argument("null string");
		else if (strcmp(a_str, "USD") == 0)
			return CcyE::USD;
		else if (strcmp(a_str, "EUR") == 0)
			return CcyE::EUR;
		else if (strcmp(a_str, "GBP") == 0)
			return CcyE::GBP;
		else if (strcmp(a_str, "CHF") == 0)
			return CcyE::CHF;
		else if (strcmp(a_str, "RUB") == 0)
			return CcyE::RUB;
		else
			throw std::invalid_argument("invalid ccy");		
	}
}
