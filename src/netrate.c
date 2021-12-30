/* SPDX-License-Identifier: GPL-2.0 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/time.h>

#include "netrate.h"

LIST_HEAD(if_data);
int backlog = 3;

char *parse_line(char *s, struct rx_stat *rx, struct tx_stat *tx)
{
	char *ret;

	s += strspn(s, " ");
	ret = s;

	s += strcspn(s, ":");
	if (*s == '\0')
		return NULL;

	*(s++) = '\0';

	if (sscanf(s, "%llu%lu%lu%lu%lu%lu%lu%lu%llu%lu%lu%lu%lu%lu%lu%lu",
				&rx->bytes,
				&rx->packets,
				&rx->errs,
				&rx->drop,
				&rx->fifo,
				&rx->frame,
				&rx->compressed,
				&rx->multicast,
				&tx->bytes,
				&tx->packets,
				&tx->errs,
				&tx->drop,
				&tx->fifo,
				&tx->colls,
				&tx->carrier,
				&tx->compressed
			  ) < 16)
		return NULL;

	return ret;
}

int read_line(char *buf, size_t size, FILE *f)
{
	int count = -1;
	size_t len;

	do {
		count++;
		if (fgets(buf, size, f) == NULL)
			return EIO;
		len = strlen(buf);
		assert(len);
	} while (buf[len - 1] != '\n');

	return count ? ENOSPC : 0;
}

void process_stats(struct list_head *lh)
{
	struct if_data *ifd, *__ifd;
	struct if_tmp *ift;
	struct if_stat *ifs;

	/* check if any interface disappeared; this is O(n^2) */
	list_for_each_entry_safe(ifd, __ifd, &if_data, lh) {
		int found = 0;

		list_for_each_entry(ift, lh, lh) {
			if (strcmp(ift->name, ifd->name))
				continue;
			found = 1;
			break;
		}

		if (found)
			continue;

		list_del(&ifd->lh);
		free(ifd);
	}

	/* update stats for all interfaces */
	list_for_each_entry(ift, lh, lh) {
		int found = 0;

		list_for_each_entry(ifd, &if_data, lh) {
			if (strcmp(ift->name, ifd->name))
				continue;
			found = 1;
			break;
		}

		if (!found) {
			ifd = malloc(sizeof(struct if_data));
			assert(ifd);
			strcpy(ifd->name, ift->name);
			INIT_LIST_HEAD(&ifd->stat);
			ifd->stat_size = 0;
			list_add_tail(&ifd->lh, &if_data);
		}

		if (ifd->stat_size >= backlog) {
			ifs = list_entry(ifd->stat.prev, struct if_stat, lh);
			list_del(&ifs->lh);
			list_add(&ifs->lh, &ifd->stat);
		} else {
			ifs = malloc(sizeof(struct if_stat));
			assert(ifs);
			list_add(&ifs->lh, &ifd->stat);
			ifd->stat_size++;
		}

		ifs->tv = ift->stat.tv;
		ifs->rx = ift->stat.rx;
		ifs->tx = ift->stat.tx;
	}
}

/* compute t1 - t2 in milliseconds */
inline double time_diff(struct timeval *t1, struct timeval *t2)
{
	return (double)(t1->tv_sec - t2->tv_sec) * 1000 +
			(double)(t1->tv_usec - t2->tv_usec) / 1000;
}

void display_stats(void)
{
	struct if_data *ifd;

	fputs("\033[2J\033[1;1H", stdout);
	printf("%8s %16s %16s %16s %16s\n\n",
			"Device", "RX kbytes/s", "RX kpackets/s", "TX kbytes/s", "TX kpackets/s");
	list_for_each_entry(ifd, &if_data, lh) {
		struct if_stat *latest = list_entry(ifd->stat.next, struct if_stat, lh);
		struct if_stat *earliest = list_entry(ifd->stat.prev, struct if_stat, lh);
		double factor = ifd->stat_size > 1 ?
			1.0 / time_diff(&latest->tv, &earliest->tv) : 0;

		printf("%8s %16.2f %16.2f %16.2f %16.2f\n",
				ifd->name,
				(double)(latest->rx.bytes - earliest->rx.bytes) * 1000 / 1024 * factor,
				(double)(latest->rx.packets - earliest->rx.packets) * factor,
				(double)(latest->tx.bytes - earliest->tx.bytes) * 1000 / 1024 * factor,
				(double)(latest->tx.packets - earliest->tx.packets) * factor
			  );

		/*
		if (!strcmp(ifd->name, "eth2")) {
			printf("debug %u %u %u %u %p %p %f %f\n", latest->rx.bytes, earliest->rx.bytes, latest->rx.packets, earliest->rx.packets, latest, earliest, time_diff(&latest->tv, &earliest->tv), factor);
		}
		*/
	}
}

int main(int argc, char **argv)
{
	FILE *f;
	int status;
	char buf[4096];
	struct timeval now;
	struct if_tmp *if_tmp, *__if_tmp;

	do {
		LIST_HEAD(lh);
		f = fopen("/proc/net/dev", "r");
		assert(f);
		/* skip table header */
		status = read_line(buf, sizeof(buf), f);
		assert(!status);
		status = read_line(buf, sizeof(buf), f);
		assert(!status);
		/* parse data */
		gettimeofday(&now, NULL);
		while (!read_line(buf, sizeof(buf), f)) {
			struct rx_stat rx;
			struct tx_stat tx;
			char *if_name = parse_line(buf, &rx, &tx);

			if (if_name == NULL)
				continue;

			if_tmp = malloc(sizeof(struct if_tmp));
			assert(if_tmp);
			strncpy(&if_tmp->name[0], if_name, IFNAMSIZ);
			if_tmp->stat.tv = now;
			if_tmp->stat.rx = rx;
			if_tmp->stat.tx = tx;
			list_add_tail(&if_tmp->lh, &lh);
		}
		fclose(f);
		process_stats(&lh);
		display_stats();
		list_for_each_entry_safe(if_tmp, __if_tmp, &lh, lh) {
			free(if_tmp);
		}
		sleep(1);
	} while (1);

	return 0;
}
