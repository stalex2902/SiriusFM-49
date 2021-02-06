#pragma once

#include "IRProvider.h"

namespace SiriusFM {
	template<>
	class IRProvider<IRModeE::Const> {
		private:
			double m_IRs[CcyE::N];
		public:
			IRProvider(char const* a_file);
			double r(CcyE a_ccy, double t) const {
				return m_IRs[int(a_ccy)];
			}
	};
}
