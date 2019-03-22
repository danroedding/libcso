/*
 * cso - strtools.c
 * ================
 *
 * Developed in 2018 for OctoGate IT Security Systems GmbH.
 *
 * Contact at OctoGate: Frank Menne <frank.menne@hsm.de>
 *
 * Author(s):
 * - Daniel Rodding <daniel@roedding-software.de>
 *
 * Software may be redistributed under the termins of the GNU General
 * Public License (Version 3 as of 29.06.2007).
 */

#include <stdlib.h>
#include <ctype.h>

#include "strtools.h"


int str_contains_intval(const char *str)
{
  if (str==NULL || !*str) return 0;

  if (*str=='-') str++;
  while (*str) {
    if (!isdigit(*str)) return 0;
    str++;
  }

  return 1;
}

