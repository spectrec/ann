#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <stddef.h>

typedef void (* panic_t)(const char *fmt, ...)
	__attribute__ ((format(printf, 1, 2)));

extern panic_t panic;

#define assert(expr_) {						\
	if (!!(expr_) == 0 && panic != NULL)			\
		panic("\nAssertation `%s' failed (%s:%d)",	\
		      #expr_, __FILE__, __LINE__);		\
}

#endif
