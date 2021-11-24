/* close interceptor */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

#include <openshift/ocprintf.h>

#undef NDEBUG /* always want an assert to fire */
#include <assert.h>

/* interposed function. */
static int (*libc_close)(int fd);

static FILE *stream;

static char *logname(char *buf, size_t bufsz) {
	snprintf(buf, bufsz, "/tmp/haproxy-%zd.close", (size_t)getpid());
	snprintf(buf, bufsz, "/proc/1/fd/1"); /* choose for OCP cluster */
	return buf;
}

int close(int fd)
{
	int rc;

	if (stream == NULL) {
		char fname[1024];
		if ((stream = fopen(logname(fname, sizeof(fname)), "w")) == NULL) {
			fprintf(stderr, "error opening %s: %s; switching to stdout\n", fname, strerror(errno));
			stream = stdout;
		}
	}

	assert(stream);

	if (libc_close == NULL) {
		if ((libc_close = dlsym(RTLD_NEXT, "close")) == NULL) {
			fprintf(stderr, "error: dlsym(close): %s\n", strerror(errno));
			exit(EXIT_FAILURE); /* has to be fatal */
		}
	}

	switch ((rc = libc_close(fd))) {
	case -1:
		OCPRINTF("LIBC_CLOSE", stream, "close(%d) = %d (errno %d)\n", fd, rc, errno);
		break;
	default:
		OCPRINTF("LIBC_CLOSE", stream, "close(%d) = %d\n", fd, rc);
		break;
	}

	return rc;
}
