cmake_minimum_required( VERSION 3.15 )
cmake_policy( VERSION 3.15 )

if ( VCPKG_TARGET_TRIPLET )
	set( VCPKG_TOOLCHAIN ON )
endif ()

# Setup lookup for external modules
if ( VCPKG_TOOLCHAIN )
	set( CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/CMake
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
	)
else ()
	set( CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/CMake
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/Traditional
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
	)
endif ()

set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release )

set( CMAKE_TEMPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

project( Aria )

set( VERSION_YEAR 2021 )
set( VERSION_MAJOR 1 )
set( VERSION_MINOR 1 )
set( VERSION_BUILD 0 )
set( MAIN_PROJECT_NAME ${PROJECT_NAME} )

include( Project )
include( CompilerVersion )
include( CompilationFlags )
include( ExternalDependencies )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

add_subdirectory( source )