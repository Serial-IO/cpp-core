#include "cpp_core/strong_types.hpp"

namespace cpp_core::tests::strong_types
{

static_assert(Baudrate{}.get() == 0);
static_assert((Baudrate{9'600} + Baudrate{115'200}).get() == 124'800);
static_assert((DataBits{8} - DataBits{3}).get() == 5);
static_assert(toInt(Parity::kOdd) == 2);
static_assert(toInt(StopBits::kTwo) == 2);
static_assert(toInt(FlowControl::kXonXoff) == 2);

} // namespace cpp_core::tests::strong_types
