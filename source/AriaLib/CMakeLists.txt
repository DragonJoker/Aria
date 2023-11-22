project( AriaLib )

option( Aria_USES_GIT "Use git filesystem plugin" OFF )

set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "" Release )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "" Release )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "AriaLib - Render tests library" )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 0 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/BeginExternHeaderGuard.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/CountedValue.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/EndExternHeaderGuard.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Options.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Plugin.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Prerequisites.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Signal.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/StringUtils.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/TestsCounts.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Options.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Plugin.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Prerequisites.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/StringUtils.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/TestsCounts.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Aui/AuiDockArt.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Aui/AuiTabArt.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Aui/AuiToolBarArt.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aui/AuiDockArt.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aui/AuiTabArt.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aui/AuiToolBarArt.cpp
)
source_group( "Header Files\\Aui"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Aui"
	FILES
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbPrerequisites.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbFieldType.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbParameterType.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DatabaseTest.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbConnection.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbField.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbParameter.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbParameterBinding.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbParameteredObject.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbParameteredObject.inl
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbResult.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbRow.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbStatement.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbStatement.inl
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbStatementParameter.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbTransaction.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValue.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValue.inl
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValueBase.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValuedObject.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValuedObjectInfos.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValuePolicy.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/DbValuePolicy.inl
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Database/TestDatabase.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DatabaseTest.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbConnection.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbField.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameter.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameteredObject.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbResult.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbRow.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbStatement.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbStatementParameter.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbTransaction.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValueBase.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuedObject.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuedObjectInfos.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/TestDatabase.cpp
)
source_group( "Header Files\\Database"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Database"
	FILES
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/FileSystem/FileSystem.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/FileSystem.cpp
)
source_group( "Header Files\\FileSystem"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\FileSystem"
	FILES
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
set( ${PROJECT_NAME}_SRC_FILES
	${${PROJECT_NAME}_SRC_FILES}
	${${PROJECT_NAME}_FOLDER_SRC_FILES}
)

if ( WIN32 )
	find_rsc_file( ${PROJECT_NAME} dll )
endif ()
add_target_min(
	${PROJECT_NAME}
	dll
)
add_library( aria::aria
	ALIAS
		${PROJECT_NAME}
)
target_add_compilation_flags( ${PROJECT_NAME} )
aria_release_pdbs( ${PROJECT_NAME} )
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${BASE_SOURCE_DIR}/include/${PROJECT_NAME}
	PUBLIC
		${BASE_SOURCE_DIR}/include
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		${wxWidgets_LIBRARIES}
		${X11_LIBRARIES}
		${GTK_LIBRARIES}
		unofficial::sqlite3::sqlite3
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
		$<$<CXX_COMPILER_ID:MSVC>:_ENFORCE_MATCHING_ALLOCATORS=0>
		$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:AriaLib_EXPORTS>
)
target_compile_options( ${PROJECT_NAME}
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
		$<$<CXX_COMPILER_ID:MSVC>:/wd4068>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		ENABLE_EXPORTS ON
		UNITY_BUILD ${PROJECTS_UNITY_BUILD}
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	${PROJECT_NAME}
	dll
	${PROJECT_NAME}
)

if ( Aria_BUILD_SETUP )
	cpack_add_component( ${PROJECT_NAME}
		DISPLAY_NAME "${PROJECT_NAME} library"
		DESCRIPTION "Render tests library."
		INSTALL_TYPES Full
	)
endif ()
