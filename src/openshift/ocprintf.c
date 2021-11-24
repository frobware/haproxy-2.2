#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <assert.h>

#include <haproxy/connection-t.h>
#include <openshift/logconn.h>
#include <openshift/ocprintf.h>

#define TIMENOW_RFC8601_SIZE 64
#define TIMENOW_FMT "%Y-%m-%d %H:%M:%S."

/*
 * Returns current time formatted as %Y-%m-%dT%H:%M%S.%06Z.
 *
 * For example "2021-12-24T14:20:22.987654Z".
 */
static char *timenow_rfc8601(char buf[]) /* rfc8601-ish */
{
	struct timespec now;
	struct tm tv;
	(void)clock_gettime(CLOCK_REALTIME, &now);
	gmtime_r(&now.tv_sec, &tv);
	strftime(buf, 21, TIMENOW_FMT, &tv);
	sprintf(buf + 20, "%06luZ", now.tv_nsec/1000);
	return buf;
}

void openshift_printf(const char *ident,
		      FILE *stream,
		      const char *filename,
		      int linenumber,
		      const char *funcname,
		      const char *fmt, ...)
{
	va_list ap;
	char ts[TIMENOW_RFC8601_SIZE];

	if (stream != NULL) {
		fprintf(stream, "%s [%d] %s [%s:%d %s] ", timenow_rfc8601(ts), getpid(), ident, filename, linenumber, funcname);
		va_start(ap, fmt);
		vfprintf(stream, fmt, ap);
		va_end(ap);
		fflush(stream);
	}
}
