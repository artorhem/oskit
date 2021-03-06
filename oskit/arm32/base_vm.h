/*
 * Copyright (c) 1999 University of Utah and the Flux Group.
 * All rights reserved.
 * 
 * This file is part of the Flux OSKit.  The OSKit is free software, also known
 * as "open source;" you can redistribute it and/or modify it under the terms
 * of the GNU General Public License (GPL), version 2, as published by the Free
 * Software Foundation (FSF).  To explore alternate licensing terms, contact
 * the University of Utah at csl-dist@cs.utah.edu or +1-801-585-3271.
 * 
 * The OSKit is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GPL for more details.  You should have
 * received a copy of the GPL along with the OSKit; see the file COPYING.  If
 * not, write to the FSF, 59 Temple Place #330, Boston, MA 02111-1307, USA.
 */

/*
 * This stuff is implementation dependent.
 */
#ifndef	_OSKIT_ARM32_BASE_VM_H_
#define _OSKIT_ARM32_BASE_VM_H_

#include <oskit/types.h>

#ifdef OSKIT_ARM32_SHARK
#include <oskit/arm32/shark/base_vm.h>
#else
#warning "Need to define a proper base_vm.h"
#endif

/*
 * All implementations need to define this somehow. This cleans the entire
 * cache. It does *not* invalidate the cache.
 */
void	arm32_cache_clean(void);
extern  oskit_u32_t arm32_cache_clean_addr, arm32_cache_clean_size;

#endif /* _OSKIT_ARM32_BASE_VM_H_ */

