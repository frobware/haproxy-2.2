#ifndef _inc_OPENSHIFT_LOGCONN_H
#define _inc_OPENSHIFT_LOGCONN_H

#include <stdio.h>
#include <stdarg.h>

#include <haproxy/connection-t.h>
#include <openshift/ocprintf.h>

extern void openshift_logconn(const char *ident,
			      FILE *stream,
			      const char *filename,
			      int linenumber,
			      const char *funcname,
			      struct connection *conn,
			      int fd,
			      const char *fmt, ...)
	__attribute__((format(printf, 8, 9)));

#define LOG_HCHK_CONN(IDENT, STREAM, CONN, FD, FMT, ...)	\
  do {								\
	  if ((CONN)->is_health_check) {			\
		  int ___saved_errno = errno;			\
		  openshift_logconn((IDENT),			\
				    (STREAM),			\
				    __FILE__,			\
				    __LINE__,			\
				    __func__,			\
				    (CONN),			\
				    (FD),			\
				    (FMT),			\
				    ##__VA_ARGS__);		\
		  errno = ___saved_errno;			\
	  }							\
  } while (0)

#endif
