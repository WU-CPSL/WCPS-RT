#include "energy-stats.h"

#include <stdio.h>


/*---------------------------------------------------------------------------*/

static int8_t energy_accounting_initiated = 0;

static uint32_t last_cpu, last_lpm, last_transmit, last_listen;

/*---------------------------------------------------------------------------*/

void energy_update(uint16_t period_ms)
{
	uint32_t curr_cpu, curr_lpm, curr_transmit, curr_listen;
	
	if(!energy_accounting_initiated) {
		printf("Please specify platform related parameters in the header file and call initiate_energy_accounting function\n");
		return;
	}

	energest_flush();

	curr_cpu = energest_type_time(ENERGEST_TYPE_CPU);
	curr_lpm = energest_type_time(ENERGEST_TYPE_LPM);
	curr_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	curr_listen = energest_type_time(ENERGEST_TYPE_LISTEN);

	long tx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_transmit - last_transmit));
	long rx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_listen - last_listen));
	long duty_cycle = 100 * (tx_time_us+rx_time_us) / ((long) period_ms);    // still need to be divided by 1000, because period is in ms.
	
	/* Energy (uJ) = v (V) * current (mA) * time (us) / 1000 */
	/* divide by 10 means scale down current value (eliminating the `dot') */
	/*uint8_t tx_level = cc2420_get_txpower();
	tx_energy = voltage * tx_current_consumption(tx_level) * tx_time_us / 1000 / 10;
	rx_energy = voltage * rx_current_consumption * rx_time_us / 1000 / 10;

	remaining_energy = MAX(remaining_energy - tx_energy, 0);
	remaining_energy = MAX(remaining_energy - rx_energy, 0);*/			
			
	printf("Radio on time %ld ms in %u ms, duty cycle %ld.%01ld %%\n",
				(tx_time_us+rx_time_us)/1000, period_ms, duty_cycle / 1000, duty_cycle % 1000);
	
	last_cpu = curr_cpu;
	last_lpm = curr_lpm;
	last_transmit = curr_transmit;
	last_listen = curr_listen;
}


long energy_tx_time(uint16_t period_ms)
{
	uint32_t curr_cpu, curr_lpm, curr_transmit, curr_listen;
	
	if(!energy_accounting_initiated) {
		printf("Please specify platform related parameters in the header file and call initiate_energy_accounting function\n");
		return;
	}

	energest_flush();

	curr_cpu = energest_type_time(ENERGEST_TYPE_CPU);
	curr_lpm = energest_type_time(ENERGEST_TYPE_LPM);
	curr_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	curr_listen = energest_type_time(ENERGEST_TYPE_LISTEN);

	long tx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_transmit - last_transmit));
	long rx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_listen - last_listen));
	long duty_cycle = 100 * (tx_time_us+rx_time_us) / ((long) period_ms);    // still need to be divided by 1000, because period is in ms.		
	return tx_time_us;
}



long energy_rx_time(uint16_t period_ms)
{
	uint32_t curr_cpu, curr_lpm, curr_transmit, curr_listen;
	
	if(!energy_accounting_initiated) {
		printf("Please specify platform related parameters in the header file and call initiate_energy_accounting function\n");
		return;
	}

	energest_flush();

	curr_cpu = energest_type_time(ENERGEST_TYPE_CPU);
	curr_lpm = energest_type_time(ENERGEST_TYPE_LPM);
	curr_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	curr_listen = energest_type_time(ENERGEST_TYPE_LISTEN);

	long tx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_transmit - last_transmit));
	long rx_time_us = ((long) 1e6 / RTIMER_SECOND) * ((long) (curr_listen - last_listen));
	long duty_cycle = 100 * (tx_time_us+rx_time_us) / ((long) period_ms);    // still need to be divided by 1000, because period is in ms.
	last_cpu = curr_cpu;
	last_lpm = curr_lpm;
	last_transmit = curr_transmit;
	last_listen = curr_listen;
	return rx_time_us;
}




void initiate_energy_accounting(void) {
	
	energy_accounting_initiated = 1;
	
	energest_flush();

	last_cpu = energest_type_time(ENERGEST_TYPE_CPU);
	last_lpm = energest_type_time(ENERGEST_TYPE_LPM);
	last_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	last_listen = energest_type_time(ENERGEST_TYPE_LISTEN);
}



void debug()
{
    printf("Yehan is debuging.");
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// #include "CPSL-tinyos-2x-serial.h"

// PROCESS(tinyos2x_serial_process, "Tiny OS 2.x Serial Process");
// PROCESS(tinyos2x_serial_ack_send_process, "Tiny OS 2.x Serial ACK Send Process");
// PROCESS(tinyos2x_serial_unescape_process, "Tiny OS 2.x Serial unescape");
static unsigned char packet_start = 0;
static unsigned char escape_start = 0;
static unsigned int serial_char_count = 0;


// void tinyos2x_serial_dispatcher(unsigned char c){
//     if(packet_start == 0){
//         if(c == TINYOS_2X_FRAME_BYTE){
//             packet_start = 1;
//             tinyos2x_serial_escaped_buffer[tinyos2x_serial_escaped_counter] = c;
//             tinyos2x_serial_escaped_counter += 1;
//         }
//     }else{
//         tinyos2x_serial_escaped_buffer[tinyos2x_serial_escaped_counter] = c;
//         tinyos2x_serial_escaped_counter += 1;
//         if(c == TINYOS_2X_FRAME_BYTE){
//             packet_start = 0;
//             tinyos2x_serial_escaped_data_len =
//                 tinyos2x_serial_escaped_counter;
//             tinyos2x_serial_escaped_counter = 0;
// 			//process_post(PROCESS_BROADCAST,
// 			//		 tinyos2x_serial_dispatch_event_message, NULL);
//             process_poll(&tinyos2x_serial_unescape_process);
//             //printf("a packet is received!\n");
//         }
//     }
// }

// void tinyos2x_serial_init(){
// 	unsigned int i = 0;
//     /*
// 	for(i = 0; i < TINYOS_2X_SERIAL_BUFFER_SIZE; i++){
// 		tinyos_2x_serial_buffer[i] = 0;
// 	}
// */
// 	escape_start = 0;
// 	packet_start = 0;

//     tinyos2x_serial_current_seqno = 0;
// 	tinyos_2x_dispatch_finished = 0;
//     tinyos2x_serial_escaped_counter = 0;
//     tinyos2x_serial_clean_unescaped_buffer();
//     tinyos2x_serial_clean_escaped_buffer();
//     tinyos2x_ack_buffers_init();
// 	process_start(&tinyos2x_serial_process, NULL);
//     process_start(&tinyos2x_serial_unescape_process, NULL);
//     process_start(&tinyos2x_serial_ack_send_process, NULL);
// #ifdef SERIAL_SCHEDULE_DISTRIBUTION
// 	serial_schedule_distribution_init();
// #endif
// }

void tinyos2x_serial_unescape_buffer(){
    int i = 0;
    uint16_t unescape_count = 0;
    while(i < tinyos2x_serial_escaped_data_len){
        if(tinyos2x_serial_escaped_buffer[i] == TINYOS_2X_ESCAPE_CHAR){
            tinyos2x_serial_unescaped_buffer[unescape_count] = \
                tinyos2x_serial_escaped_buffer[i + 1] | 0x20;
            i += 2;
            unescape_count += 1;
        }else{
            tinyos2x_serial_unescaped_buffer[unescape_count] = \
                tinyos2x_serial_escaped_buffer[i];
            i += 1;
            unescape_count += 1;
        }
    }
    

    tinyos2x_serial_unescaped_data_len = unescape_count;

}

void tinyos2x_serial_clean_escaped_buffer(){
    int i = 0;
    tinyos2x_serial_escaped_counter = 0;
    tinyos2x_serial_escaped_data_len = 0;
    for(i = 0; i < TINYOS_2X_SERIAL_BUFFER_SIZE; i++){
        tinyos2x_serial_escaped_buffer[i] = 0;
    }
}

void tinyos2x_serial_clean_unescaped_buffer(){
    int i = 0;
    tinyos2x_serial_unescaped_counter = 0;
    tinyos2x_serial_unescaped_data_len = 0;
    for(i = 0; i < TINYOS_2X_SERIAL_BUFFER_SIZE; i++){
        tinyos2x_serial_unescaped_buffer[i] = 0;
    }
}

void tinyos2x_serial_get_seqno(){
    tinyos2x_serial_current_seqno = tinyos2x_serial_unescaped_buffer[SEQNO_POS];
}


void tinyos2x_clean_send_unescaped_buffer(){
    int i = 0;
    for(i = 0; i < TINYOS_2X_SERIAL_BUFFER_SIZE; i++){
        tinyos2x_unescaped_send_buffer[i] = 0;
    }
    tinyos2x_send_unescaped_data_len = 0;

}

void tinyos2x_clean_send_escaped_buffer(){
    int i = 0;
    for(i = 0; i < TINYOS_2X_SERIAL_BUFFER_SIZE; i++){
        tinyos2x_escaped_send_buffer[i] = 0;
    }
    tinyos2x_send_escaped_data_len = 0;
}

void tinyos2x_build_ack_packet(){

    tinyos2x_ack_buffer_unescaped[0] = TINYOS_ACK;
    tinyos2x_ack_buffer_unescaped[1] = tinyos2x_serial_current_seqno;
    uint16_t crc = crc16_packet(tinyos2x_ack_buffer_unescaped, 2);
    tinyos2x_ack_buffer_unescaped[2] = (uint8_t)crc;
    crc >>= 8;
    tinyos2x_ack_buffer_unescaped[3] = (uint8_t)crc;
    tinyos2x_ack_unescaped_buffer_length = 4;

    //start to escaped the buffer
    tinyos2x_ack_buffer_escaped[0] = TINYOS_2X_FRAME_BYTE;
    tinyos2x_ack_escaped_buffer_length = 1;
    unsigned char temp = 0;
    int i = 0;
    for(i = 0; i < ACK_MAX_LENGTH_UNESCAPED; i++){
        if(tinyos2x_ack_buffer_unescaped[i] == TINYOS_2X_ESCAPE_CHAR){
            tinyos2x_ack_buffer_escaped[tinyos2x_ack_escaped_buffer_length] = \
                TINYOS_2X_ESCAPE_CHAR;
            temp = tinyos2x_ack_buffer_unescaped[i];
            temp &= 0x0f;
            temp |= tinyos2x_ack_buffer_unescaped[i];
            tinyos2x_ack_escaped_buffer_length += 1;
            tinyos2x_ack_buffer_escaped[tinyos2x_ack_escaped_buffer_length] = \
                temp;
            tinyos2x_ack_escaped_buffer_length += 1;
        }else{
            tinyos2x_ack_buffer_escaped[tinyos2x_ack_escaped_buffer_length] = \
                    tinyos2x_ack_buffer_unescaped[i];
            tinyos2x_ack_escaped_buffer_length += 1;
        }
    }
    tinyos2x_ack_buffer_escaped[tinyos2x_ack_escaped_buffer_length] = \
            TINYOS_2X_FRAME_BYTE;
    tinyos2x_ack_escaped_buffer_length += 1;
}

void tinyos2x_clean_ack_buffers(){
    int i = 0;
    for(i = 0; i < ACK_MAX_LENGTH_UNESCAPED; i++){
        tinyos2x_ack_buffer_unescaped[i] = 0;
    }

    for(i = 0; i < ACK_MAX_LENGTH_ESCAPED; i++){
        tinyos2x_ack_buffer_escaped[i] = 0;
    }
    tinyos2x_ack_escaped_buffer_length = 0;
    tinyos2x_ack_unescaped_buffer_length = 0;
}

void tinyos2x_ack_buffers_init(){
    int i = 0;
    for(i = 0; i < ACK_MAX_LENGTH_UNESCAPED; i++){
        tinyos2x_ack_buffer_unescaped[i] = 0;
    }

    for(i = 0; i < ACK_MAX_LENGTH_ESCAPED; i++){
        tinyos2x_ack_buffer_escaped[i] = 0;
    }
    tinyos2x_ack_escaped_buffer_length = 0;
    tinyos2x_ack_unescaped_buffer_length = 0;
}

void escape_send_buffer(){
    int i = 0;
    uint16_t escaped_buffer_length = 0;
    unsigned char temp = 0;
    tinyos2x_escaped_send_buffer[0] = TINYOS_2X_FRAME_BYTE;
    escaped_buffer_length += 1;
    for(i = 0; i < tinyos2x_send_unescaped_data_len; i++){
        if(tinyos2x_unescaped_send_buffer[i] == TINYOS_2X_FRAME_BYTE || \
                tinyos2x_unescaped_send_buffer[i] == TINYOS_2X_ESCAPE_CHAR){
            tinyos2x_escaped_send_buffer[escaped_buffer_length] = \
                                TINYOS_2X_ESCAPE_CHAR;
            temp = tinyos2x_unescaped_send_buffer[i];
            temp &= 0x0f;
            temp |= tinyos2x_unescaped_send_buffer[i];
            tinyos2x_escaped_send_buffer[escaped_buffer_length + 1] = \
                                temp;
            escaped_buffer_length += 2;
        }else{
            tinyos2x_escaped_send_buffer[escaped_buffer_length] = \
                    tinyos2x_unescaped_send_buffer[i];
            escaped_buffer_length += 1;
        }
    }
    tinyos2x_escaped_send_buffer[escaped_buffer_length] = \
                            TINYOS_2X_FRAME_BYTE;
    escaped_buffer_length += 1;

    tinyos2x_send_escaped_data_len = escaped_buffer_length;
}

uint16_t crc16_packet(unsigned char *data, uint16_t len){
    uint16_t crc = 0;
    while(len-- > 0){
        crc = crc_byte(crc, *data++);
    }

    return crc;
}

uint16_t crc_byte(uint16_t crc, uint8_t b){
    uint8_t i;
    crc = crc ^ b << 8;
    i = 8;
    do{
        if(crc & 0x8000){
            crc = crc << 1 ^ 0x1021;
        }else{
            crc = crc << 1;
        }
    }while(--i);

    return crc;
}

void uart1_send_array(unsigned char *data, uint16_t len){
    int i = 0;
    for(i = 0; i < len; i++){
        uart1_writeb(*(data + i));
        //printf("the character sent is %d \n", *(data+i));
    }
}


void send_data_to_server(unsigned char *data, uint16_t len, \
        uint8_t protocol_type){
    if(protocol_type == TINYOS_PACKET_ACK){
        //printf("send non-ack packet\n");
        tinyos2x_unescaped_send_buffer[0] = TINYOS_PACKET_ACK;
        //sequence number
        tinyos2x_unescaped_send_buffer[1] = 1;
        //packet type
        tinyos2x_unescaped_send_buffer[2] = 0;
        //destination high byte
        tinyos2x_unescaped_send_buffer[3] = 0;
        //destination low byte
        tinyos2x_unescaped_send_buffer[4] = 0;
        //source high byte
        tinyos2x_unescaped_send_buffer[5] = 0;
        //source low byte
        tinyos2x_unescaped_send_buffer[6] = 102;
        //payload length
        tinyos2x_unescaped_send_buffer[7] = (uint8_t)len;
        //destination PAN group
        tinyos2x_unescaped_send_buffer[8] = 0;
        //Packet AM type
        tinyos2x_unescaped_send_buffer[9] = 0;
        int i = 0;
        for(i = 0; i < len; i++){
            tinyos2x_unescaped_send_buffer[10 + i] = *(data+i);
            //printf("the %dth data is %d\n", i, *(data+i));
        }
        uint16_t packet_crc = 0;
        packet_crc = crc16_packet(tinyos2x_unescaped_send_buffer, (10 + len));
        //crc low byte
        tinyos2x_unescaped_send_buffer[10 + len] = (uint8_t)packet_crc;
        //crc high byte
        tinyos2x_unescaped_send_buffer[10 + len + 1] = \
                    (uint8_t)(packet_crc >> 8);
        //the total length right now is (9 + len + 2)
        tinyos2x_send_unescaped_data_len = 10 + len + 2;
        escape_send_buffer();
        uart1_send_array(tinyos2x_escaped_send_buffer, \
            tinyos2x_send_escaped_data_len);
        tinyos2x_clean_send_unescaped_buffer();
        tinyos2x_clean_send_escaped_buffer();

    }
}


// PROCESS_THREAD(tinyos2x_serial_process, ev, data){
// 	PROCESS_BEGIN();
// 	tinyos2x_serial_dispatch_event_message =
// 		process_alloc_event();

// 	while(1){

// 		//PROCESS_YIELD_UNTIL(tinyos_2x_dispatch_finished);
// 		PROCESS_YIELD();
// 		if(ev != tinyos2x_serial_dispatch_event_message){
// 			continue;
// 		}

// 		//printf("one packet received\n");
// 		tinyos_2x_dispatch_finished = 0;
// 		//process_poll(&serial_schedule_distribution_process);
// 		process_post(PROCESS_BROADCAST,
// 					 serial_schedule_distribution_event_message, NULL);

// 	}


// 	PROCESS_END();
// }

// PROCESS_THREAD(tinyos2x_serial_ack_send_process, ev, data){
// 	PROCESS_BEGIN();

// 	while(1){
// 		//PROCESS_YIELD_UNTIL(tinyos_2x_dispatch_finished);
// 		//PROCESS_YIELD();
//         PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
//         //get the sequence number
//         tinyos2x_serial_get_seqno();
// 		//if(ev != tinyos2x_serial_dispatch_event_message){
// 		//	continue;
// 		//}
//         //printf("send ack!!\n");
//         tinyos2x_build_ack_packet();
//         uart1_send_array(tinyos2x_ack_buffer_escaped, \
//                          tinyos2x_ack_escaped_buffer_length);
//         tinyos2x_clean_ack_buffers();
//         process_poll(&serial_schedule_distribution_process);

// 		//this means that the packet dispatching is already over, start to send
// 		//ACK back to the host
// 	}
// 	PROCESS_END();
// }


// PROCESS_THREAD(tinyos2x_serial_unescape_process, ev, data){
// 	PROCESS_BEGIN();

// 	while(1){

// 		//PROCESS_YIELD_UNTIL(tinyos_2x_dispatch_finished);
// 		//PROCESS_YIELD();
//         PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
//         tinyos2x_serial_unescape_buffer();
//         tinyos2x_serial_clean_escaped_buffer();
//         //printf("The length of the packet is %d \n",
//         //        tinyos2x_serial_unescaped_data_len);
//         process_poll(&tinyos2x_serial_ack_send_process);
// 		//this means that the packet dispatching is already over, start to send
// 		//ACK back to the host
// 	}
// 	PROCESS_END();
// }

// void debug()
// {
//     printf("Yehan is debuging.");
// }