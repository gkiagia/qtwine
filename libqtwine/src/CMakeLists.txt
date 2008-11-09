# set the version of this library.
SET( LIBQTWINE_VERSION_MAJOR 0 )
SET( LIBQTWINE_VERSION_MINOR 0 )
SET( LIBQTWINE_VERSION_PATCH 1 )
SET( LIBQTWINE_API_VERSION ${LIBQTWINE_VERSION_MAJOR} )
SET( LIBQTWINE_VERSION ${LIBQTWINE_VERSION_MAJOR}.${LIBQTWINE_VERSION_MINOR}.${LIBQTWINE_VERSION_PATCH} )

# The source files that will be compiled
SET( LIBQTWINE_SOURCES
    winedlloverrides.cpp
    winedebugoptions.cpp
    wineapplication.cpp
    wineinstallation.cpp
    wineconfiguration.cpp
    wineprocess.cpp
    monitoredprocess.cpp
    winepath.cpp
    regedit.cpp
    wineprefixcreate.cpp
    argumentslist.cpp

    private/wineerrordetector.cpp
    private/processioconnector.cpp
    private/terminal.cpp
    private/helpers.cpp
)

# The public headers
SET( LIBQTWINE_PUBLIC_HEADERS
    libqtwine_export.h
    libqtwine_global.h
    winedlloverrides.h
    winedebugoptions.h
    wineapplication.h
    wineinstallation.h
    wineconfiguration.h
    wineprocess.h
    monitoredprocess.h
    winepath.h
    regedit.h
    wineprefixcreate.h
    argumentslist.h
)

# setup misc includes
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# add build target
KDE4_ADD_LIBRARY(libqtwine SHARED ${LIBQTWINE_SOURCES})
TARGET_LINK_LIBRARIES(libqtwine ${KDE4_KDECORE_LIBS} ${QT_QTNETWORK_LIBRARY})

# set library version
SET_TARGET_PROPERTIES(libqtwine PROPERTIES VERSION ${LIBQTWINE_VERSION} SOVERSION ${LIBQTWINE_API_VERSION} OUTPUT_NAME qtwine)

# configure pkg-config
CONFIGURE_FILE("libqtwine.pc.in" "libqtwine.pc" @ONLY)

# setup the "make install" target
INSTALL(TARGETS libqtwine DESTINATION ${LIB_INSTALL_DIR} COMPONENT runtime)
INSTALL(FILES "${CMAKE_BINARY_DIR}/libqtwine.pc" DESTINATION ${LIB_INSTALL_DIR}/pkgconfig COMPONENT development)
FOREACH(header ${LIBQTWINE_PUBLIC_HEADERS})
        GET_FILENAME_COMPONENT(header_path ${header} PATH)
        INSTALL(FILES ${header} DESTINATION include/qtwine/${header_path} COMPONENT development)
ENDFOREACH(header ${LIBQTWINE_PUBLIC_HEADERS})

