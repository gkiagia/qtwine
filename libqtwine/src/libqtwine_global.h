/***************************************************************************
 *   Copyright (C) 2008 by George Kiagiadakis                              *
 *   gkiagia@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#ifndef LIBQTWINE_GLOBAL_H
#define LIBQTWINE_GLOBAL_H

#if !defined(LIBQTWINE_NO_NAMESPACE) && !defined(LIBQTWINE_NAMESPACE)
# define LIBQTWINE_NAMESPACE QtWine
#endif

#if defined(LIBQTWINE_NAMESPACE)
# define LIBQTWINE_BEGIN_NAMESPACE namespace LIBQTWINE_NAMESPACE {
# define LIBQTWINE_END_NAMESPACE }
# define LIBQTWINE_PREPEND_NAMESPACE(class) LIBQTWINE_NAMESPACE::class
#else
# define LIBQTWINE_BEGIN_NAMESPACE
# define LIBQTWINE_END_NAMESPACE
# define LIBQTWINE_PREPEND_NAMESPACE(class) class
#endif

#if defined(__GNUC__) && __GNUC__ - 0 >= 3
# define LIBQTWINE_ISLIKELY( x )    __builtin_expect(!!(x),1)
# define LIBQTWINE_ISUNLIKELY( x )  __builtin_expect(!!(x),0)
#else
# define LIBQTWINE_ISLIKELY( x )   ( x )
# define LIBQTWINE_ISUNLIKELY( x )  ( x )
#endif


#endif
