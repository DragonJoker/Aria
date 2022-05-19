project( Aria )

option( Aria_USES_GIT "Use git filesystem plugin" OFF )

set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "" Release )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "" Release )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Aria - Render tests manager" )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 0 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Plugin.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Prerequisites.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/Options.hpp
	${BASE_SOURCE_DIR}/include/${PROJECT_NAME}/StringUtils.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aria.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/ConfigurationDialog.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/CountedValue.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/DiffImage.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/MainFrame.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/RendererPage.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Signal.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/TestsCounts.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aria.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/ConfigurationDialog.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/DiffImage.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/MainFrame.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Options.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Plugin.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Prerequisites.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/RendererPage.cpp
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DatabaseTest.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbConnection.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbField.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameter.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameterBinding.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameteredObject.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbParameteredObject.inl
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbResult.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbRow.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbStatement.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbStatement.inl
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbStatementParameter.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbTransaction.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValue.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValue.inl
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValueBase.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuedObject.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuedObjectInfos.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuePolicy.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/DbValuePolicy.inl
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Database/TestDatabase.hpp
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/FileSystem.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/GitFileSystemPlugin.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/FileSystem.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/GitFileSystemPlugin.cpp
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

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/color.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/FLIP.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/image.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/mapMagma.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/mapViridis.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/pooling.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/sharedflip.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/stb_image.h
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/flip/tensor.h
)
source_group( "Header Files\\Flip"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/ModelPrerequisites.hpp
)
source_group( "Header Files\\Model"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
set( ${PROJECT_NAME}_HDR_FILES
	${${PROJECT_NAME}_HDR_FILES}
	${${PROJECT_NAME}_FOLDER_HDR_FILES}
)

set( ${PROJECT_NAME}_FOLDER_HDR_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestsModelPrerequisites.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestStatusRenderer.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestTreeModel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestTreeModelNode.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestStatusRenderer.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestTreeModel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/TestsModel/TestTreeModelNode.cpp
)
source_group( "Header Files\\Model\\TestsModel"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Model\\TestsModel"
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunsModelPrerequisites.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunStatusRenderer.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunTreeModel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunTreeModelNode.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunStatusRenderer.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunTreeModel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Model/RunsModel/RunTreeModelNode.cpp
)
source_group( "Header Files\\Model\\RunsModel"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Model\\RunsModel"
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/CategoryPanel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/LayeredPanel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestPanel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestResultsPanel.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestRunsList.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestStatsPanel.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/CategoryPanel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/LayeredPanel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestPanel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestResultsPanel.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestRunsList.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Panels/TestStatsPanel.cpp
)
source_group( "Header Files\\Panels"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Panels"
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/sqlite/sqlite3.h
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/sqlite/sqlite3.c
)
source_group( "Header Files\\sqlite"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\sqlite"
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
	find_rsc_file( ${PROJECT_NAME} bin )
endif ()
add_target_min(
	${PROJECT_NAME}
	bin
)
target_add_compilation_flags( ${PROJECT_NAME} )
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${BASE_SOURCE_DIR}/include/${PROJECT_NAME}
	PUBLIC
		${BASE_SOURCE_DIR}/include
)
target_include_directories( ${PROJECT_NAME} SYSTEM
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/sqlite
		${CMAKE_CURRENT_SOURCE_DIR}/flip
		${CMAKE_SOURCE_DIR}/source/wxCharts
	INTERFACE
		${wxWidgets_INCLUDE_DIRS}
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		wx::charts
	INTERFACE
		${wxWidgets_LIBRARIES}
)
if ( UNIX )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${CMAKE_DL_LIBS}
	)
endif ()
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
		$<$<CXX_COMPILER_ID:MSVC>:_ENFORCE_MATCHING_ALLOCATORS=0>
		$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:Aria_EXPORTS>
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
		ENABLE_EXPORTS ON
		UNITY_BUILD ${PROJECTS_UNITY_BUILD}
)
set_source_files_properties( sqlite/sqlite3.c
	PROPERTIES
		COMPILE_FLAGS $<$<CXX_COMPILER_ID:MSVC>:/wd4242>
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	bin
	${PROJECT_NAME}
)

if ( Aria_USES_GIT )
	find_package( Git )

	if ( Git_FOUND )
		target_compile_definitions( ${PROJECT_NAME}
			PRIVATE
				ARIA_GitSupport
				ARIA_GitPath="${GIT_EXECUTABLE}"
		)
	endif ()
endif ()

if ( Aria_BUILD_SETUP )
	cpack_add_component( ${PROJECT_NAME}
		DISPLAY_NAME "${PROJECT_NAME} application"
		DESCRIPTION "Render tests appication."
		INSTALL_TYPES Full
	)
endif ()
