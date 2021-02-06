#include <cstdio>

#include "IRProviderConst.h"

namespace SiriusFM {
	IRProvider<IRModeE::Const>::IRProvider(char const* a_file) {
		FILE* source = fopen(a_file, r);
		char buff[128];
		double r[CcyE::N] = {0};
		fgets(buff, 128, a_file);
		CcyE ccy = str2Ccy(buff);
	}
}
