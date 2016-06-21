#include "stdlib/stdlib.h"
#include "stdlib/string.h"

int atoi(const char *nptr)
{
	int r = 0, mul = 1;

	for (int len = strlen(nptr)-1; len >= 0; len--) {
		if (nptr[len] < '0' || nptr[len] > '9')
			return 0;

		r += mul * (nptr[len] - '0');
		mul *= 10;
	}

	return r;
}
