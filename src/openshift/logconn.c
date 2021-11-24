#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <assert.h>

#include <haproxy/connection-t.h>

#include <openshift/ocprintf.h>
#include <openshift/logconn.h>

void openshift_logconn(const char *ident,
		       FILE *stream,
		       const char *filename,
		       int linenumber,
		       const char *funcname,
		       struct connection *conn,
		       int fd,
		       const char *fmt, ...)
{
	char addr[INET6_ADDRSTRLEN] = { '0', '\0' };
	int port = -1;
	va_list ap;

	assert(stream != NULL);
	assert(conn != NULL);
	assert(conn->dst->ss_family == AF_INET);
	inet_ntop(AF_INET, &(((struct sockaddr_in *)conn->dst)->sin_addr), addr, INET6_ADDRSTRLEN);
	port = ntohs(((struct sockaddr_in *)conn->dst)->sin_port);
	openshift_printf(ident, stdout, filename, linenumber, funcname, "fd %d server %s:%d ", fd, addr, port);
	va_start(ap, fmt);
	vfprintf(stream, fmt, ap);
	va_end(ap);
	fflush(stream);
}
