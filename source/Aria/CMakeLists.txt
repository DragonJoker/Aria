project( Aria )

set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "" Release )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "" Release )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Aria - Render tests manager" )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	Aria.hpp
	CategoryPanel.hpp
	ConfigurationDialog.hpp
	CountedValue.hpp
	DatabaseTest.hpp
	DiffImage.hpp
	FileSystem.hpp
	GitFileSystemPlugin.hpp
	LayeredPanel.hpp
	MainFrame.hpp
	Prerequisites.hpp
	RendererPage.hpp
	Signal.hpp
	StringUtils.hpp
	TestDatabase.hpp
	TestPanel.hpp
	TestsCounts.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	Aria.cpp
	CategoryPanel.cpp
	ConfigurationDialog.cpp
	DatabaseTest.cpp
	DiffImage.cpp
	FileSystem.cpp
	GitFileSystemPlugin.cpp
	LayeredPanel.cpp
	MainFrame.cpp
	Prerequisites.cpp
	RendererPage.cpp
	StringUtils.cpp
	TestDatabase.cpp
	TestPanel.cpp
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
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
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
	Model/DataViewTestStatusRenderer.hpp
	Model/ModelPrerequisites.hpp
	Model/TreeModel.hpp
	Model/TreeModelNode.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	Model/DataViewTestStatusRenderer.cpp
	Model/TreeModel.cpp
	Model/TreeModelNode.cpp
)
source_group( "Header Files\\Model"
	FILES
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
)
source_group( "Source Files\\Model"
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
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${wxWidgets_INCLUDE_DIRS}
		${GTK2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		${wxWidgets_LIBRARIES}
		${GTK2_LIBRARIES}
		${X11_LIBRARIES}
)
if ( UNIX )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${CMAKE_DL_LIBS}
	)
endif ()
target_compile_definitions( ${PROJECT_NAME}
	PRIVATE
		${wxWidgets_DEFINITIONS}
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
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	bin
	${PROJECT_NAME}
)

find_package( Git )

if ( Git_FOUND )
	target_compile_definitions( ${PROJECT_NAME}
		PRIVATE
			ARIA_GitSupport
			ARIA_GitPath="${GIT_EXECUTABLE}"
	)
endif ()
