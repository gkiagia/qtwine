#ifndef LIBQTWINE_EXPORT
# include <kdemacros.h>
# if defined(MAKE_QTWINE_LIB)
   /* We are building this library */
#  define LIBQTWINE_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define LIBQTWINE_EXPORT KDE_IMPORT
# endif
#endif
