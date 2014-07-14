#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/llc.h>
#include <stdlib.h>
#include "config.h"
#include "net.h"
#include "maps.h"	// page_rand
#include "random.h"
#include "utils.h"	// ARRAY_SIZE
#include "compat.h"

void llc_gen_sockaddr(struct sockaddr **addr, socklen_t *addrlen)
{
	struct sockaddr_llc *llc;
	unsigned int i;

	llc = zmalloc(sizeof(struct sockaddr_llc));

	llc->sllc_family = AF_LLC;
	llc->sllc_arphrd = ARPHRD_ETHER;
	llc->sllc_test = rand();
	llc->sllc_xid = rand();
	llc->sllc_ua = rand();
	llc->sllc_sap = rand();
	for (i = 0; i < IFHWADDRLEN; i++)
		llc->sllc_mac[i] = rand();
	*addr = (struct sockaddr *) llc;
	*addrlen = sizeof(struct sockaddr_llc);
}

void llc_rand_socket(struct socket_triplet *st)
{
	st->protocol = rand() % PROTO_MAX;
	if (rand_bool())
		st->type = SOCK_STREAM;
	else
		st->type = SOCK_DGRAM;
}

#define SOL_LLC 268

#ifndef USE_LLC_OPT_PKTINFO
#define LLC_OPT_PKTINFO LLC_OPT_UNKNOWN
#endif

#define NR_SOL_LLC_OPTS ARRAY_SIZE(llc_opts)
static const unsigned int llc_opts[] = {
	LLC_OPT_RETRY, LLC_OPT_SIZE, LLC_OPT_ACK_TMR_EXP, LLC_OPT_P_TMR_EXP,
	LLC_OPT_REJ_TMR_EXP, LLC_OPT_BUSY_TMR_EXP, LLC_OPT_TX_WIN, LLC_OPT_RX_WIN,
	LLC_OPT_PKTINFO
};

void llc_setsockopt(struct sockopt *so)
{
	unsigned char val;

	so->level = SOL_LLC;

	val = rand() % NR_SOL_LLC_OPTS;
	so->optname = llc_opts[val];
}
