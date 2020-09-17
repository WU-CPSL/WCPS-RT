#include "slot-allocation.h"
#include "slot-participation.h"
#include "energy-stats.h"
// #include "CPSL-tinyos2x-serial-header.h"
// #include "CPSL-tinyos-2x-serial.h"

/**
 * \defgroup custom-lwb-variables Application variables
 * @{
 */
static sync_data_struct sync_data;     /**< \brief Flooding data. */
static struct rtimer rt;                   /**< \brief Rtimer used to schedule Glossy. */
static struct pt pt;                       /**< \brief Protothread used to schedule Glossy. */
static rtimer_clock_t t_ref_l_old = 0;     /**< \brief Reference time computed from the Glossy
                                                phase before the last one. \sa get_t_ref_l */
static uint8_t skew_estimated = 0;         /**< \brief Not zero if the clock skew over a period of length
                                                \link GLOSSY_PERIOD \endlink has already been estimated. */
static uint8_t sync_missed = 0;            /**< \brief Current number of consecutive phases without
                                                synchronization (reference time not computed). */
static int period_skew = 0;                /**< \brief Current estimation of clock skew over a period
                                                of length \link GLOSSY_PERIOD \endlink. */

/** @} */

/**
 * \defgroup lwb-test-variables-stats Statistics variables
 * @{
 */

/*---------------------------------------------------------------------------*/
static sensed_data_struct  sensed_data, sensed_data1, sensed_data2;
static request_data_struct req_reply;
/*---------------------------------------------------------------------------*/
static rtimer_clock_t REF_TIME;
static rtimer_clock_t NEXT_SLOT;
/*---------------------------------------------------------------------------*/
static int16_t period = 0;
static int     offset_err;
/*---------------------------------------------------------------------------*/
static uint16_t run_time, last_time;

static uint16_t heartbeat_period=20;

static uint8_t  RR_SLOTS, rr_slots;

static uint16_t DATA_SLOTS, data_slots, allocated_slot1, allocated_slot2;
static uint16_t max_data_slots;

static uint8_t  SLEEP_SLOTS;
/*---------------------------------------------------------------------------*/
static flow_info_struct flow_info;
static int trigger[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

static uint16_t last_update[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint16_t flow1src = 110;
static uint16_t flow1dst = 164;
static uint16_t flow2src = 109;
static uint16_t flow2dst = 131;
static uint16_t flow3src = 131;
static uint16_t flow3dst = 164;
////
static uint16_t superframe_len = 3;


static schedule_struct schedule[20]={
		{110,164,1,1},{109,131,2,1},{131,164,3,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
		{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}
	};

// static schedule_struct schedule[20]={
// 		{110,164,1,1},{109,131,2,1},{131,164,3,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},
// 		{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1},{0,0,0,1}
// 	};
/*---------------------------------------------------------------------------*/
static int8_t FLOODING_ROLE, TX_COUNT;
static uint16_t rx_count;
static int8_t hop_count;
static enum err_type errno;
static uint8_t active_slots;
/*---------------------------------------------------------------------------*/

char tailored_lwb_scheduler(struct rtimer *t, void *ptr);
/*---------------------------------------------------------------------------*/

/** @} */
/** @} */

/**
 * \defgroup lwb-test-processes Application processes and functions
 * @{
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define SERIAL_BUF_SIZE 8
#define SERIAL_BUF_SIZE2 8
 // static char rx_buf[SERIAL_BUF_SIZE];
static int act_int[SERIAL_BUF_SIZE];
static int mode_int[SERIAL_BUF_SIZE2];
static char rx_buf[SERIAL_BUF_SIZE+SERIAL_BUF_SIZE2];
static long int actuation_data_int=0;
// static long int mode_data_int=1;
static long int mode_data_int=1;

int i = 0;
 // int m;
 // for (m=0;m<SERIAL_BUF_SIZE - 1;m=m+1)
 // { act_int[m]=0;}
 float power_of_ten (int exp)
 {
    float result=1;
    if (exp>=0)
    {
    	while(exp){result*=10;exp--;}
	}
	if (exp<0)
    {
    	int m=-exp;
    	// printf("%d\n",m);
    	while(m){result*=0.1;m--;}
	}
    return result;
 }
   static int uart_rx_callback(char c) 
   {
		 rx_buf[i] = c;
         i = i + 1;
         if(i >= SERIAL_BUF_SIZE+SERIAL_BUF_SIZE2)
         {	
            i=0;
            int k;
            int dot=SERIAL_BUF_SIZE;
            int dot2=SERIAL_BUF_SIZE2;
            float actuation_data=0;
            float mode=0;
            for (k=0;k<SERIAL_BUF_SIZE;k=k+1)
            {
               act_int[k]=rx_buf[k]-'0';
               // printf("%d",rx_buf[k]);
               if (act_int[k]==-2)
               {
                  dot=k;
               }
            }
            for (k=SERIAL_BUF_SIZE;k<SERIAL_BUF_SIZE+SERIAL_BUF_SIZE2;k=k+1)
            {
                mode_int[k-SERIAL_BUF_SIZE]=rx_buf[k]-'0';
                // printf("%d",rx_buf[k]);
                if (mode_int[k-SERIAL_BUF_SIZE]==-2)
               {
                  dot2=k-SERIAL_BUF_SIZE;
                  // printf("dot2 = %d\n",dot2);
               }
            }


         	if(rx_buf[0]=='-')
         	{
            	act_int[0]=0;
               for (k=0;k<dot;k=k+1)
            	{
            	    actuation_data=actuation_data+act_int[k]*power_of_ten(dot-k-1);
               }
               for (k=dot+1;k<SERIAL_BUF_SIZE;k=k+1)
               {
               	actuation_data=actuation_data+act_int[k]*power_of_ten(dot-k);
               }
               actuation_data=actuation_data*power_of_ten(6)*(-1);
               actuation_data_int=(long int)actuation_data;
               if (actuation_data_int >2000000000||actuation_data_int<-2000000000)
               {
               	actuation_data_int = 0;
               }
               sensed_data.data[1] = actuation_data_int;
               printf("%ld\n",actuation_data_int);
               // printf("1i is %d\n",i);
         	}
         	else if(rx_buf[0]!='-')
         	{
               for (k=0;k<dot;k=k+1)
            	{
            	    actuation_data=actuation_data+act_int[k]*power_of_ten(dot-k-1);
               }
               for (k=dot+1;k<SERIAL_BUF_SIZE;k=k+1)
               {
               	actuation_data=actuation_data+act_int[k]*power_of_ten(dot-k);
               }
               actuation_data=actuation_data*power_of_ten(6);
               actuation_data_int=(long int)actuation_data;
               if (actuation_data_int >2000000000||actuation_data_int<-2000000000)
               {
               	actuation_data_int = 0;
               }
               sensed_data.data[1] = actuation_data_int;
               printf("%ld\n",actuation_data_int);
               // printf("2i is %d\n",i);
         	}

            if(rx_buf[SERIAL_BUF_SIZE]=='-')
            {  
               // k=0;
               mode_int[0]=0;
               for (k=0;k<dot2;k=k+1)
               {
                  mode=mode+mode_int[k]*power_of_ten(dot2-k-1);
                  // printf("int %d\n",mode_int[k]);
               }
               for (k=dot2+1;k<SERIAL_BUF_SIZE2;k=k+1)
               {
                  mode=mode+mode_int[k]*power_of_ten(dot2-k);
                  // printf("digit %d\n",mode_int[k]);
               }
               // mode=mode*power_of_ten(6)*(-1);
               mode=mode*(-1);
               mode_data_int=(long int)mode;
               if (mode_data_int>15||mode_data_int<1)
			   {
			   	mode_data_int=1;
			   }

			   sensed_data.data[2]=mode_data_int;
			   if (sensed_data.data[2]>15||sensed_data.data[2]<1)
			   {
			   	sensed_data.data[2]=1;
			   }
               printf("%ld\n",mode_data_int);
               // printf("Nagative %d",dot2);
               // printf("3i is %d\n",i);
            }
            else
            {
               k=0;
               for (k=0;k<dot2;k=k+1)
               {
                  mode=mode+mode_int[k]*power_of_ten(dot2-k-1);
                  // printf("int %d\n",mode_int[k]);
               }
               for (k=dot2+1;k<SERIAL_BUF_SIZE2;k=k+1)
               {
                  mode=mode+mode_int[k]*power_of_ten(dot2-k);
                  // printf("digit %d\n",mode_int[k]);
               }
               // mode=mode*power_of_ten(6);
               mode=mode;
               mode_data_int=(long int)mode;
               
               if (mode_data_int>15||mode_data_int<1)
			   {
			   	mode_data_int=1;
			   }

               sensed_data.data[2]=mode_data_int;
               if (sensed_data.data[2]>15||sensed_data.data[2]<1)
			   {
			   	sensed_data.data[2]=1;
			   }
               printf("%ld\n",mode_data_int);
               // printf("Positive %d\n",dot2);
               // printf("4i is %d\n",i);
            }
            
      }
      return 1;
 	}






//////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void estimate_period_skew(void) {
	// Estimate clock skew over a period only if the reference time has been updated.
	if (GLOSSY_IS_SYNCED()) {
		// Estimate clock skew based on previous reference time and the Glossy period.
		period_skew = get_t_ref_l() - (t_ref_l_old + (rtimer_clock_t)GLOSSY_PERIOD);
		// Update old reference time with the newer one.
		t_ref_l_old = get_t_ref_l();
		// If Glossy is still bootstrapping, count the number of consecutive updates of the reference time.
		if (GLOSSY_IS_BOOTSTRAPPING()) {
			// Increment number of consecutive updates of the reference time.
			skew_estimated++;
			// Check if Glossy has exited from bootstrapping.
			if (!GLOSSY_IS_BOOTSTRAPPING()) {
				// Glossy has exited from bootstrapping.
				leds_off(LEDS_RED);
				// Initialize Energest values.
				energest_init();
#if GLOSSY_DEBUG
				high_T_irq = 0;
				bad_crc = 0;
				bad_length = 0;
				bad_header = 0;
#endif /* GLOSSY_DEBUG */

			}
		}
	}
}
//// Store the schedule in each node
static inline void static_schedule(void)
{
	int k;
	for (k=0;k<superframe_len-1;k++)
	{
		if(node_id==schedule[k].src)
		{
			flow_info.dst = schedule[k].dst;
			flow_info.slot =schedule[k].slot;
		}
	}
}

/**
 * 
 */
PROCESS(tailored_lwb_print_process, "Tailored LWB print process");

PROCESS_THREAD(tailored_lwb_print_process, ev, data)
{
	PROCESS_BEGIN();
	
	while(1) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		// printf("Node id is: %d\n",node_id);
		// printf("%08ld %08ld %08ld %d\n",sensed_data1.data[0],sensed_data1.data[1],sensed_data1.data[2],node_id);
		if (node_id==flow1dst)
		{	
		printf("%08ld %08ld %08ld\n",sensed_data1.data[0],sensed_data1.data[1],sensed_data1.data[2]);
		// printf("%08ld %08ld %08ld %d %d\n",sensed_data1.data[0],sensed_data1.data[1],sensed_data1.data[2],trigger[0],(int)schedule[0].next_trigger);
		}

		if (node_id==flow2dst)
		{	
		 printf("%08ld %08ld %08ld\n",sensed_data2.data[0],sensed_data2.data[1],sensed_data2.data[2]);
		 // printf("%08ld %08ld %08ld %d %d\n",sensed_data2.data[0],sensed_data2.data[1],sensed_data2.data[2],trigger[1],(int)schedule[1].next_trigger);
		}

		if ((node_id !=flow1dst)&&(node_id!=flow2dst))
		{
			// printf("%08ld %08ld %08ld %d %d %d\n",sensed_data1.data[0],sensed_data1.data[1],sensed_data1.data[2],sensed_data1.src, sensed_data1.dst,allocated_slot1);
			// printf("%08ld %08ld %08ld trigger of loop1: %d %d\n",sensed_data1.data[0],sensed_data1.data[1],sensed_data1.data[2],trigger[0],(int)schedule[0].next_trigger);
			// printf("trigger of loop2: %d %d\n",trigger[1],(int)schedule[1].next_trigger);
			// printf("%d %d\n",schedule[0].next_trigger,run_time);
			long tx_time_us = energy_tx_time((run_time-last_time)*1000);
			long rx_time_us = energy_rx_time((run_time-last_time)*1000);
			long period_ms = (long)(run_time-last_time)*1000;
			long duty_cycle = 100 * (tx_time_us+rx_time_us) / (period_ms);    // still need to be divided by 1000, because period is in ms.		
			// printf("Radio on time %ld ms in %ld ms, duty cycle %ld.%01ld %%\n",
			// 	(tx_time_us+rx_time_us)/1000, period_ms, duty_cycle/1000, duty_cycle%1000);
			if (node_id == 108)
			{//3
				printf("%ld %ld %ld %ld\n", tx_time_us, rx_time_us, period_ms, duty_cycle);
			}
			else
			{
			unsigned char id_testarray[] = {(unsigned char)(node_id/100),(unsigned char)((node_id%100)/10),(unsigned char)(node_id%10)};

			//6
			unsigned char seq1_testarray[] = {(unsigned char)(sensed_data1.data[0]/100000),(unsigned char)((sensed_data1.data[0]%100000)/10000),
				(unsigned char)((sensed_data1.data[0]%10000)/1000), (unsigned char)((sensed_data1.data[0]%1000)/100),
				(unsigned char)((sensed_data1.data[0]%100)/10),(unsigned char)(sensed_data1.data[0]%10)};

			//6
			unsigned char seq2_testarray[] = {(unsigned char)(sensed_data2.data[0]/100000),(unsigned char)((sensed_data2.data[0]%100000)/10000),
				(unsigned char)((sensed_data2.data[0]%10000)/1000), (unsigned char)((sensed_data2.data[0]%1000)/100),
				(unsigned char)((sensed_data2.data[0]%100)/10),(unsigned char)(sensed_data2.data[0]%10)};
			//8
			unsigned char tx_testarray[] = {(unsigned char)(tx_time_us/10000000),(unsigned char)((tx_time_us%10000000)/1000000),
				(unsigned char)((tx_time_us%1000000)/100000),(unsigned char)((tx_time_us%100000)/10000),(unsigned char)((tx_time_us%10000)/1000),
				(unsigned char)((tx_time_us%1000)/100),(unsigned char)((tx_time_us%100)/10),(unsigned char)(tx_time_us%10)};

			//8
			unsigned char rx_testarray[] = {(unsigned char)(rx_time_us/10000000),(unsigned char)((rx_time_us%10000000)/1000000),
				(unsigned char)((rx_time_us%1000000)/100000),(unsigned char)((rx_time_us%100000)/10000),(unsigned char)((rx_time_us%10000)/1000),
				(unsigned char)((rx_time_us%1000)/100),(unsigned char)((rx_time_us%100)/10),(unsigned char)(rx_time_us%10)};

			//6
			unsigned char p_testarray[] = {(unsigned char)(period_ms/100000),(unsigned char)((period_ms%100000)/10000),
				(unsigned char)((period_ms%10000)/1000), (unsigned char)((period_ms%1000)/100),(unsigned char)((period_ms%100)/10),(unsigned char)(period_ms%10)};

			//5
			unsigned char dt_testarray[] = {(unsigned char)(duty_cycle/10000),(unsigned char)((duty_cycle%10000)/1000),
				(unsigned char)((duty_cycle%1000)/100),(unsigned char)((duty_cycle%100)/10),(unsigned char)(duty_cycle%10)};

			//3+6+6+8+8+6+5 = 42
			// unsigned char ack_testarray[] = {id_testarray, seq1_testarray, seq2_testarray, tx_testarray, rx_testarray, p_testarray, dt_testarray};
			unsigned char ack_testarray[] = {id_testarray[0],id_testarray[1],id_testarray[2], 
				seq1_testarray[0], seq1_testarray[1], seq1_testarray[2], seq1_testarray[3],seq1_testarray[4], seq1_testarray[5],
				seq2_testarray[0], seq2_testarray[1], seq2_testarray[2], seq2_testarray[3],seq2_testarray[4], seq2_testarray[5],
				tx_testarray[0],tx_testarray[1],tx_testarray[2],tx_testarray[3],tx_testarray[4],tx_testarray[5],tx_testarray[6],tx_testarray[7],
				rx_testarray[0],rx_testarray[1],rx_testarray[2],rx_testarray[3],rx_testarray[4],rx_testarray[5],rx_testarray[6],rx_testarray[7],
				p_testarray[0], p_testarray[1], p_testarray[2], p_testarray[3],p_testarray[4], p_testarray[5],
				dt_testarray[0],dt_testarray[1],dt_testarray[2],dt_testarray[3],dt_testarray[4]
			};


			// unsigned char ack_testarray[] = {
			// 	p_testarray[0], p_testarray[1], p_testarray[2], p_testarray[3],p_testarray[4], p_testarray[5]
			// };

			// uint8_t TINYOS_PACKET_ACK = 0;
			send_data_to_server(ack_testarray, 42, TINYOS_PACKET_ACK);
			}
			// debug();
			last_time = run_time;
		}

		switch(errno) {
		case NO_SYNC_PACKET:
			// printf("Glossy NOT received\n");
			break;
		case NO_ERROR:
			break;
		case DATA_SLOT_END:
			// printf("total rx %d nodes\n", rx_count);
			break;
		case DATA_TX_SUCC:
		case DATA_TX_FAIL:
		case DATA_TX_IGNORE:
		////
			// energy_update((run_time-last_time)*1000);
			// last_time = run_time;
		default:
			break;
		}
		errno   = NO_ERROR;
	}

	PROCESS_END();
}

/**
 * 
 */
static inline int8_t reset_parameters() {
	//data_slots  = 0;
	////
	data_slots = 0;
	rr_slots = 0;
	rx_count = 0;
	active_slots = 0;
		
	if(get_rx_cnt() == 0) {
		RR_SLOTS 		  = 0;
		//DATA_SLOTS = 0;
		////
		DATA_SLOTS = superframe_len;
		skew_estimated--;
		return NO_SYNC_PACKET;
	}
	else {
		run_time = sync_data.run_time;
		//RR_SLOTS = sync_data.rr_slots;
		////
		RR_SLOTS = 0;
		//DATA_SLOTS  = sync_data.data_slots;
		////
		DATA_SLOTS = superframe_len;
			
		SLEEP_SLOTS	= sync_data.sleep_slots;
		if(sync_data.sleep_slots>1) {
			t_ref_l_old = (rtimer_clock_t)(t_ref_l_old + ((SLEEP_SLOTS-1)%2)*RTIMER_SECOND);
		} else {
			SLEEP_SLOTS	= 1;
		}
		return NO_ERROR;
	}
}

/**
 * 
 */
void filled_superframe() {
	
	//sync_data.data_slots = max_data_slots;
	////
	sync_data.data_slots = superframe_len;
	
	if(IPI <= MINIMUM_LWB_ROUND) {
		sync_data.sleep_slots = MINIMUM_LWB_ROUND;
	} else {
		if(IPI%MINIMUM_LWB_ROUND == 0) {
			sync_data.sleep_slots = MINIMUM_LWB_ROUND;
		} else {
			sync_data.sleep_slots = MINIMUM_LWB_ROUND + IPI%MINIMUM_LWB_ROUND;
		}
	}
}

/**
 * 
 */
void bare_superframe(uint8_t sleep_duration) {
	sync_data.data_slots  = 0;
	sync_data.sleep_slots = sleep_duration;
}

/**
 * 
 */
void prepare_next_superframe() {
		
	sync_data.run_time = run_time;	

	if(run_time < COOLOFF_PERIOD) {
		/* before anything starts, just keep sending sync packet every seconds, 
		 * no other slots in the superframe */
		sync_data.rr_slots = 0;
		bare_superframe(1);
	} else if(run_time < STABILIZATION_PERIOD) {
		if(run_time == COOLOFF_PERIOD) {
			//sync_data.rr_slots = MAX_RR_SLOTS_P_SECOND;
			////
			sync_data.rr_slots = 0;
		}
		/* send sync packets every seconds */
		bare_superframe(1);				
	} else if(run_time%(IPI) == 0) {
		/* allow only minimum RR slots, if some node is yet to get a slot */
		//sync_data.rr_slots = MIN_RR_SLOTS;
		////
		sync_data.rr_slots = 0;
		/* fill the superframe with suitable slots */
		filled_superframe();
	} else if(run_time%(MINIMUM_LWB_ROUND) == 0){
		/* send sync packet every MINIMUM_LWB_ROUND, if IPI is equal to this, then this does not arise */
		sync_data.rr_slots = 0;
		bare_superframe(MINIMUM_LWB_ROUND);
	} else {
		/* in all other cases, send sync packet every second, without any contention slot */
		sync_data.rr_slots = 0;
		bare_superframe(1);
	}
}

/**
 * 
 */
void next_radio_activity_schedule(struct rtimer *t, void *ptr) {
		
	rtimer_clock_t GRACE_PERIOD = 0;
	
	if(data_slots < DATA_SLOTS) {
		NEXT_SLOT = (rtimer_clock_t)(NEXT_SLOT + DATA_SLOT_LEN);
		if(flow_info.slot == data_slots+1) {
			GRACE_PERIOD = TX_GUARD_TIME;
		}
	}
	else if(rr_slots < RR_SLOTS) {
		NEXT_SLOT = (rtimer_clock_t)(NEXT_SLOT + RR_SLOT_LEN);
		
		if(rr_slots%MIN_RR_SLOTS==0) {
			if(IS_SINK()) {
				GRACE_PERIOD = (-1)*TX_GUARD_TIME;
			}
		} else if(rr_slots%MIN_RR_SLOTS==1) {
			if(IS_SINK()) {
				GRACE_PERIOD = TX_GUARD_TIME;
			}
		} else if(rr_slots%MIN_RR_SLOTS==2) {
			if(req_reply.dst == node_id) {
				GRACE_PERIOD = TX_GUARD_TIME;
			}
		}
	}
	else if(NEXT_SLOT < RTIMER_SECOND) { 
		NEXT_SLOT = RTIMER_SECOND;
	}
	
	/* if a second has been passed, add it to the RX_TIME */
	if(NEXT_SLOT >= RTIMER_SECOND) {
		NEXT_SLOT   = (rtimer_clock_t)(NEXT_SLOT - RTIMER_SECOND);
		REF_TIME    = (rtimer_clock_t)(REF_TIME + RTIMER_SECOND);
		period++;
		run_time++;
		if(SLEEP_SLOTS > 0) {
			SLEEP_SLOTS--;
		}
		
		if(SLEEP_SLOTS==0) {
			if(!IS_SINK()) {
				//GRACE_PERIOD = GLOSSY_GUARD_TIME * (1 + sync_missed) * (-1);
				GRACE_PERIOD = GLOSSY_GUARD_TIME * (-1);
			}
		}
	}
	
	/* schedule next radio activity */
	rtimer_set(t, (rtimer_clock_t)(REF_TIME + NEXT_SLOT + GRACE_PERIOD), 1, 
				(rtimer_callback_t)tailored_lwb_scheduler, ptr);
				
	if(SLEEP_SLOTS==0) {
		if(DATA_SLOTS) {
			process_poll(&tailored_lwb_print_process);
		}

		/* load own data slots, and slot it is going to participate */
		if(run_time == STABILIZATION_PERIOD) {
			load_forwarder_selection_vector();
		}
		
		/* sink node decides structure of the next superframe */
		if (IS_SINK()) {
			prepare_next_superframe();
		}
	}
	
}

/**
 * 
 */
void node_init() {
	
	run_time = 0;
	last_time = run_time;
	max_data_slots = superframe_len;
		
	if (IS_SINK()) {
		data_slots	  = 0;
		sync_data.run_time	  = 0;
		sync_data.rr_slots    = 0;
		sync_data.sleep_slots = 1;
	} 
	////
}

/**
 * 
 */
void prepare_data() {
	
	sensed_data.dst = 0;
	
	if(flow_info.slot == data_slots) {
		////
		//sensed_data.data_len = 0;
		sensed_data.src 	   = node_id;
		sensed_data.dst 	   = flow_info.dst;
		sensed_data.data_len   = 3;
		if (node_id==flow1src)
		{
		// sensed_data.data[0]++;
		sensed_data.data[0]=sensed_data1.data[0];
		// sensed_data.data[1]=0;
		// sensed_data.data[1]=2;
		 // sensed_data.data[1]=sensed_data.data[1]+3;
		uart1_init(BAUD2UBR(115200));
		uart1_set_input(uart_rx_callback);
		sensed_data.data[1]=actuation_data_int;
		sensed_data.data[2]=mode_data_int;
		schedule[0].next_trigger = mode_data_int;
		}

		if (node_id==flow2src)
		{
		// sensed_data.data[0]++;
		sensed_data.data[0]=sensed_data2.data[0];
		// sensed_data.data[1]=0;
		 // sensed_data.data[1]=sensed_data.data[1]+3;
		uart1_init(BAUD2UBR(115200));
		uart1_set_input(uart_rx_callback);
		sensed_data.data[1]=actuation_data_int;
		sensed_data.data[2]=mode_data_int;
		schedule[1].next_trigger = mode_data_int;
		////
		//schedule[1].next_trigger = 4;
		}

		errno = DATA_TX_SUCC;
		FLOODING_ROLE = GLOSSY_INITIATOR;
	} 
	else if(decide_participation(data_slots)){
		FLOODING_ROLE = GLOSSY_RECEIVER;
	}
	else {
		FLOODING_ROLE = GLOSSY_NO_FLOODING;
	}
}

/**
 * 
 */
void process_data() {
	
	/* if the data is not sent, set error code to TX_FAIL */
	if(FLOODING_ROLE == GLOSSY_INITIATOR && get_rx_cnt()==0) {
		errno = DATA_TX_FAIL;
	}
	
	/* increase the number of packet received */
	if(sensed_data.dst == node_id) {
		rx_count++;
	}
	
	/* at the end of data slots */
	if(data_slots == DATA_SLOTS) {
		if(IS_SINK()) {
			errno = DATA_SLOT_END;
		}
	}
}



/** @} */

/**
 * \defgroup lwb-test-scheduler Periodic scheduling
 * @{
 */

char tailored_lwb_scheduler(struct rtimer *t, void *ptr) {
	
	PT_BEGIN(&pt);

	node_init();

	int k;

	uint8_t src, dst;
	
	while (1) {
		if(data_slots < DATA_SLOTS) {
			/* account for how many data slots have elapsed */
			data_slots++;
			
			/* prepare the data packet for the global slot */
			prepare_data();


			if (sensed_data.src == flow1src && sensed_data.dst == flow1dst)
			{

				schedule[0].next_trigger = sensed_data.data[2];

			}
			if (sensed_data.src==flow2src && sensed_data.dst == flow2dst)
			{

				schedule[1].next_trigger = sensed_data.data[2];
			}	

			////FLOODING ROLE
			for (k=0;k<superframe_len;k++)
			{
				// if(trigger==1) 
				if(data_slots == schedule[k].slot)
				{
					if (trigger[k]==1)//added by yehan, to avoid lose synchronization
					{
						if (node_id == schedule[k].src)
						{
							// if (trigger[k]==1)
							// {
							FLOODING_ROLE = GLOSSY_INITIATOR;
							if (node_id == flow1src)
							{
								sensed_data.data[0]=sensed_data1.data[0]+1;
							}
							if (node_id == flow2src)
							{
								sensed_data.data[0]=sensed_data2.data[0]+1;
							}
							// }
							// else
							// {
							// 	FLOODING_ROLE = GLOSSY_NO_FLOODING;
							// }
							// trigger[k] == schedule[k].next_trigger;
						} 
						else 
						{
							FLOODING_ROLE = GLOSSY_RECEIVER;
							// trigger[k] == schedule[k].next_trigger;
						}
					}
					else
					{
						FLOODING_ROLE = GLOSSY_NO_FLOODING;
						// trigger[k] = trigger[k]-1;
						// if (trigger[k]<=0)
						// {
						// 	trigger[k]=1;
						// }
					}
					
				}

			}

			/* selective nodes participate (using flooding) in delivering the data */
			if(FLOODING_ROLE != GLOSSY_NO_FLOODING) {	
				tailored_glossy_start((uint8_t *)&sensed_data, SDATA_LEN, FLOODING_ROLE, FLOODING, GLOSSY_SYNC, D_TX,
						APPLICATION_HEADER, (rtimer_clock_t)(RTIMER_TIME(t) + DATA_SLOT_DURATION), 
						(rtimer_callback_t)tailored_lwb_scheduler, t, ptr);
				PT_YIELD(&pt);
				tailored_glossy_stop(&src, &dst);
				active_slots += 2;

			}

			if (sensed_data.src == flow1src && sensed_data.dst == flow1dst)
			{
				sensed_data1 = sensed_data;
				allocated_slot1 = data_slots; 
				schedule[0].next_trigger = sensed_data.data[2];

			}
			if (sensed_data.src==flow2src && sensed_data.dst == flow2dst)
			{
				sensed_data2 = sensed_data;
				allocated_slot2 = data_slots;
				schedule[1].next_trigger = sensed_data.data[2];
			}


			for (k=0;k<superframe_len;k++)
			{
				// if(trigger==1) 
				if(data_slots == schedule[k].slot)
				{
					if (trigger[k]==1)
					{

							// FLOODING_ROLE = GLOSSY_INITIATOR;
						if (k ==0)
						{
							if (last_update[k]!=sensed_data1.data[0])
							{
								trigger[k] = (int)schedule[k].next_trigger;
								last_update[k]=sensed_data1.data[0];
							}
							else
							{
								trigger[k] = trigger[k]-1;
							}
							
						}

						if (k ==1)
						{
							if (last_update[k]!=sensed_data2.data[0])
							{
								trigger[k] = (int)schedule[k].next_trigger;
								last_update[k]=sensed_data2.data[0];
							}
							else
							{
								trigger[k] = trigger[k]-1;
							}
							
						}
						


					}
					else
					{
						// FLOODING_ROLE = GLOSSY_NO_FLOODING;
						trigger[k] = trigger[k]-1;
					}
						// if (trigger[k]<=0)
						// {
						// 	trigger[k]=(int)schedule[k].next_trigger;
						// }
						// if (trigger[k] <=-(int)schedule[k].next_trigger)
						if (trigger[k] <=0)
						{
							schedule[k].next_trigger = 1;
							trigger[k] = 1;

							if (k ==0)
							{
									last_update[k]=sensed_data1.data[0];
								
							}
							if (k ==1)
							{
									last_update[k]=sensed_data2.data[0];
								
							}

						}
					
				}

			}


			
			/* schedule the next radio activity */
			next_radio_activity_schedule(t, ptr);
	
			/* process the data received in the global slot */
			process_data();
		}
		////
		
		else if(SLEEP_SLOTS > 0) {
			/* in the sleep slot nothing to be done */
			/* schedule the next radio activity */
			next_radio_activity_schedule(t, ptr);
		} 
		else {
			if (IS_SINK()) {
				// Glossy phase.
				leds_on(LEDS_GREEN);
				rtimer_clock_t t_stop = RTIMER_TIME(t) + GLOSSY_DURATION;
				// Start Glossy.
				tailored_glossy_start((uint8_t *)&sync_data, SYNC_LEN, GLOSSY_INITIATOR, FLOODING, GLOSSY_SYNC, N_TX,
						APPLICATION_HEADER, t_stop, (rtimer_callback_t)tailored_lwb_scheduler, t, ptr);
				// Store time at which Glossy has started.
				REF_TIME = RTIMER_TIME(t);
				
				// Yield the protothread. It will be resumed when Glossy terminates.
				PT_YIELD(&pt);

				// Off phase.
				leds_off(LEDS_GREEN);
				// Stop Glossy.
				tailored_glossy_stop(&src, &dst);
				if (!GLOSSY_IS_BOOTSTRAPPING()) {
					// Glossy has already successfully bootstrapped.
					if (!GLOSSY_IS_SYNCED()) {
						// The reference time was not updated: increment reference time by GLOSSY_PERIOD.
						set_t_ref_l(GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD);
						set_t_ref_l_updated(1);
					}
				}
				
				// Estimate the clock skew over the last period.
				estimate_period_skew();
				
				if (GLOSSY_IS_BOOTSTRAPPING()) {
					rtimer_set(t, REF_TIME + GLOSSY_PERIOD, 1, (rtimer_callback_t)tailored_lwb_scheduler, ptr);
				} else {
					NEXT_SLOT  = GLOSSY_SYNC_GUARD;
					reset_parameters();				
					rtimer_set(t, REF_TIME + NEXT_SLOT, 1, (rtimer_callback_t)tailored_lwb_scheduler, ptr);
				}
			} else{
				leds_on(LEDS_GREEN);
				rtimer_clock_t t_stop;
				if (GLOSSY_IS_BOOTSTRAPPING()) {
					// Glossy is still bootstrapping:
					// Schedule end of Glossy phase based on GLOSSY_INIT_DURATION.
					t_stop = RTIMER_TIME(t) + GLOSSY_INIT_DURATION;
				} else {
					// Glossy has already successfully bootstrapped:
					// Schedule end of Glossy phase based on GLOSSY_DURATION.
					t_stop = RTIMER_TIME(t) + GLOSSY_DURATION;
				}
				// Start Glossy.
				tailored_glossy_start((uint8_t *)&sync_data, SYNC_LEN, GLOSSY_RECEIVER, FLOODING, GLOSSY_SYNC, N_TX,
						APPLICATION_HEADER, t_stop, (rtimer_callback_t)tailored_lwb_scheduler, t, ptr);
				// Yield the protothread. It will be resumed when Glossy terminates.
				PT_YIELD(&pt);
				
				// Off phase.
				leds_off(LEDS_GREEN);
				// Stop Glossy.
				tailored_glossy_stop(&src, &dst);
				if (GLOSSY_IS_BOOTSTRAPPING()) {
					// Glossy is still bootstrapping.
					if (!GLOSSY_IS_SYNCED()) {
						// The reference time was not updated: reset skew_estimated to zero.
						skew_estimated = 0;
					}
				} else {
					// Glossy has already successfully bootstrapped.
					if (!GLOSSY_IS_SYNCED()) {
						// The reference time was not updated:
						// increment reference time by GLOSSY_PERIOD + period_skew.
						set_t_ref_l(GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD + period_skew);
						set_t_ref_l_updated(1);
						// Increment sync_missed.
						sync_missed++;
					} else {
						// The reference time was not updated: reset sync_missed to zero.
						sync_missed = 0;
					}
				}
				// Estimate the clock skew over the last period.
				estimate_period_skew();
				if (GLOSSY_IS_BOOTSTRAPPING()) {
					// Glossy is still bootstrapping.
					if (skew_estimated == 0) {
						rtimer_set(t, RTIMER_TIME(t) + GLOSSY_INIT_PERIOD, 1,
								(rtimer_callback_t)tailored_lwb_scheduler, ptr);
					} else {
						REF_TIME = GLOSSY_REFERENCE_TIME + GLOSSY_PERIOD;
						rtimer_set(t, REF_TIME - GLOSSY_INIT_GUARD_TIME, 1,
									(rtimer_callback_t)tailored_lwb_scheduler, ptr);
					}
				} else {
					offset_err = GLOSSY_REFERENCE_TIME - (rtimer_clock_t)REF_TIME;
					REF_TIME   = GLOSSY_REFERENCE_TIME;
					NEXT_SLOT  = GLOSSY_SYNC_GUARD;
					errno = reset_parameters();
					rtimer_set(t, REF_TIME + NEXT_SLOT, 1, (rtimer_callback_t)tailored_lwb_scheduler, ptr);
					if(errno == NO_SYNC_PACKET) {
						process_poll(&tailored_lwb_print_process);
					} else {
						hop_count = get_my_hop();
					}
				}
			}
		}
		// Yield the protothread.
		PT_YIELD(&pt);
	}
	
	PT_END(&pt);
}
	

PROCESS(tailored_lwb_process, "LWB Process");
PROCESS_THREAD(tailored_lwb_process, ev, data)
{
	PROCESS_BEGIN();
	
	initiate_energy_accounting();
	static_schedule();
	
	leds_on(LEDS_RED);
	schedule[0].src = flow1src;
	schedule[0].dst = flow1dst;
	schedule[1].src = flow2src;
	schedule[1].dst = flow2dst;
	schedule[2].src = flow3src;
	schedule[2].dst = flow3dst;

	sensed_data.data[0] = 0;
	sensed_data.data[1] = 0;
	sensed_data.data[2] = 1;

	sensed_data1 = sensed_data;
	sensed_data2 = sensed_data;
	allocated_slot1 = 0;
	allocated_slot2 = 0;
	// Start print stats processes.
	process_start(&tailored_lwb_print_process, NULL);

	// Start Glossy busy-waiting process.
	process_start(&tailored_glossy_process, NULL);
	// Start Glossy experiment in one second.
	//rtimer_set(&rt, RTIMER_NOW() + RTIMER_SECOND, 1, (rtimer_callback_t)tailored_lwb_scheduler, NULL);
	rtimer_set(&rt, RTIMER_NOW() + RTIMER_SECOND, 1, (rtimer_callback_t)tailored_lwb_scheduler, NULL);
	
	PROCESS_END();
}

/** @} */
/** @} */
/** @} */
