cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

# Percorso root di Qt (usa la cartella Qt, non la sottocartella lib/cmake)
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/mingw_64")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggiungi include del progetto tramite target (più sicuro che include_directories)
# I file sorgente e header del progetto dovrebbero essere sotto src/ o fileCpp/
# Adatta i percorsi se la tua struttura è diversa.
file(GLOB_RECURSE PROJECT_SOURCES
    fileCpp/*.cpp
    src/*.cpp
    fileCpp/QtUI/*.ui
)

# Trova Qt6 (Widgets e Sql se ti servono)
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# Eseguibile
qt_add_executable(untitled
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Aggiungi include directories al target (non globalmente)
target_include_directories(untitled PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/fileCpp
    ${CMAKE_SOURCE_DIR}/fileCpp/dataora
    ${CMAKE_SOURCE_DIR}/fileCpp/impegno
    ${CMAKE_SOURCE_DIR}/fileCpp/persona
    ${CMAKE_SOURCE_DIR}/fileCpp/Json
    ${CMAKE_SOURCE_DIR}/fileCpp/db
)

# Link a Qt
target_link_libraries(untitled PRIVATE Qt6::Widgets Qt6::Sql)

# Opzioni per Windows bundle
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
