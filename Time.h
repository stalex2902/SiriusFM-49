#pragma once

#include <ctime>

namespace SiriusFM {

	constexpr int SEC_IN_MIN = 60;
	constexpr int SEC_IN_DAY = 86'400;
	constexpr double AVG_DAYS_IN_YEAR = 365.25;
	constexpr double EPOCH_BEGIN = 1970.0;

	inline double YearFrac(time_t a_t) {
		constexpr double SecY = AVG_DAYS_IN_YEAR * SEC_IN_DAY; // average year in seconds
		return EPOCH_BEGIN + double(a_t) / SecY;
	}
	
	inline double YearFracInt(time_t a_t) {
		constexpr double SecY = 365.25 * 86400;
		return double(a_t) / SecY;
	}
}
