add_subdirectory( wxCharts )

if( APPLE )
	set( GTK_FOUND TRUE )
	set( GTK_LIBRARIES
		"-framework AppKit"
		"-framework QuartzCore"
		"-framework Metal"
	)
elseif ( WIN32 )
	set( GTK_FOUND TRUE )
else()
	find_package( GTK3 QUIET )
	find_package( X11 QUIET )
	if ( GTK3_FOUND )
		set( GTK_FOUND ${GTK3_FOUND} )
		set( GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS} )
		set( GTK_LIBRARIES ${GTK3_LIBRARIES} )
	else ()
		set( GTK_FOUND FALSE )
	endif ()
endif()

find_package( wxWidgets CONFIG COMPONENTS core base adv aui stc )

if ( wxWidgets_FOUND AND GTK_FOUND )
	add_subdirectory( Aria )
	add_subdirectory( Plugins )
else ()
	if ( NOT wxWidgets_FOUND )
		message( SEND_ERROR "no (missing wxWidgets)" )
	endif ()
	if ( NOT GTK_FOUND )
		message( SEND_ERROR "no (missing gtk)" )
	endif ()
endif ()
