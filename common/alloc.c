/*
 * Copyright (c) 1999-2005, 2007, 2010-2013
 *	Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

#include <config.h>

#include <sys/types.h>
#include <stdio.h>
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif /* STDC_HEADERS */
#ifdef HAVE_STRING_H
# if defined(HAVE_MEMORY_H) && !defined(STDC_HEADERS)
#  include <memory.h>
# endif
# include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRING_H */
#if defined(HAVE_MALLOC_H) && !defined(STDC_HEADERS)
# include <malloc.h>
#endif /* HAVE_MALLOC_H && !STDC_HEADERS */
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#include "missing.h"
#include "alloc.h"
#include "error.h"
#include "errno.h"

#define DEFAULT_TEXT_DOMAIN	"sudo"
#include "gettext.h"

/*
 * If there is no SIZE_MAX or SIZE_T_MAX we have to assume that size_t
 * could be signed (as it is on SunOS 4.x).  This just means that
 * emalloc2() and erealloc3() cannot allocate huge amounts on such a
 * platform but that is OK since sudo doesn't need to do so anyway.
 */
#ifndef SIZE_MAX
# ifdef SIZE_T_MAX
#  define SIZE_MAX	SIZE_T_MAX
# else
#  define SIZE_MAX	INT_MAX
# endif /* SIZE_T_MAX */
#endif /* SIZE_MAX */

/*
 * emalloc() calls the system malloc(3) and exits with an error if
 * malloc(3) fails.
 */
void *
emalloc(size_t size)
{
    void *ptr;

    if (size == 0)
	fatalx_nodebug(_("internal error, tried to emalloc(0)"));

    if ((ptr = malloc(size)) == NULL)
	fatalx_nodebug(NULL);
    return ptr;
}

/*
 * emalloc2() allocates nmemb * size bytes and exits with an error
 * if overflow would occur or if the system malloc(3) fails.
 */
void *
emalloc2(size_t nmemb, size_t size)
{
    void *ptr;

    if (nmemb == 0 || size == 0)
	fatalx_nodebug(_("internal error, tried to emalloc2(0)"));
    if (nmemb > SIZE_MAX / size)
	fatalx_nodebug(_("internal error, %s overflow"), "emalloc2()");

    size *= nmemb;
    if ((ptr = malloc(size)) == NULL)
	fatalx_nodebug(NULL);
    return ptr;
}

/*
 * ecalloc() allocates nmemb * size bytes and exits with an error
 * if overflow would occur or if the system malloc(3) fails.
 * On success, the allocated space is zero-filled.
 */
void *
ecalloc(size_t nmemb, size_t size)
{
    void *ptr;

    if (nmemb == 0 || size == 0)
	fatalx_nodebug(_("internal error, tried to ecalloc(0)"));
    if (nmemb != 1) {
	if (nmemb > SIZE_MAX / size)
	    fatalx_nodebug(_("internal error, %s overflow"), "ecalloc()");
	size *= nmemb;
    }
    if ((ptr = malloc(size)) == NULL)
	fatalx_nodebug(NULL);
    memset(ptr, 0, size);
    return ptr;
}

/*
 * erealloc() calls the system realloc(3) and exits with an error if
 * realloc(3) fails.  You can call erealloc() with a NULL pointer even
 * if the system realloc(3) does not support this.
 */
void *
erealloc(void *ptr, size_t size)
{

    if (size == 0)
	fatalx_nodebug(_("internal error, tried to erealloc(0)"));

    ptr = ptr ? realloc(ptr, size) : malloc(size);
    if (ptr == NULL)
	fatalx_nodebug(NULL);
    return ptr;
}

/*
 * erealloc3() realloc(3)s nmemb * size bytes and exits with an error
 * if overflow would occur or if the system malloc(3)/realloc(3) fails.
 * You can call erealloc() with a NULL pointer even if the system realloc(3)
 * does not support this.
 */
void *
erealloc3(void *ptr, size_t nmemb, size_t size)
{

    if (nmemb == 0 || size == 0)
	fatalx_nodebug(_("internal error, tried to erealloc3(0)"));
    if (nmemb > SIZE_MAX / size)
	fatalx_nodebug(_("internal error, %s overflow"), "erealloc3()");

    size *= nmemb;
    ptr = ptr ? realloc(ptr, size) : malloc(size);
    if (ptr == NULL)
	fatalx_nodebug(NULL);
    return ptr;
}

#ifdef notyet
/*
 * erecalloc() realloc(3)s nmemb * msize bytes and exits with an error
 * if overflow would occur or if the system malloc(3)/realloc(3) fails.
 * On success, the new space is zero-filled.  You can call ereallocz()
 * with a NULL pointer even if the system realloc(3) does not support this.
 */
void *
erecalloc(void *ptr, size_t onmemb, size_t nmemb, size_t msize)
{
    size_t size;

    if (nmemb == 0 || msize == 0)
	fatalx_nodebug(_("internal error, tried to erecalloc(0)"));
    if (nmemb > SIZE_MAX / msize)
	fatalx_nodebug(_("internal error, %s overflow"), "erecalloc()");

    size = nmemb * msize;
    ptr = ptr ? realloc(ptr, size) : malloc(size);
    if (ptr == NULL)
	fatalx_nodebug(NULL);
    if (nmemb > onmemb) {
	size = (nmemb - onmemb) * msize;
	memset((char *)ptr + (onmemb * msize), 0, size);
    }
    return ptr;
}
#endif

/*
 * estrdup() is like strdup(3) except that it exits with an error if
 * malloc(3) fails.  NOTE: unlike strdup(3), estrdup(NULL) is legal.
 */
char *
estrdup(const char *src)
{
    char *dst = NULL;
    size_t len;

    if (src != NULL) {
	len = strlen(src);
	dst = (char *) emalloc(len + 1);
	(void) memcpy(dst, src, len);
	dst[len] = '\0';
    }
    return dst;
}

/*
 * estrdup() is like strndup(3) except that it exits with an error if
 * malloc(3) fails.  NOTE: unlike strdup(3), estrdup(NULL) is legal.
 */
char *
estrndup(const char *src, size_t maxlen)
{
    char *dst = NULL;
    size_t len = 0;

    if (src != NULL) {
	while (maxlen != 0 && src[len] != '\0') {
	    len++;
	    maxlen--;
	}
	dst = (char *) emalloc(len + 1);
	(void) memcpy(dst, src, len);
	dst[len] = '\0';
    }
    return dst;
}

/*
 * easprintf() calls vasprintf() and exits with an error if vasprintf()
 * returns -1 (out of memory).
 */
int
easprintf(char **ret, const char *fmt, ...)
{
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vasprintf(ret, fmt, ap);
    va_end(ap);

    if (len == -1)
	fatalx_nodebug(NULL);
    return len;
}

/*
 * evasprintf() calls vasprintf() and exits with an error if vasprintf()
 * returns -1 (out of memory).
 */
int
evasprintf(char **ret, const char *format, va_list args)
{
    int len;

    if ((len = vasprintf(ret, format, args)) == -1)
	fatalx_nodebug(NULL);
    return len;
}
