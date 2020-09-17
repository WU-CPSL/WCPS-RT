#include "contiki.h"
#include "dev/serial-line.h"
#include "net/queuebuf.h"
//#include "net/linkaddr.h"
#include "net/packetbuf.h"

#include "net/mac/mac.h"

int wirelesshart_output(void);
void wirelesshart_unicast_send(const linkaddr_t *receiver);

