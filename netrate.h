#ifndef _NETRATE_H
#define _NETRATE_H

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
/* FIXME take value from kernel headers; linux/if.h seems not to include by itself */
#endif

#include "list.h"

struct rx_stat {
	unsigned int bytes;
	unsigned int packets;
	unsigned int errs;
	unsigned int drop;
	unsigned int fifo;
	unsigned int frame;
	unsigned int compressed;
	unsigned int multicast;
};

struct tx_stat {
	unsigned int bytes;
	unsigned int packets;
	unsigned int errs;
	unsigned int drop;
	unsigned int fifo;
	unsigned int colls;
	unsigned int carrier;
	unsigned int compressed;
};

struct if_stat {
	struct list_head lh;
	struct timeval tv;
	struct rx_stat rx;
	struct tx_stat tx;
};

struct if_data {
	struct list_head lh;
	char name[IFNAMSIZ];
	struct list_head stat;
	int stat_size;
};

struct if_tmp {
	struct list_head lh;
	char name[IFNAMSIZ];
	struct if_stat stat;
};

#endif
