/* Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.
 */

#define import_spp
#define import_knames
#include <iraf.h>

/* ACLRR -- Clear a block of memory.
 * [Specially optimized for Sun/IRAF].
 */
ACLRR (a, n)
XREAL	*a;
XINT	*n;
{
	bzero ((char *)a, *n * sizeof(*a));
}
