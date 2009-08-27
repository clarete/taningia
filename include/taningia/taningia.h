/*
 * Copyright (C) 2009 Lincoln de Sousa <lincoln@minaslivre.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _TANINGIA_H_
#define _TANINGIA_H_

/* These macros are param modifier marks, they have no meaning for the
 * C user. They are mainly used by the binding scanner script to
 * provide some more info about function parameters. */

/* Used for output params */
#define _out_

/* Used for optional parameters  */
#define _optional_

/* Used to receive a string size. The parameter right before the one
 * marked with this must be a string */
#define _len_

/* Mark parameters that can receive NULL values. Different from _out_
 * parameters because, it is still required, but can receive a NULL
 * value.
 */
#define _nullable_

#endif /* _TANINGIA_H_ */
