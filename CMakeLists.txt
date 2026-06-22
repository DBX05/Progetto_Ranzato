cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

# Percorso root di Qt (adatta se usi versione diversa)
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sorgenti del progetto (includi fileCpp, src, auth, utils)
file(GLOB_RECURSE PROJECT_SOURCES
    fileCpp/*.cpp
    src/*.cpp
    fileCpp/auth/*.cpp
    fileCpp/utils/*.cpp
    fileCpp/db_connector/*.cpp
)

# UI files se presenti
file(GLOB_RECURSE PROJECT_UIS
    fileCpp/QtUI/*.ui
)

# Risorse Qt (se hai .qrc)
file(GLOB_RECURSE PROJECT_RCS
    fileCpp/*.qrc
)

# Trova Qt6 (Widgets e Sql)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# Eseguibile
qt_add_executable(untitled
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${PROJECT_UIS}
    ${PROJECT_RCS}
)

# Include directories per-target
target_include_directories(untitled PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/fileCpp
    ${CMAKE_SOURCE_DIR}/fileCpp/dataora
    ${CMAKE_SOURCE_DIR}/fileCpp/impegno
    ${CMAKE_SOURCE_DIR}/fileCpp/persona
    ${CMAKE_SOURCE_DIR}/fileCpp/Json
    ${CMAKE_SOURCE_DIR}/fileCpp/db_connector
    ${CMAKE_SOURCE_DIR}/fileCpp/utils
    ${CMAKE_SOURCE_DIR}/auth
    ${CMAKE_SOURCE_DIR}/fileCpp/QtUI

)

# Link a Qt (Widgets e Sql)
target_link_libraries(untitled PRIVATE Qt6::Widgets Qt6::Sql)

# Windows bundle properties
set_target_properties(untitled PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install rules (opzionale)
include(GNUInstallDirs)
install(TARGETS untitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(untitled)
