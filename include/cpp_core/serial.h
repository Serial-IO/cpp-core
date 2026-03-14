#pragma once

#include "module_api.h"
#include "version.hpp"

// Aggregated interface headers
#include "interface/get_version.h"
#include "interface/serial_abort_read.h"
#include "interface/serial_abort_write.h"
#include "interface/serial_clear_buffer_in.h"
#include "interface/serial_clear_buffer_out.h"
#include "interface/serial_close.h"
#include "interface/serial_drain.h"
#include "interface/serial_in_bytes_total.h"
#include "interface/serial_in_bytes_waiting.h"
#include "interface/serial_list_ports.h"
#include "interface/serial_open.h"
#include "interface/serial_out_bytes_total.h"
#include "interface/serial_out_bytes_waiting.h"
#include "interface/serial_read.h"
#include "interface/serial_read_line.h"
#include "interface/serial_read_until.h"
#include "interface/serial_read_until_sequence.h"
#include "interface/serial_set_error_callback.h"
#include "interface/serial_set_read_callback.h"
#include "interface/serial_set_write_callback.h"
#include "interface/serial_write.h"

// Modem line control
#include "interface/serial_set_dtr.h"
#include "interface/serial_set_rts.h"
#include "interface/serial_get_cts.h"
#include "interface/serial_get_dsr.h"
#include "interface/serial_get_dcd.h"
#include "interface/serial_get_ri.h"

// Extended configuration
#include "interface/serial_set_flow_control.h"
#include "interface/serial_send_break.h"
#include "interface/serial_update_baudrate.h"
#include "interface/serial_get_config.h"

// Port discovery
#include "interface/serial_monitor_ports.h"
