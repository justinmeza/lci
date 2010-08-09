/** Data and functions for converting from Unicode normative names to
  * codepoints.
  *
  * \file   unicode.h
  *
  * \author Justin J. Meza
  *
  * \date   2010 */

#ifndef __UNICODE_H__
#define __UNICODE_H__

#include <stdio.h>
#include <string.h>

int binarySearch(const char **, int, int, const char *);
long convertNormativeNameToCodePoint(const char *);
unsigned int convertCodePointToUTF8(long, char *);

#endif /* __UNICODE_H_ */
