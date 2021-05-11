project( DiffImageLib )

set( ${PROJECT_NAME}_DESCRIPTION "Library to diff two images." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	0 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	DiffImageLib.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	DiffImageLib.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
add_target_min(
	${PROJECT_NAME}
	lib
)
add_library( aria::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		${wxWidgets_INCLUDE_DIRS}
		${GTK2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		${wxWidgets_LIBRARIES}
		${GTK2_LIBRARIES}
		${X11_LIBRARIES}
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		${wxWidgets_DEFINITIONS}
		$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
		$<$<CXX_COMPILER_ID:MSVC>:_ENFORCE_MATCHING_ALLOCATORS=0>
		$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)
target_compile_options( ${PROJECT_NAME}
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
		$<$<CXX_COMPILER_ID:MSVC>:/wd4068>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	lib
	${PROJECT_NAME}
)
