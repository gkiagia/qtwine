#ifndef LIBQTWINE_EXPORT
# if defined(__GNUC__)
#  define LIBQTWINE_NO_EXPORT __attribute__ ((visibility("hidden")))
#  define LIBQTWINE_EXPORT __attribute__ ((visibility("default")))
# else
#  define LIBQTWINE_NO_EXPORT
#  define LIBQTWINE_EXPORT
# endif
#endif
