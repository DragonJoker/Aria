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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aria.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/ConfigurationDialog.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/DiffImage.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/MainFrame.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Prerequisites.hpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/RendererPage.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Aria.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/ConfigurationDialog.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/DiffImage.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/MainFrame.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/Prerequisites.cpp
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/RendererPage.cpp
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
	${BASE_SOURCE_DIR}/source/${PROJECT_NAME}/FileSystem/GitFileSystemPlugin.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
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
	PUBLIC
		${BASE_SOURCE_DIR}/include
)
target_include_directories( ${PROJECT_NAME} SYSTEM
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/flip
		${CMAKE_SOURCE_DIR}/source/wxCharts/include
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		wxcharts::wxcharts
		aria::aria
)
if ( UNIX )
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${CMAKE_DL_LIBS}
	)
endif ()
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		UNITY_BUILD ${PROJECTS_UNITY_BUILD}
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
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
