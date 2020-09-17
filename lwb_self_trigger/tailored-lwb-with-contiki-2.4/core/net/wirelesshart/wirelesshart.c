#include "net/netstack.h"
#include "wirelesshart.h"
/*---------------------------------------------------------------------------*/
static void
packet_sent(void *ptr, int status, int num_tx)
{
  struct channel *c = ptr;
  struct rime_sniffer *s;

  switch(status) {
  case MAC_TX_COLLISION:
    printf("wirelesshart: collision after %d tx\n", num_tx);
    break;
  case MAC_TX_NOACK:
    printf("wirelesshart: noack after %d tx\n", num_tx);
    break;
  case MAC_TX_OK:
    printf("wirelesshart: sent after %d tx\n", num_tx);
    break;
  default:
    printf("wirelesshart: error %d after %d tx\n", status, num_tx);
  }
}

int
wirelesshart_output(void)
{
    NETSTACK_LLSEC.send(packet_sent, NULL);
    return 1;
}

void wirelesshart_unicast_send(const linkaddr_t *receiver){
	packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, receiver);
    packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
	wirelesshart_output();
}

static void
input(void)
{

    unsigned char message[100];
    int i = 0;
    for(i = 0; i < 100; i++){
        message[i] = 0;
    }

    printf("Message received...\n");
    packetbuf_copyto(message);
    printf(message);

    //printf("message received...\n");
}


static void
init(void)
{
  queuebuf_init();
  packetbuf_clear();
}
/*---------------------------------------------------------------------------*/
const struct network_driver wirelesshart_driver = {
  "WirelessHART",
  init,
  input
};
/** @} */

