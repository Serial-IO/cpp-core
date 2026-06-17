#include "cpp_core/reflection.hpp"
#include "cpp_core/result.hpp"
#include "cpp_core/serial_config.hpp"
#include "cpp_core/strong_types.hpp"

namespace cpp_core::tests::reflection
{

struct PrivateMemberAggregate
{
  private:
    int value;
};

static_assert(cpp_core::reflection::enumeratorCount<cpp_core::Parity>() == 3);
static_assert(cpp_core::reflection::enumeratorCount<cpp_core::StopBits>() == 2);
static_assert(cpp_core::reflection::enumeratorCount<cpp_core::FlowControl>() == 3);
static_assert(cpp_core::reflection::enumeratorName<cpp_core::Parity, 0>() == "kNone");
static_assert(cpp_core::reflection::enumeratorName<cpp_core::Parity, 1>() == "kEven");
static_assert(cpp_core::reflection::enumerator_name_v<cpp_core::FlowControl, 2> == "kXonXoff");
static_assert(cpp_core::reflection::hasPubliclyReflectableFields<cpp_core::SerialConfig>());
static_assert(cpp_core::reflection::publicFieldCount<cpp_core::SerialConfig>() == 4);
static_assert(cpp_core::reflection::publicFieldName<cpp_core::SerialConfig, 2>() == "parity");
static_assert(cpp_core::reflection::public_field_name_v<cpp_core::Error, 1> == "message");
static_assert(cpp_core::reflection::public_field_count_v<cpp_core::Error> == 2);
static_assert(!cpp_core::reflection::hasPubliclyReflectableFields<PrivateMemberAggregate>());

} // namespace cpp_core::tests::reflection
