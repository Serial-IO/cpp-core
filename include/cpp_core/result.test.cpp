#include "cpp_core/result.hpp"

namespace cpp_core::tests::result
{

constexpr auto leafValue() -> Result<int>
{
    return ok(41);
}

constexpr auto leafFailure() -> Result<int>
{
    return fail<int>(StatusCode::Connection::kInvalidHandleError);
}

constexpr auto addOne() -> Result<int>
{
    return leafValue().transform([](int value) { return value + 1; });
}

constexpr auto propagateLeafFailure() -> Result<int>
{
    auto value = leafFailure();
    if (!value)
    {
        return forwardUnexpected(std::move(value));
    }
    return ok(std::move(value).value() + 1);
}

constexpr auto okStatus() -> Status
{
    return ok();
}

constexpr auto failedStatus() -> Status
{
    return fail(StatusCode::Io::kReadError);
}

constexpr auto propagateStatusFailure() -> Status
{
    return failedStatus().and_then([]() -> Status { return ok(); });
}

static_assert(addOne().has_value());
static_assert(addOne().value() == 42);
static_assert(okStatus().has_value());
static_assert(!propagateLeafFailure().has_value());
static_assert(propagateLeafFailure().error() == StatusCode::Connection::kInvalidHandleError);
static_assert(!propagateStatusFailure().has_value());
static_assert(propagateStatusFailure().error() == StatusCode::Io::kReadError);

} // namespace cpp_core::tests::result
