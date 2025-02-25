#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>
#include <assert.h>

int u64_to_str_with_sep(uint64_t x, char sep, char* out) {
	if (x < 1000) {
		return sprintf(out, "%li", x);
	}
	
	// x >= 1000
	int c1 = u64_to_str_with_sep(x / 1000, sep, out);
	out[c1] = sep;
	
	uint64_t y = x % 1000;
	assert (0 <= y && y <= 999);
	int c2;
	if (y >= 100) {
		c2 = sprintf(out + c1 + 1, "%li", y);
	} else if (y >= 10) {
		c2 = sprintf(out + c1 + 1, "0%li", y);
	} else {
		c2 = sprintf(out + c1 + 1, "00%li", y);
	}
	assert (c2 == 3);

	return c1 + 4;
}

#endif // _UTILS_
