#pragma once

#include <ctime>

namespace SiriusFM {
	inline double YearFrac(time_t a_t) {
		constexpr double SecY = 365.25 * 86400; // average year in seconds
		return 1970.0 + double(a_t) / SecY;
	}
}
