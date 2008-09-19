/*
    This file is a modified copy of kglobal.h from the KDE libraries.

    Copyright (C) 1999 Sirtaj Singh Kanq <taj@kde.org>
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2008 George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef LIBQTWINE_PRIVATE_H
#define LIBQTWINE_PRIVATE_H

#include <QtGlobal>

// we actually can't support qt < 4.4.0, so we don't care for changing Q_BASIC_ATOMIC_INITIALIZER like kde does
#if QT_VERSION < 0x040400
#error "libqtwine requires Qt >= 4.4.0"
#endif

#include <QAtomicPointer>

typedef void (*LibqtwineCleanUpFunction)();

class LibqtwineCleanUpGlobalStatic
{
public:
    LibqtwineCleanUpFunction func;
    inline ~LibqtwineCleanUpGlobalStatic() { func(); }
};

#define LIBQTWINE_GLOBAL_STATIC(TYPE, NAME) LIBQTWINE_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ())

#define LIBQTWINE_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                    \
static QBasicAtomicPointer<TYPE > _libqtwine_static_##NAME = Q_BASIC_ATOMIC_INITIALIZER(0); \
static bool _libqtwine_static_##NAME##_destroyed;                              \
static struct                                                                  \
{                                                                              \
    inline bool isDestroyed() const                                            \
    {                                                                          \
        return _libqtwine_static_##NAME##_destroyed;                           \
    }                                                                          \
    inline bool exists() const                                                 \
    {                                                                          \
        return _libqtwine_static_##NAME != 0;                                  \
    }                                                                          \
    inline operator TYPE*()                                                    \
    {                                                                          \
        return operator->();                                                   \
    }                                                                          \
    inline TYPE *operator->()                                                  \
    {                                                                          \
        if (!_libqtwine_static_##NAME) {                                       \
            if (isDestroyed()) {                                               \
                qFatal("Fatal Error: Accessed global static '%s *%s()' after destruction. " \
                       "Defined at %s:%d", #TYPE, #NAME, __FILE__, __LINE__);  \
            }                                                                  \
            TYPE *x = new TYPE ARGS;                                           \
            if (!_libqtwine_static_##NAME.testAndSetOrdered(0, x)              \
                && _libqtwine_static_##NAME != x ) {                           \
                delete x;                                                      \
            } else {                                                           \
                static LibqtwineCleanUpGlobalStatic cleanUpObject = { destroy }; \
            }                                                                  \
        }                                                                      \
        return _libqtwine_static_##NAME;                                       \
    }                                                                          \
    inline TYPE &operator*()                                                   \
    {                                                                          \
        return *operator->();                                                  \
    }                                                                          \
    static void destroy()                                                      \
    {                                                                          \
        _libqtwine_static_##NAME##_destroyed = true;                           \
        TYPE *x = _libqtwine_static_##NAME;                                    \
        _libqtwine_static_##NAME = 0;                                          \
        delete x;                                                              \
    }                                                                          \
} NAME;

#endif
