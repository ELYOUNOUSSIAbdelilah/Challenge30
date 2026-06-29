cmake_minimum_required(VERSION 3.19)
project(Challenge30 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Multimedia Charts)
qt_standard_project_setup()

qt_add_executable(Challenge30
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasemanager.cpp
    databasemanager.h
    registerdialog.h registerdialog.cpp registerdialog.ui
    dashboardwindow.h dashboardwindow.cpp dashboardwindow.ui
    challengewindow.h challengewindow.cpp challengewindow.ui
    leaderboardwindow.h leaderboardwindow.cpp leaderboardwindow.ui
    statisticswindow.h statisticswindow.cpp statisticswindow.ui
    questionmanagerwindow.h questionmanagerwindow.cpp questionmanagerwindow.ui
    addquestiondialog.h addquestiondialog.cpp addquestiondialog.ui
    resultdialog.h resultdialog.cpp resultdialog.ui
    startchallengedialog.h startchallengedialog.cpp startchallengedialog.ui
    historywindow.h historywindow.cpp historywindow.ui

    resources.qrc
    style.qss
    app.rc
    soundmanager.h soundmanager.cpp
    usermanagerwindow.h usermanagerwindow.cpp usermanagerwindow.ui
    achievementswindow.h achievementswindow.cpp achievementswindow.ui
    mainwindo.h mainwindo.cpp mainwindo.ui
)

target_link_libraries(Challenge30
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
        Qt::Multimedia
        Qt::Charts
)

include(GNUInstallDirs)

install(TARGETS Challenge30
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Challenge30
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
