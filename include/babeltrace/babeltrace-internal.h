#ifndef _BABELTRACE_INTERNAL_H
#define _BABELTRACE_INTERNAL_H

/*
 * babeltrace/babeltrace-internal.h
 *
 * Copyright 2012 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <glib.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <babeltrace/compat/string-internal.h>
#include <babeltrace/types.h>

#define PERROR_BUFLEN	200

extern bt_bool babeltrace_verbose, babeltrace_debug;

#define printf_verbose(fmt, args...)					\
	do {								\
		if (babeltrace_verbose)					\
			fprintf(stdout, "[verbose] " fmt, ## args);	\
	} while (0)

#define printf_debug(fmt, args...)					\
	do {								\
		if (babeltrace_debug)					\
			fprintf(stdout, "[debug] " fmt, ## args);	\
	} while (0)

#define _bt_printf(fp, kindstr, fmt, args...)				\
	fprintf(fp, "[%s]%s%s%s: " fmt "\n",				\
		kindstr,						\
		babeltrace_debug ? " \"" : "",				\
		babeltrace_debug ? __func__ : "",			\
		babeltrace_debug ? "\"" : "",				\
		## args)

#define _bt_printfl(fp, kindstr, lineno, fmt, args...)			\
	fprintf(fp, "[%s]%s%s%s at line %u: " fmt "\n",			\
		kindstr,						\
		babeltrace_debug ? " \"" : "",				\
		babeltrace_debug ? __func__ : "",			\
		babeltrace_debug ? "\"" : "",				\
		lineno,							\
		## args)

#define _bt_printfe(fp, kindstr, perrorstr, fmt, args...)		\
	fprintf(fp, "[%s]%s%s%s: %s: " fmt "\n",			\
		kindstr,						\
		babeltrace_debug ? " \"" : "",				\
		babeltrace_debug ? __func__ : "",			\
		babeltrace_debug ? "\"" : "",				\
		perrorstr,						\
		## args)

#define _bt_printfle(fp, kindstr, lineno, perrorstr, fmt, args...)	\
	fprintf(fp, "[%s]%s%s%s at line %u: %s: " fmt "\n",		\
		kindstr,						\
		babeltrace_debug ? " \"" : "",				\
		babeltrace_debug ? __func__ : "",			\
		babeltrace_debug ? "\"" : "",				\
		lineno,							\
		perrorstr,						\
		## args)

#define _bt_printf_perror(fp, fmt, args...)				\
	({								\
		const char *errstr;					\
		errstr = g_strerror(errno);				\
		_bt_printfe(fp, "error", errstr, fmt, ## args);		\
	})

#define _bt_printfl_perror(fp, lineno, fmt, args...)			\
	({								\
		const char *errstr;					\
		errstr = g_strerror(errno);				\
		_bt_printfle(fp, "error", lineno, errstr, fmt, ## args);\
	})

/* printf without lineno information */
#define printf_fatal(fmt, args...)					\
	_bt_printf(stderr, "fatal", fmt, ## args)
#define printf_error(fmt, args...)					\
	_bt_printf(stderr, "error", fmt, ## args)
#define printf_warning(fmt, args...)					\
	_bt_printf(stderr, "warning", fmt, ## args)
#define printf_perror(fmt, args...)					\
	_bt_printf_perror(stderr, fmt, ## args)

/* printf with lineno information */
#define printfl_fatal(lineno, fmt, args...)				\
	_bt_printfl(stderr, "fatal", lineno, fmt, ## args)
#define printfl_error(lineno, fmt, args...)				\
	_bt_printfl(stderr, "error", lineno, fmt, ## args)
#define printfl_warning(lineno, fmt, args...)				\
	_bt_printfl(stderr, "warning", lineno, fmt, ## args)
#define printfl_perror(lineno, fmt, args...)				\
	_bt_printfl_perror(stderr, lineno, fmt, ## args)

/* printf with node lineno information */
#define printfn_fatal(node, fmt, args...)				\
	_bt_printfl(stderr, "fatal", (node)->lineno, fmt, ## args)
#define printfn_error(node, fmt, args...)				\
	_bt_printfl(stderr, "error", (node)->lineno, fmt, ## args)
#define printfn_warning(node, fmt, args...)				\
	_bt_printfl(stderr, "warning", (node)->lineno, fmt, ## args)
#define printfn_perror(node, fmt, args...)				\
	_bt_printfl_perror(stderr, (node)->lineno, fmt, ## args)

/* fprintf with Node lineno information */
#define fprintfn_fatal(fp, node, fmt, args...)				\
	_bt_printfl(fp, "fatal", (node)->lineno, fmt, ## args)
#define fprintfn_error(fp, node, fmt, args...)				\
	_bt_printfl(fp, "error", (node)->lineno, fmt, ## args)
#define fprintfn_warning(fp, node, fmt, args...)			\
	_bt_printfl(fp, "warning", (node)->lineno, fmt, ## args)
#define fprintfn_perror(fp, node, fmt, args...)				\
	_bt_printfl_perror(fp, (node)->lineno, fmt, ## args)

#ifndef likely
# ifdef __GNUC__
#  define likely(x)      __builtin_expect(!!(x), 1)
# else
#  define likely(x)      (!!(x))
# endif
#endif

#ifndef unlikely
# ifdef __GNUC__
#  define unlikely(x)    __builtin_expect(!!(x), 0)
# else
#  define unlikely(x)    (!!(x))
# endif
#endif

#ifndef min
#define min(a, b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef max_t
#define max_t(type, a, b)	\
	((type) (a) > (type) (b) ? (type) (a) : (type) (b))
#endif

/*
 * Memory allocation zeroed
 */
#define zmalloc(x) calloc(1, x)

/*
 * BT_HIDDEN: set the hidden attribute for internal functions
 * On Windows, symbols are local unless explicitly exported,
 * see https://gcc.gnu.org/wiki/Visibility
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#define BT_HIDDEN
#else
#define BT_HIDDEN __attribute__((visibility("hidden")))
#endif

#define __STRINGIFY(x)	#x
#define TOSTRING(x)	__STRINGIFY(x)

#endif
