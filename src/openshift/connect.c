/* connect interceptor */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <openshift/logconn.h>

#undef NDEBUG /* always want an assert to fire */
#include <assert.h>

/* interposed function. */
static int (*libc_connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

static FILE *stream;

static char *logname(char *buf, size_t bufsz) {
	snprintf(buf, bufsz, "/tmp/haproxy-%zd.close", (size_t)getpid());
	snprintf(buf, bufsz, "/proc/1/fd/1"); /* choose for OCP cluster */
	return buf;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int rc;
	int port = -1;
	char addrstr[INET6_ADDRSTRLEN] = { '0', '\0' };

	if (stream == NULL) {
		char fname[1024];
		if ((stream = fopen(logname(fname, sizeof(fname)), "w")) == NULL) {
			fprintf(stderr, "error opening %s: %s; switching to stdout\n", fname, strerror(errno));
			stream = stdout;
		}
	}

	assert(stream);

	if (libc_connect == NULL) {
		if ((libc_connect = dlsym(RTLD_NEXT, "connect")) == NULL) {
			fprintf(stderr, "error: dlsym(connect): %s\n", strerror(errno));
			exit(EXIT_FAILURE); /* has to be fatal */
		}
	}

	if (addr->sa_family == AF_INET) {
		inet_ntop(AF_INET, &((struct sockaddr_in *)addr)->sin_addr, addrstr, INET6_ADDRSTRLEN);
		port = ntohs(((struct sockaddr_in *)addr)->sin_port);
	}

	switch ((rc = libc_connect(sockfd, addr, addrlen))) {
	case -1:
		OCPRINTF("LIBC_CONNECT", stream, "connect sockfd %d %s:%d = %d (errno %d)\n", sockfd, addrstr, port, rc, errno);
		break;
	default:
		OCPRINTF("LIBC_CONNECT", stream, "connect sockfd %d %s:%d = %d\n", sockfd, addrstr, port, rc);
		break;
	}

	return rc;
}
