//==========================================================================//
//                              "IRProviderConst.h"                         //
// We read constant IRs from a file with data provided                      //
//==========================================================================//

#include "IRProviderConst.h"

#include <cstdio>
#include <cstdlib>

namespace SiriusFM {

	IRProvider<IRModeE::Const>::IRProvider(const char* a_file) {
	constexpr int BUF_SIZE = 64;
	constexpr int CCY_SIZE = 3;

		// zero-out all the rates
		for (int k = 0; k < int(CcyE::N); ++k)
			m_IRs[k] = 0;

		if (a_file == nullptr || *a_file == '\0') // check if a_file empty
			return;

		FILE* src = fopen(a_file, "r");

		if (src == nullptr)
			throw std::runtime_error("Cannot open file");

		char buf[BUF_SIZE];
		char ccy[CCY_SIZE + 1] = "XXX";
		
		while (fgets(buf, BUF_SIZE, src) != nullptr) {
			if (*buf == '\0' || *buf == '\n' || *buf == '#')
				continue;

			strncpy(ccy, buf, CCY_SIZE);
			m_IRs[int(Str2CcyE(ccy))] = atof(buf + CCY_SIZE + 1);
		}
		fclose(src);
	}
}
