cmake_minimum_required (VERSION 3.0)

project      ( ResourceArchiver )

set          ( WARNINGS_AS_ERRORS true )
set          ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../../Sources/CMake/Modules/" ) 

include      ( CMake-common )

find_package( DavaTools     REQUIRED )
find_package( DocDirSetup REQUIRED )
dava_add_definitions ( -DCONSOLE )
find_package( DavaFramework REQUIRED COMPONENTS DAVA_DISABLE_AUTOTESTS )

include_directories (Classes)
include_directories ( ${DAVA_INCLUDE_DIR} ) 

define_source ( SOURCE  "Classes"  )

set( MAC_DISABLE_BUNDLE     true)
set( DISABLE_SOUNDS         true)

setup_main_executable()

set_subsystem_console()