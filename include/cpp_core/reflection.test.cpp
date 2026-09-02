#include "cpp_core/reflection.hpp"
#include "cpp_core/interface/meta.h"
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
static_assert(cpp_core::reflection::publicFieldCount<cpp_core::SerialConfig>() == 5);
static_assert(cpp_core::reflection::publicFieldName<cpp_core::SerialConfig, 2>() == "parity");
static_assert(cpp_core::reflection::publicFieldName<cpp_core::SerialConfig, 4>() == "flow_mode");
static_assert(cpp_core::reflection::publicFieldCount<cpp_core::SerialTimeoutConfig>() == 2);
static_assert(cpp_core::reflection::hasPubliclyReflectableFields<cpp_core::Meta>());
static_assert(cpp_core::reflection::publicFieldCount<cpp_core::Meta>() == 19);
static_assert(cpp_core::reflection::publicFieldName<cpp_core::Meta, 0>() == "major");
static_assert(cpp_core::reflection::publicFieldName<cpp_core::Meta, 5>() == "library_name");
static_assert(cpp_core::reflection::publicFieldName<cpp_core::Meta, 18>() == "git_dirty_suffix");
static_assert(cpp_core::reflection::public_field_name_v<cpp_core::Error, 1> == "message");
static_assert(cpp_core::reflection::public_field_count_v<cpp_core::Error> == 2);
static_assert(!cpp_core::reflection::hasPubliclyReflectableFields<PrivateMemberAggregate>());

} // namespace cpp_core::tests::reflection
