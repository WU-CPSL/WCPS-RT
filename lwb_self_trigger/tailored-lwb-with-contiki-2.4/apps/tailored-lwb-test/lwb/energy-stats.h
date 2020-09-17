#ifndef ENERGY_STATS_H_
#define ENERGY_STATS_H_

#include "contiki.h"


/*---------------------------------------------------------------------------*/

void energy_update(uint16_t period_ms);
long energy_tx_time(uint16_t period_ms);
long energy_rx_time(uint16_t period_ms);

void initiate_energy_accounting(void);

/*---------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------*/


#define ACK_MAX_LENGTH_UNESCAPED 4
#define ACK_MAX_LENGTH_ESCAPED 10

// process_event_t tinyos2x_serial_dispatch_event_message;
// PROCESS_NAME(tinyos2x_serial_process);
// PROCESS_NAME(tinyos2x_serial_ack_send_process);
// PROCESS_NAME(tinyos2x_serial_unescape_process);
// PROCESS_NAME(serial_schedule_distribution_process);

unsigned char tinyos_2x_dispatch_finished;

unsigned char tinyos2x_ack_buffer_unescaped[ACK_MAX_LENGTH_UNESCAPED];
unsigned char tinyos2x_ack_buffer_escaped[ACK_MAX_LENGTH_ESCAPED];
unsigned char tinyos2x_ack_escaped_buffer_length;
unsigned char tinyos2x_ack_unescaped_buffer_length;
void tinyos2x_clean_ack_buffers();
void tinyos2x_ack_buffers_init();


void tinyos2x_serial_dispatcher(unsigned char c);
void tinyos2x_serial_init();
int get_packet_type();

void tinyos2x_serial_clean_unescaped_buffer();
void tinyos2x_serial_clean_escaped_buffer();
void tinyos2x_clean_send_unescaped_buffer();
void tinyos2x_clean_send_escaped_buffer();


void tinyos2x_build_ack_packet();
void escape_send_buffer();
uint16_t crc16_packet(unsigned char *data, uint16_t len);
uint16_t crc_byte(uint16_t crc, uint8_t b);
void tinyos2x_serial_get_seqno();

unsigned int tinyos2x_serial_escaped_counter;
unsigned int tinyos2x_serial_unescaped_counter;
unsigned int tinyos2x_serial_escaped_data_len;
unsigned int tinyos2x_serial_unescaped_data_len;

unsigned int tinyos2x_send_escaped_data_len;
unsigned int tinyos2x_send_unescaped_data_len;

unsigned char tinyos2x_serial_current_seqno;


void uart1_send_array(unsigned char *data, uint16_t len);

void send_data_to_server(unsigned char *data, uint16_t len, \
        uint8_t protocol_type);

void debug(void);

#endif /* ENERGY_STATS_H_ */
