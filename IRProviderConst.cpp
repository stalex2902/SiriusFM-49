#include <cstdio>
#include <cstdlib>

#include "IRProviderConst.h"

namespace SiriusFM {
	IRProvider<IRModeE::Const>::IRProvider(char const* a_file) {
		char buff[255];
		m_IRs[int(CcyE::N)] = {0}; // memset(m_IRs, 0.0, CcyE::N)
		if (a_file == nullptr || *a_file == '\0')
			return;
		FILE* source = fopen(a_file, "r");
		while (fgets(buff, 255, source) != nullptr) {
			buff[3] = '\0';
			int code = int(str2CcyE(buff));
			double r = std::atof(buff + 4);
		}
		fclose(source);
	}
}
