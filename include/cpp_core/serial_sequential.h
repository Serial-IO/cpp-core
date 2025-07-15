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

// Aggregates all *Sequential headers
#include "interface/sequential/serial_abort_read_sequential.h"
#include "interface/sequential/serial_abort_write_sequential.h"
#include "interface/sequential/serial_clear_buffer_in_sequential.h"
#include "interface/sequential/serial_clear_buffer_out_sequential.h"
#include "interface/sequential/serial_close_sequential.h"
#include "interface/sequential/serial_drain_sequential.h"
#include "interface/sequential/serial_in_bytes_total_sequential.h"
#include "interface/sequential/serial_in_bytes_waiting_sequential.h"
#include "interface/sequential/serial_out_bytes_total_sequential.h"
#include "interface/sequential/serial_out_bytes_waiting_sequential.h"
#include "interface/sequential/serial_read_line_sequential.h"
#include "interface/sequential/serial_read_sequential.h"
#include "interface/sequential/serial_read_until_sequence_sequential.h"
#include "interface/sequential/serial_read_until_sequential.h"
#include "interface/sequential/serial_write_sequential.h"
