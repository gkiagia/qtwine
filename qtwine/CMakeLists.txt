PROJECT(qtwine_bin)

IF (CONFIGURE_OUTSIDE_QTWINE_TREE)
    #find KDE4
    FIND_PACKAGE(KDE4 REQUIRED)
    INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
    ADD_DEFINITIONS(${KDE4_DEFINITIONS})

    #find libqtwine using pkg-config
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_SEARCH_MODULE(LIBQTWINE_PKG REQUIRED libqtwine)
    LINK_DIRECTORIES(${LIBQTWINE_PKG_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${LIBQTWINE_PKG_INCLUDE_DIRS})

    # we can't use ${LIBQTWINE_PKG_LIBRARIES} because it expands to the string "qtwine"
    # which is the same as the target, so we use find_library to obtain a full path.
    FIND_LIBRARY(LIBQTWINE qtwine PATHS ${LIBQTWINE_PKG_LIBRARY_DIRS} NO_DEFAULT_PATH)
ELSE (CONFIGURE_OUTSIDE_QTWINE_TREE)
    SET(LIBQTWINE libqtwine) # link against the local "libqtwine" target
ENDIF (CONFIGURE_OUTSIDE_QTWINE_TREE)


SET( QTWINE_SOURCES
    src/main.cpp
    src/qtwinemainwindow.cpp
    src/qtwineapplication.cpp

    src/models/qtwinesqltablemodel.cpp
    src/models/wineinstallationsmodel.cpp
    src/models/wineconfigurationsmodel.cpp
    src/models/shortcutsmodel.cpp

    src/dialogs/editorpagedialog.cpp
    src/dialogs/installationeditor.cpp
    src/dialogs/configurationeditor.cpp
    src/dialogs/programshortcuteditor.cpp
    src/dialogs/regfilemergedialog.cpp
    src/dialogs/launcher.cpp
    src/dialogs/createconfigurationdialog.cpp

    src/assistants/commonpages.cpp
    src/assistants/createinstallationassistant.cpp

    src/widgets/metabar.cpp
    src/widgets/metatextlabel.cpp
    src/widgets/metalistviewwidget.cpp
    src/widgets/iconrequesterbutton.cpp
    src/widgets/winedlloverridesedit.cpp
    src/widgets/winedlloverridesrequester.cpp

    src/utils/urlrequestercapabledelegate.cpp

    src/mainwindowparts/metalistviewpart.cpp
    src/mainwindowparts/wineinstallationslistpart.cpp
    src/mainwindowparts/wineconfigurationslistpart.cpp
    src/mainwindowparts/shortcutslistpart.cpp
)

SET( QTWINE_RC_FILES
    src/qtwineui.rc
    src/mainwindowparts/shortcutslistpart.rc
    src/mainwindowparts/wineconfigurationslistpart.rc
    src/mainwindowparts/wineinstallationslistpart.rc
)

SET( QTWINE_UI_FILES
    forms/behaviourPreferences.ui
    forms/createConfigurationDialog.ui
)

KDE4_ADD_UI_FILES(QTWINE_SOURCES ${QTWINE_UI_FILES})
KDE4_ADD_KCFG_FILES(QTWINE_SOURCES src/qtwinepreferences.kcfgc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS(-DQT_GUI_LIB)

KDE4_ADD_EXECUTABLE(qtwine RUN_UNINSTALLED ${QTWINE_SOURCES})
TARGET_LINK_LIBRARIES(qtwine ${KDE4_KPARTS_LIBS} ${LIBQTWINE} ${QT_QTSQL_LIBRARY})

ADD_SUBDIRECTORY(icons)
INSTALL(TARGETS qtwine DESTINATION ${BIN_INSTALL_DIR} COMPONENT runtime)
INSTALL(FILES ${QTWINE_RC_FILES} DESTINATION ${DATA_INSTALL_DIR}/qtwine COMPONENT runtime)
