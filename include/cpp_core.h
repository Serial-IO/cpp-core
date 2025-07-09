#pragma once

/*
 * cpp_core.h — Umbrella header for the cpp_core library
 *
 * Include this single file to gain access to the complete public C/C++ API
 * of the cpp_core runtime.  Internally it forwards to the individual header
 * units located in the cpp_core/ sub-directory so fine-grained includes continue
 * to work as before.  Prefer including the specific headers you actually need
 * in translation units with strict compile-time requirements; however for
 * quick prototyping and high-level application code, this umbrella header
 * offers the most convenient entry point.
 */

#include "cpp_core/serial.h"
#include "cpp_core/status_codes.h"
#include "cpp_core/version.h"
