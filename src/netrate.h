/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NETRATE_H
#define _NETRATE_H

#include <net/if.h>

#include "list.h"

struct rx_stat {
	unsigned long long bytes;
	unsigned long packets;
	unsigned long errs;
	unsigned long drop;
	unsigned long fifo;
	unsigned long frame;
	unsigned long compressed;
	unsigned long multicast;
};

struct tx_stat {
	unsigned long long bytes;
	unsigned long packets;
	unsigned long errs;
	unsigned long drop;
	unsigned long fifo;
	unsigned long colls;
	unsigned long carrier;
	unsigned long compressed;
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
