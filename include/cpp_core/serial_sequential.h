#pragma once

#include "module_api.h"
#include "version.h"

// Missing from the sequential interface
#include "interface/get_version.h"
#include "interface/serial_get_ports_info.h"
#include "interface/serial_open.h"
#include "interface/serial_set_error_callback.h"
#include "interface/serial_set_read_callback.h"
#include "interface/serial_set_write_callback.h"

// Sequential wrappers
#include "interface/sequential.h"
