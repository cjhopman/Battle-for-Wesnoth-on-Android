#ifndef SYS_LOCALE_H
#define SYS_LOCALE_H

#pragma GCC system_header

#include_next <locale.h>
#include <string.h>

static inline char *setlocale_nonullreturn(int category, const char *locale) {
	static char* clocale = "C";
	char* ret = setlocale(category, locale);
	return 
		locale == 0 || (strcmp(locale, "") == 0) || (strcmp(locale, "C") == 0) ? clocale : ret;
}

#define setlocale setlocale_nonullreturn

#endif
