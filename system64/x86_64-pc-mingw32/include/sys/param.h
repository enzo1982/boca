/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 */
/*
 * param.h
 *
 * This file is part of the Mingw32 package.
 *
 * Contributors:
 *  Created by Earnie Boyd <earnie@users.sf.net>
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Revision$
 * $Author$
 * $Date$
 *
 */

#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#include <sys/types.h>
#include <limits.h>

#ifndef __STRICT_ANSI__
/* These are useful for cross-compiling */ 
#define BIG_ENDIAN	4321
#define LITTLE_ENDIAN	1234
#define BYTE_ORDER	LITTLE_ENDIAN

#define MAXPATHLEN PATH_MAX

#endif

#endif
