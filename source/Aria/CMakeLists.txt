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
	Aria.hpp
	ConfigurationDialog.hpp
	CountedValue.hpp
	DiffImage.hpp
	MainFrame.hpp
	Prerequisites.hpp
	RendererPage.hpp
	Signal.hpp
	StringUtils.hpp
	TestsCounts.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	Aria.cpp
	ConfigurationDialog.cpp
	DiffImage.cpp
	MainFrame.cpp
	Prerequisites.cpp
	RendererPage.cpp
	StringUtils.cpp
	TestsCounts.cpp
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
	Aui/AuiDockArt.hpp
	Aui/AuiTabArt.hpp
	Aui/AuiToolBarArt.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Aui/AuiDockArt.cpp
	Aui/AuiTabArt.cpp
	Aui/AuiToolBarArt.cpp
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
	Database/DatabaseTest.hpp
	Database/DbConnection.hpp
	Database/DbField.hpp
	Database/DbFieldType.hpp
	Database/DbParameter.hpp
	Database/DbParameterBinding.hpp
	Database/DbParameteredObject.hpp
	Database/DbParameteredObject.inl
	Database/DbParameterType.hpp
	Database/DbPrerequisites.hpp
	Database/DbResult.hpp
	Database/DbRow.hpp
	Database/DbStatement.hpp
	Database/DbStatement.inl
	Database/DbStatementParameter.hpp
	Database/DbTransaction.hpp
	Database/DbValue.hpp
	Database/DbValue.inl
	Database/DbValueBase.hpp
	Database/DbValuedObject.hpp
	Database/DbValuedObjectInfos.hpp
	Database/DbValuePolicy.hpp
	Database/DbValuePolicy.inl
	Database/TestDatabase.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Database/DatabaseTest.cpp
	Database/DbConnection.cpp
	Database/DbField.cpp
	Database/DbParameter.cpp
	Database/DbParameteredObject.cpp
	Database/DbResult.cpp
	Database/DbRow.cpp
	Database/DbStatement.cpp
	Database/DbStatementParameter.cpp
	Database/DbTransaction.cpp
	Database/DbValueBase.cpp
	Database/DbValuedObject.cpp
	Database/DbValuedObjectInfos.cpp
	Database/TestDatabase.cpp
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
	Editor/EditorModule.hpp
	Editor/LanguageInfo.hpp
	Editor/SceneFileDialog.hpp
	Editor/SceneFileEditor.hpp
	Editor/StcContext.hpp
	Editor/StcTextEditor.hpp
	Editor/StyleInfo.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Editor/LanguageInfo.cpp
	Editor/SceneFileDialog.cpp
	Editor/SceneFileEditor.cpp
	Editor/StcContext.cpp
	Editor/StcTextEditor.cpp
	Editor/StyleInfo.cpp
)
source_group( "Header Files\\Editor"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Editor"
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
	FileSystem/FileSystem.hpp
	FileSystem/GitFileSystemPlugin.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	FileSystem/FileSystem.cpp
	FileSystem/GitFileSystemPlugin.cpp
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
	flip/color.h
	flip/FLIP.h
	flip/image.h
	flip/mapMagma.h
	flip/mapViridis.h
	flip/pooling.h
	flip/sharedflip.h
	flip/stb_image.h
	flip/tensor.h
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
	Model/ModelPrerequisites.hpp
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
	Model/TestsModel/TestsModelPrerequisites.hpp
	Model/TestsModel/TestStatusRenderer.hpp
	Model/TestsModel/TestTreeModel.hpp
	Model/TestsModel/TestTreeModelNode.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Model/TestsModel/TestStatusRenderer.cpp
	Model/TestsModel/TestTreeModel.cpp
	Model/TestsModel/TestTreeModelNode.cpp
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
	Model/RunsModel/RunsModelPrerequisites.hpp
	Model/RunsModel/RunStatusRenderer.hpp
	Model/RunsModel/RunTreeModel.hpp
	Model/RunsModel/RunTreeModelNode.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Model/RunsModel/RunStatusRenderer.cpp
	Model/RunsModel/RunTreeModel.cpp
	Model/RunsModel/RunTreeModelNode.cpp
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
	Panels/CategoryPanel.hpp
	Panels/LayeredPanel.hpp
	Panels/TestPanel.hpp
	Panels/TestResultsPanel.hpp
	Panels/TestRunsList.hpp
	Panels/TestStatsPanel.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Panels/CategoryPanel.cpp
	Panels/LayeredPanel.cpp
	Panels/TestPanel.cpp
	Panels/TestResultsPanel.cpp
	Panels/TestRunsList.cpp
	Panels/TestStatsPanel.cpp
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
	sqlite/sqlite3.h
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	sqlite/sqlite3.c
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
)
target_include_directories( ${PROJECT_NAME} SYSTEM
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/sqlite
		${CMAKE_CURRENT_SOURCE_DIR}/flip
		${CMAKE_SOURCE_DIR}/source/wxCharts
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		wx::charts
)
if ( UNIX )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${CMAKE_DL_LIBS}
	)
endif ()
target_compile_definitions( ${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
		$<$<CXX_COMPILER_ID:MSVC>:_ENFORCE_MATCHING_ALLOCATORS=0>
		$<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)
target_compile_options( ${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
		$<$<CXX_COMPILER_ID:MSVC>:/wd4068>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
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
