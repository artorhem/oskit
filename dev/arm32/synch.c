/*
 * Copyright (c) 1996, 1998, 1999, 2000 University of Utah and the Flux Group.
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

#include <oskit/arm32/proc_reg.h>
#include <oskit/arm32/shark/pic.h>

/*
 * Enable/disable interrupts.
 */
inline void
osenv_intr_enable(void)
{
	enable_interrupts();
}

inline void
osenv_intr_disable(void)
{
	disable_interrupts();
}

/*
 * Return the current interrupt enable flag.
 */
inline int
osenv_intr_enabled(void)
{
	return interrupts_enabled();
}

/*
 * Disable interrupts returning the old value.  Combo of:
 *	save = osenv_intr_enabled();
 *	osenv_intr_disable();
 */
inline int
osenv_intr_save_disable(void)
{
	int enabled;

	if ((enabled = interrupts_enabled()) != 0)
		disable_interrupts();

	return enabled;
}
