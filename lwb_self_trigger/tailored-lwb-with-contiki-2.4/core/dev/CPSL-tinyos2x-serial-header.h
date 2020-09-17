// #include "net/mac/tsch/tsch.h"

#define TINYOS_2X_FRAME_BYTE 0x7e
#define TINYOS_2X_ESCAPE_CHAR 0x7d
#define TINYOS_2X_SERIAL_BUFFER_SIZE 256

#define TINYOS_2X_SCHEDULE_DISTRIBUTION 0xf0

#define TINYOS_2X_SERIAL_HEADER_LENGTH 4

#define TINYOS_HEADER_LENGTH_ACK 11
#define TINYOS_HEADER_LENGTH_NOACK 10

#define TINYOS_PACKET_ACK 0x44
#define TINYOS_PACKET_NOACK 0x45
#define TINYOS_ACK 0x43

#define PROTOCOL_POS 1
#define SEQNO_POS 2

#ifndef SERIAL_SCHEDULE_DISTRIBUTION
#define SERIAL_SCHEDULE_DISTRIBUTION
#endif

#ifndef TINYOS_2X_SERIAL_PACKET
#define TINYOS_2X_SERIAL_PACKET
#endif

// process_event_t serial_schedule_distribution_event_message;
// process_event_t serial_schedule_distribution_finished_event_message;
unsigned char tinyos2x_serial_unescaped_buffer[TINYOS_2X_SERIAL_BUFFER_SIZE];
unsigned char tinyos2x_serial_escaped_buffer[TINYOS_2X_SERIAL_BUFFER_SIZE];
unsigned char tinyos2x_unescaped_send_buffer[TINYOS_2X_SERIAL_BUFFER_SIZE];
unsigned char tinyos2x_escaped_send_buffer[TINYOS_2X_SERIAL_BUFFER_SIZE];
/*
const linkaddr_t node110_address = { { 0, 110 } };
const linkaddr_t node112_address = { { 0, 112 } };
const linkaddr_t node117_address = { { 0, 117 } };
const linkaddr_t node118_address = { { 0, 118 } };
*/
/*
const linkaddr_t node110_address;
const linkaddr_t node112_address;
const linkaddr_t node117_address;
const linkaddr_t node118_address;
*/
//unsigned char tinyos_2x_serial_buffer[TINYOS_2X_SERIAL_BUFFER_SIZE];

