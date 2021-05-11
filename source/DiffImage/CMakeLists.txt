project( DiffImage )

set( ${PROJECT_NAME}_DESCRIPTION "Diffs two images." )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	0 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	DiffImage.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	DiffImage.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
if ( WIN32 )
	find_rsc_file( ${PROJECT_NAME} bin_dos )
endif ()
add_target_min(
	${PROJECT_NAME}
	bin_dos
)
target_link_libraries( ${PROJECT_NAME} PRIVATE
	aria::DiffImageLib
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)
install_target_ex( ${PROJECT_NAME}
	${PROJECT_NAME}
	bin_dos
	${PROJECT_NAME}
)
