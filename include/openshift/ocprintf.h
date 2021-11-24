#ifndef _inc_OPENSHIFTPRINTF_H
#define _inc_OPENSHIFTPRINTF_H

#include <stdio.h>
#include <stdarg.h>

extern void openshift_printf(const char *ident,
			     FILE *stream,
			     const char *filename,
			     int linenumber,
			     const char *funcname,
			     const char *fmt, ...)
	__attribute__((format(printf, 6, 7)));

#define OCPRINTF(IDENT, STREAM, FMT, ...)		\
    do {						\
	    if ((STREAM)) {				\
		    int ___saved_errno = errno;		\
		    openshift_printf((IDENT),		\
				     (STREAM),		\
				     __FILE__,		\
				     __LINE__,		\
				     __func__,		\
				     (FMT),		\
				     ##__VA_ARGS__);	\
		    fflush((STREAM));			\
		    errno = ___saved_errno;		\
	    }						\
    } while (0)

#endif
