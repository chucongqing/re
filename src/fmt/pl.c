/**
 * @file pl.c Pointer-length functions
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <ctype.h>
#include <sys/types.h>
#ifdef HAVE_STRINGS_H
#define __EXTENSIONS__ 1
#include <strings.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <re_types.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_fmt.h>


/** Pointer-length NULL initialiser */
const struct pl pl_null = {NULL, 0};


static void pl_alloc_destruct(void *arg)
{
	struct pl *pl = arg;

	mem_deref((void *)pl->p);
}


/**
 * Allocate a pointer-length object from a NULL-terminated string
 *
 * @param str NULL-terminated string
 *
 * @return Allocated Pointer-length object or NULL
 */
struct pl *pl_alloc_str(const char *str)
{
	struct pl *pl;

	if (!str)
		return NULL;

	size_t sz = strlen(str);

	pl = mem_zalloc(sizeof(struct pl), pl_alloc_destruct);
	if (!pl)
		return NULL;

	pl->p = mem_alloc(sz, NULL);
	if (!pl->p) {
		mem_deref(pl);
		return NULL;
	}

	memcpy((void *)pl->p, str, sz);

	pl->l = sz;

	return pl;
}


/**
 * Initialise a pointer-length object from a NULL-terminated string
 *
 * @param pl  Pointer-length object to be initialised
 * @param str NULL-terminated string
 */
void pl_set_str(struct pl *pl, const char *str)
{
	if (!pl || !str)
		return;

	pl->p = str;
	pl->l = strlen(str);
}


/**
 * Initialise a pointer-length object from current position and
 * length of a memory buffer
 *
 * @param pl  Pointer-length object to be initialised
 * @param mb  Memory buffer
 */
void pl_set_mbuf(struct pl *pl, const struct mbuf *mb)
{
	if (!pl || !mb)
		return;

	pl->p = (char *)mbuf_buf(mb);
	pl->l = mbuf_get_left(mb);
}


/**
 * Convert a pointer-length object to an int32_t.
 *
 * @param pl Pointer-length object
 *
 * @return int value
 */
int32_t pl_i32(const struct pl *pl)
{
	int32_t v = 0;
	uint32_t mul = 1;
	const char *p;
	bool neg = false;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {
		const char ch = *--p;

		if ('0' <= ch && ch <= '9') {
			v -= mul * (ch - '0');
			mul *= 10;
		}
		else if (ch == '-' && p == pl->p) {
			neg = true;
			break;
		}
		else if (ch == '+' && p == pl->p) {
			break;
		}
		else {
			return 0;
		}
	}

	if (!neg && v == INT32_MIN)
		return INT32_MIN;

	return neg ? v : -v;
}


/**
 * Convert a pointer-length object to an int64_t.
 *
 * @param pl Pointer-length object
 *
 * @return int value
 */
int64_t pl_i64(const struct pl *pl)
{
	int64_t v = 0;
	uint64_t mul = 1;
	const char *p;
	bool neg = false;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {
		const char ch = *--p;

		if ('0' <= ch && ch <= '9') {
			v -= mul * (ch - '0');
			mul *= 10;
		}
		else if (ch == '-' && p == pl->p) {
			neg = true;
			break;
		}
		else if (ch == '+' && p == pl->p) {
			break;
		}
		else {
			return 0;
		}
	}

	if (!neg && v == INT64_MIN)
		return INT64_MIN;

	return neg ? v : -v;
}


/**
 * Convert a pointer-length object to a numeric 32-bit value
 *
 * @param pl Pointer-length object
 *
 * @return 32-bit value
 */
uint32_t pl_u32(const struct pl *pl)
{
	uint32_t v=0, mul=1;
	const char *p;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {
		const uint8_t c = *--p - '0';
		if (c > 9)
			return 0;
		v += mul * c;
		mul *= 10;
	}

	return v;
}


/**
 * Convert a hex pointer-length object to a numeric 32-bit value
 *
 * @param pl Pointer-length object
 *
 * @return 32-bit value
 */
uint32_t pl_x32(const struct pl *pl)
{
	uint32_t v=0, mul=1;
	const char *p;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {

		const char ch = *--p;
		uint8_t c;

		if ('0' <= ch && ch <= '9')
			c = ch - '0';
		else if ('A' <= ch && ch <= 'F')
			c = ch - 'A' + 10;
		else if ('a' <= ch && ch <= 'f')
			c = ch - 'a' + 10;
		else
			return 0;

		v += mul * c;
		mul *= 16;
	}

	return v;
}


/**
 * Convert a pointer-length object to a numeric 64-bit value
 *
 * @param pl Pointer-length object
 *
 * @return 64-bit value
 */
uint64_t pl_u64(const struct pl *pl)
{
	uint64_t v=0, mul=1;
	const char *p;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {
		const uint8_t c = *--p - '0';
		if (c > 9)
			return 0;
		v += mul * c;
		mul *= 10;
	}

	return v;
}


/**
 * Convert a hex pointer-length object to a numeric 64-bit value
 *
 * @param pl Pointer-length object
 *
 * @return 64-bit value
 */
uint64_t pl_x64(const struct pl *pl)
{
	uint64_t v=0, mul=1;
	const char *p;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];
	while (p > pl->p) {

		const char ch = *--p;
		uint8_t c;

		if ('0' <= ch && ch <= '9')
			c = ch - '0';
		else if ('A' <= ch && ch <= 'F')
			c = ch - 'A' + 10;
		else if ('a' <= ch && ch <= 'f')
			c = ch - 'a' + 10;
		else
			return 0;

		v += mul * c;
		mul *= 16;
	}

	return v;
}


/**
 * Convert a pointer-length object to floating point representation.
 * Both positive and negative numbers are supported, a string with a
 * minus sign ('-') is treated as a negative number.
 *
 * @param pl Pointer-length object
 *
 * @return Double value
 */
double pl_float(const struct pl *pl)
{
	double v=0, mul=1;
	const char *p;
	bool neg = false;

	if (!pl || !pl->p)
		return 0;

	p = &pl->p[pl->l];

	while (p > pl->p) {

		const char ch = *--p;

		if ('0' <= ch && ch <= '9') {
			v += mul * (ch - '0');
			mul *= 10;
		}
		else if (ch == '.') {
			v /= mul;
			mul = 1;
		}
		else if (ch == '-' && p == pl->p) {
			neg = true;
		}
		else {
			return 0;
		}
	}

	return neg ? -v : v;
}


/**
 * Decode a bool from a pointer-length object
 *
 * @param val Pointer to bool for returned value
 * @param pl  Pointer-length object
 *
 * @return int 0 if success, otherwise errorcode
 */

int pl_bool(bool *val, const struct pl *pl)
{
	const char *tval[] = {"1", "true",  "enable",  "yes", "on"};
	const char *fval[] = {"0", "false", "disable", "no",  "off"};
	size_t i;

	if (!val || !pl)
		return EINVAL;

	for (i = 0; i < RE_ARRAY_SIZE(tval); ++i) {
		if (!pl_strcasecmp(pl, tval[i])) {
			*val = true;
			return 0;
		}
	}

	for (i = 0; i < RE_ARRAY_SIZE(fval); ++i) {
		if (!pl_strcasecmp(pl, fval[i])) {
			*val = false;
			return 0;
		}
	}

	return EINVAL;
}


/**
 * Convert an ASCII hex string as a pointer-length object to binary format
 *
 * @param pl  Pointer-length object
 * @param hex Destination binary buffer
 * @param len Length of binary buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int pl_hex(const struct pl *pl, uint8_t *hex, size_t len)
{
	if (!pl_isset(pl) || !hex || (pl->l != (2 * len)))
		return EINVAL;

	for (size_t i = 0; i < pl->l; i += 2) {
		hex[i/2]  = ch_hex(*(pl->p + i)) << 4;
		hex[i/2] += ch_hex(*(pl->p + i +1));
	}

	return 0;
}


/**
 * Check if pointer-length object is set
 *
 * @param pl Pointer-length object
 *
 * @return true if set, false if not set
 */
bool pl_isset(const struct pl *pl)
{
	return pl ? pl->p && pl->l : false;
}


/**
 * Copy a pointer-length object to a NULL-terminated string
 *
 * @param pl   Pointer-length object
 * @param str  Buffer for NULL-terminated string
 * @param size Size of buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int pl_strcpy(const struct pl *pl, char *str, size_t size)
{
	size_t len;

	if (!pl || !pl->p || !str || !size)
		return EINVAL;

	len = min(pl->l, size-1);

	memcpy(str, pl->p, len);
	str[len] = '\0';

	return 0;
}


/**
 * Duplicate a pointer-length object to a NULL-terminated string
 *
 * @param dst Pointer to destination string (set on return)
 * @param src Source pointer-length object
 *
 * @return 0 if success, otherwise errorcode
 */
int pl_strdup(char **dst, const struct pl *src)
{
	char *p;

	if (!dst || !src || !src->p)
		return EINVAL;

	p = mem_alloc(src->l+1, NULL);
	if (!p)
		return ENOMEM;

	memcpy(p, src->p, src->l);
	p[src->l] = '\0';

	*dst = p;

	return 0;
}


/**
 * Duplicate a pointer-length object to a new pointer-length object
 *
 * @param dst Destination pointer-length object (set on return)
 * @param src Source pointer-length object
 *
 * @return 0 if success, otherwise errorcode
 */
int pl_dup(struct pl *dst, const struct pl *src)
{
	char *p;

	if (!dst || !src || !src->p)
		return EINVAL;

	p = mem_alloc(src->l, NULL);
	if (!p)
		return ENOMEM;

	memcpy(p, src->p, src->l);

	dst->p = p;
	dst->l = src->l;

	return 0;
}


/**
 * Compare a pointer-length object with a NULL-terminated string
 * (case-sensitive)
 *
 * @param pl  Pointer-length object
 * @param str NULL-terminated string
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_strcmp(const struct pl *pl, const char *str)
{
	struct pl s;

	if (!pl || !str)
		return EINVAL;

	pl_set_str(&s, str);

	return pl_cmp(pl, &s);
}


/**
 * Compare n characters of a pointer-length object with a NULL-terminated
 * string (case-sensitive)
 *
 * @param pl  Pointer-length object
 * @param str NULL-terminated string
 * @param n   number of characters that should be compared
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_strncmp(const struct pl *pl, const char *str, size_t n)
{
	if (!pl_isset(pl) || !str || !n)
		return EINVAL;

	if (pl->l < n)
		return EINVAL;

	return strncmp(pl->p, str, n) == 0 ? 0 : EINVAL;
}


/**
 * Compare n characters of a pointer-length object with a NULL-terminated
 * string (case-insensitive)
 *
 * @param pl  Pointer-length object
 * @param str NULL-terminated string
 * @param n   number of characters that should be compared
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_strncasecmp(const struct pl *pl, const char *str, size_t n)
{
	if (!pl_isset(pl) || !str || !n)
		return EINVAL;

	if (pl->l < n)
		return EINVAL;
#ifdef WIN32
	return _strnicmp(pl->p, str, n) == 0 ? 0 : EINVAL;
#else
	return strncasecmp(pl->p, str, n) == 0 ? 0 : EINVAL;
#endif
}


/**
 * Compare a pointer-length object with a NULL-terminated string
 * (case-insensitive)
 *
 * @param pl  Pointer-length object
 * @param str NULL-terminated string
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_strcasecmp(const struct pl *pl, const char *str)
{
	struct pl s;

	if (!pl || !str)
		return EINVAL;

	pl_set_str(&s, str);

	return pl_casecmp(pl, &s);
}


/**
 * Compare two pointer-length objects (case-sensitive)
 *
 * @param pl1  First pointer-length object
 * @param pl2  Second pointer-length object
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_cmp(const struct pl *pl1, const struct pl *pl2)
{
	if (!pl1 || !pl2)
		return EINVAL;

	/* Different length -> no match */
	if (pl1->l != pl2->l)
		return EINVAL;

	/* Zero-length strings are always identical */
	if (pl1->l == 0)
		return 0;

	/*
	 * ~35% speed increase for fmt/pl test
	 */

	/* The two pl's are the same */
	if (pl1 == pl2)
		return 0;

	/* Two different pl's pointing to same string */
	if (pl1->p == pl2->p)
		return 0;

	return 0 == memcmp(pl1->p, pl2->p, pl1->l) ? 0 : EINVAL;
}


#ifndef HAVE_STRINGS_H
static int casecmp(const struct pl *pl, const char *str)
{
	size_t i = 0;

#define LOWER(d) ((d) | 0x20202020)
	const uint32_t *p1 = (uint32_t *)pl->p;
	const uint32_t *p2 = (uint32_t *)str;
	const size_t len = pl->l & ~0x3;

	/* Skip any unaligned pointers */
	if (((size_t)pl->p) & (sizeof(void *) - 1))
		goto next;
	if (((size_t)str) & (sizeof(void *) - 1))
		goto next;

	/* Compare word-wise */
	for (; i<len; i+=4) {
		if (LOWER(*p1++) != LOWER(*p2++))
			return EINVAL;
	}

 next:
	/* Compare byte-wise */
	for (; i<pl->l; i++) {
		if (tolower(pl->p[i]) != tolower(str[i]))
			return EINVAL;
	}

	return 0;
}
#endif


/**
 * Compare two pointer-length objects (case-insensitive)
 *
 * @param pl1  First pointer-length object
 * @param pl2  Second pointer-length object
 *
 * @return 0 if match, otherwise errorcode
 */
int pl_casecmp(const struct pl *pl1, const struct pl *pl2)
{
	if (!pl1 || !pl2)
		return EINVAL;

	/* Different length -> no match */
	if (pl1->l != pl2->l)
		return EINVAL;

	/* Zero-length strings are always identical */
	if (pl1->l == 0)
		return 0;

	/*
	 * ~35% speed increase for fmt/pl test
	 */

	/* The two pl's are the same */
	if (pl1 == pl2)
		return 0;

	/* Two different pl's pointing to same string */
	if (pl1->p == pl2->p)
		return 0;

#ifdef HAVE_STRINGS_H
	return 0 == strncasecmp(pl1->p, pl2->p, pl1->l) ? 0 : EINVAL;
#else
	return casecmp(pl1, pl2->p);
#endif
}


/**
 * Locate character in pointer-length string
 *
 * @param pl  Pointer-length string
 * @param c   Character to locate
 *
 * @return Pointer to first char if found, otherwise NULL
 */
const char *pl_strchr(const struct pl *pl, char c)
{
	const char *p, *end;

	if (!pl)
		return NULL;

	end = pl->p + pl->l;
	for (p = pl->p; p < end; p++) {
		if (*p == c)
			return p;
	}

	return NULL;
}


/**
 * Locate the last occurrence of character in pointer-length string
 *
 * @param pl  Pointer-length string
 * @param c   Character to locate
 *
 * @return Pointer to last char if found, otherwise NULL
 */
const char *pl_strrchr(const struct pl *pl, char c)
{
	const char *p, *end;

	if (!pl_isset(pl))
		return NULL;

	end = pl->p + pl->l - 1;
	for (p = end; p >= pl->p; p--) {
		if (*p == c)
			return p;
	}

	return NULL;
}


/**
 * Locate the first substring in a pointer-length string
 *
 * @param pl  Pointer-length string
 * @param str Substring to locate
 *
 * @return Pointer to first char if substring is found, otherwise NULL
 */
const char *pl_strstr(const struct pl *pl, const char *str)
{
	size_t len = str_len(str);

	/*case pl not set & pl is not long enough*/
	if (!pl_isset(pl) || pl->l < len)
		return NULL;

	/*case str is empty or just '\0'*/
	if (!len)
		return pl->p;

	for (size_t i = 0; i < pl->l; ++i) {
		/*case rest of pl is not long enough*/
		if (pl->l - i < len)
			return NULL;

		if (!memcmp(pl->p + i, str, len))
			return pl->p + i;
	}

	return NULL;
}


/**
 * Trim white space characters at start of pointer-length string
 *
 * @param pl Pointer-length string
 *
 * @return int 0 if success, otherwise errorcode
 */
int pl_ltrim(struct pl *pl)
{
	if (!pl)
		return EINVAL;

	if (!pl_isset(pl))
		return 0;

	size_t i = 0;
	while (i < pl->l && isspace((unsigned char)pl->p[i])) {
		++i;
	}

	if (i == pl->l)
		pl->l = 0;
	else
		pl_advance(pl, i);

	return 0;
}


/**
 * Trim white space characters at end of pointer-length string
 *
 * @param pl Pointer-length string
 *
 * @return int 0 if success, otherwise errorcode
 */
int pl_rtrim(struct pl *pl)
{
	if (!pl)
		return EINVAL;

	if (!pl_isset(pl))
		return 0;

	while (pl->l > 0 && isspace((unsigned char)pl->p[pl->l - 1])) {
		--pl->l;
	}

	return 0;
}


/**
 * Trim a pointer-length string on both ends
 *
 * @param pl Pointer-length string
 *
 * @return int 0 if success, otherwise errorcode
 */
int pl_trim(struct pl *pl)
{
	int err;

	err  = pl_ltrim(pl);
	err |= pl_rtrim(pl);

	return err;
}
