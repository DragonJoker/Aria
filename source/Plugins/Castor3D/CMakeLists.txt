project( Castor3D )

set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "" Release )
set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "" Release )

set( ${PROJECT_NAME}_DESCRIPTION "AriaCastor3D - Castor3D plugin for Aria" )
set( ${PROJECT_NAME}_VERSION_MAJOR 1 )
set( ${PROJECT_NAME}_VERSION_MINOR 0 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Castor3DPlugin.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Castor3DPlugin.cpp
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
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/EditorModule.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/LanguageInfo.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/SceneFileDialog.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/SceneFileEditor.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StcContext.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StcTextEditor.hpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StyleInfo.hpp
)
set( ${PROJECT_NAME}_FOLDER_SRC_FILES
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/LanguageInfo.cpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/SceneFileDialog.cpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/SceneFileEditor.cpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StcContext.cpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StcTextEditor.cpp
	${BASE_SOURCE_DIR}/source/Plugins/${PROJECT_NAME}/Editor/StyleInfo.cpp
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

if ( WIN32 )
	find_rsc_file( ${PROJECT_NAME}
		plugin
	)
endif ()
add_target_min( ${PROJECT_NAME}
	plugin
)
add_library( aria::Castor3D
	ALIAS
		${PROJECT_NAME}
)
target_add_compilation_flags( ${PROJECT_NAME} )
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries( ${PROJECT_NAME}
	PRIVATE
		aria::aria
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		PREFIX aria
		UNITY_BUILD ${PROJECTS_UNITY_BUILD}
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	${PROJECT_NAME}
	plugin
	${PROJECT_NAME}
)

if ( Aria_BUILD_SETUP )
	cpack_add_component( ${PROJECT_NAME}
		DISPLAY_NAME "${PROJECT_NAME} plugin for Aria"
		DESCRIPTION "${PROJECT_NAME} plugin for Aria."
		INSTALL_TYPES Full
	)
endif ()
