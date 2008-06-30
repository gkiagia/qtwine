SET(QT_USE_QTSQL)
FIND_PACKAGE(KDE4 REQUIRED)
FIND_LIBRARY(LIBQTWINE qtwine)

IF(LIBQTWINE_NOTFOUND)
    MESSAGE(FATAL_ERROR "libqtwine not found")
ENDIF(LIBQTWINE_NOTFOUND)

SET( QTWINE_SOURCES
    src/main.cpp
    src/qtwinemainwindow.cpp
    src/qtwineapplication.cpp

    src/providers/abstractsqltableprovider.cpp
    src/providers/wineinstallationsprovider.cpp
    src/providers/wineconfigurationsprovider.cpp
    src/providers/shortcutsprovider.cpp

    src/dialogs/editorpagedialog.cpp
    src/dialogs/installationeditor.cpp
    src/dialogs/configurationeditor.cpp
    src/dialogs/programshortcuteditor.cpp
    src/dialogs/regfilemergedialog.cpp
    src/dialogs/launcher.cpp

    src/assistants/commonpages.cpp
    src/assistants/createinstallationassistant.cpp

    src/widgets/metabar.cpp
    src/widgets/metatextlabel.cpp
    src/widgets/metalistviewwidget.cpp
    src/widgets/infoseparator.cpp
    src/widgets/iconrequesterbutton.cpp
    src/widgets/winedlloverridesedit.cpp
    src/widgets/winedlloverridesrequester.cpp

    src/utils/urlrequestercapabledelegate.cpp

    src/mainwindowparts/metalistviewpart.cpp
    src/mainwindowparts/wineinstallationslistpart.cpp
    src/mainwindowparts/wineconfigurationslistpart.cpp
    src/mainwindowparts/shortcutslistpart.cpp
)

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS(-DQT_GUI_LIB)

KDE4_ADD_EXECUTABLE(qtwine RUN_UNINSTALLED ${QTWINE_SOURCES})
TARGET_LINK_LIBRARIES(qtwine ${KDE4_KPARTS_LIBS} ${LIBQTWINE} ${QT_QTSQL_LIBRARY})
