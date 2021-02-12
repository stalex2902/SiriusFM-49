#pragma once

namespace SiriusFM {

	constexpr int SEC_IN_MIN 		  = 60;
	constexpr int SEC_IN_DAY 		  = 86'400;
	constexpr double AVG_DAYS_IN_YEAR = 365.25;
	constexpr double EPOCH_BEGIN 	  = 1970.0;
	constexpr double AVG_SEC_IN_YEAR  = AVG_DAYS_IN_YEAR * SEC_IN_DAY;

	inline double YearFrac(time_t a_t) {
		return EPOCH_BEGIN + double(a_t) / AVG_SEC_IN_YEAR;
	}
	
	inline double YearFracInt(time_t a_t) {
		return double(a_t) / AVG_SEC_IN_YEAR;
	}
}
