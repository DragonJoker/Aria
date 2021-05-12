if( APPLE )
	set( GTK2_FOUND TRUE )
	set( GTK2_LIBRARIES
		"-framework AppKit"
		"-framework QuartzCore"
		"-framework Metal"
	)
elseif ( WIN32 )
	set( GTK2_FOUND TRUE )
else()
	find_package( GTK2 )
	find_package( X11 )
endif()

if ( MSVC )
	find_package( wxWidgets COMPONENTS core base adv aui stc )
else ()
	find_package( wxWidgets COMPONENTS core base adv aui stc )
endif ()

if ( wxWidgets_FOUND AND GTK2_FOUND )
	add_subdirectory( Aria )
else ()
	if ( NOT wxWidgets_FOUND )
		message( SEND_ERROR "no (missing wxWidgets)" )
	endif ()
	if ( NOT GTK2_FOUND )
		message( SEND_ERROR "no (missing gtk2)" )
	endif ()
endif ()