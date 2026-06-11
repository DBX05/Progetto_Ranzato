cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

# --- Percorsi Qt per Windows MinGW ---
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/mingw_64/lib/cmake")
set(Qt6_DIR "C:/Qt/6.10.2/mingw_64/lib/cmake/Qt6")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Make project root available for includes (so <fileCpp/...> works)
include_directories(${CMAKE_SOURCE_DIR})

# --- Trova Qt6 ---
find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES untitled_it_IT.ts)

file(GLOB QTUI_SOURCES
    fileCpp/QtUI/*.cpp
    fileCpp/QtUI/*.h
    fileCpp/QtUI/*.ui
    fileCpp/QtUI/*.xml
)

set(PROJECT_SOURCES
    ${QTUI_SOURCES}
    ${TS_FILES}
)

# --- Eseguibile Qt6 ---
qt_add_executable(untitled
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(untitled PRIVATE Qt6::Widgets)

set_target_properties(untitled PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS untitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(untitled)

# ------------------------------------------------------------------
# Additional target: AgendaQtExample (aligned to main project)
# ------------------------------------------------------------------
# Reuse project settings and Qt found above; keep a separate executable target.
file(GLOB SRC src/*.cpp)

add_executable(agenda_qt ${SRC})

qt_add_resources(RESOURCES resources.qrc)

target_sources(agenda_qt PRIVATE ${RESOURCES})

target_link_libraries(agenda_qt PRIVATE Qt6::Widgets)
