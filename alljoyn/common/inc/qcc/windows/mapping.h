/**
 * @file
 *
 * Map API names for Win32
 */

/******************************************************************************
 * Copyright (c) 2010-2011, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#ifndef _TOOLCHAIN_QCC_MAPPING_H
#define _TOOLCHAIN_QCC_MAPPING_H

#include <windows.h>
#include <float.h>
/// @cond ALLJOYN_DEV
/**
 * Map snprintf to _snprintf
 *
 * snprintf does not properly map in windows this is needed to insure calls to
 * snprintf(char *str, size_t size, const char *format, ...) will compile in
 * Windows.
 */
#define snprintf _snprintf

/**
 * Map stroll to _strtoi64
 *
 * stroll does not properly map in windows this is needed to insure calls to
 * strtoll(const char *nptr, char **endptr, int base) will compile in Windows.
 */
#define strtoll _strtoi64

/**
 * Map strtoull to _strtoui64
 *
 * strtoull does not properly map in windows this is needed to insure calls to
 * strtoull(const char *nptr, char **endptr, int base) will compile in Windows.
 */
#define strtoull _strtoui64

/**
 * Map strcasecmp to _stricmp
 *
 * strcasecmp does not properly map in windows this is needed to insure calls to
 * strcasecmp(const char *s1, const char *s2) will compile in windows.
 */
#define strcasecmp _stricmp
/**
 * Map fpclassify to _fpclass
 *
 * fpclassify does not properly map in windows this is needed to insure calls to
 * fpclassify(x) will compile in Windows.
 */
//#define fpclassify _fpclass

//#define FP_NAN (_FPCLASS_SNAN | _FPCLASS_QNAN)
//#define FP_ZERO (_FPCLASS_NZ | _FPCLASS_PZ)
//#define FP_INFINITE (_FPCLASS_NINF | _FPCLASS_PINF)
/// @endcond
#endif
