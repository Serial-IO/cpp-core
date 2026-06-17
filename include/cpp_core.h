#pragma once

/*
 * Umbrella header for the complete public cpp_core surface.
 *
 * Prefer narrower includes in translation units with strict compile-time
 * requirements, but this header is the convenient entry point when a consumer
 * wants the full API and helper layer in one include.
 */

#include "cpp_core/error_callback.h"
#include "cpp_core/error_handling.hpp"
#include "cpp_core/result.hpp"
#include "cpp_core/reflection.hpp"
#include "cpp_core/scope_guard.hpp"
#include "cpp_core/serial.h"
#include "cpp_core/serial_config.hpp"
#include "cpp_core/status_code.h"
#include "cpp_core/strong_types.hpp"
#include "cpp_core/unique_resource.hpp"
#include "cpp_core/validation.hpp"
#include "cpp_core/version.hpp"
