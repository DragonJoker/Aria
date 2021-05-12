set_project( "Aria" )
set_version( "1.1.0" )
set_config("buildir", "build/xmake")

if is_plat( "windows" ) then
	add_syslinks( "DbgHelp", "User32", "Ole32", "Comdlg32", "Shell32", "Gdi32", "Rpcrt4", "Comctl32", "Winspool", "Advapi32", "OleAut32" )
	add_requires( "vcpkg::wxwidgets", "vcpkg::libpng", "vcpkg::tiff", "vcpkg::libjpeg-turbo", "vcpkg::zlib", "vcpkg::liblzma" )
elseif is_plat( "linux" ) then
	add_syslinks( "X11", "dl" )
end


add_rules( "mode.debug", "mode.release" )

target( "Aria" )
	set_kind( "binary" )
	set_languages( "c++17" )
	add_defines("Aria_VERSION_MAJOR=1", "Aria_VERSION_MINOR=1", "Aria_VERSION_BUILD=0")
	set_targetdir( "./binaries/$(mode)/$(arch)/bin" )
	if is_plat( "windows" ) then
		add_packages( "vcpkg::wxwidgets", "vcpkg::libpng", "vcpkg::tiff", "vcpkg::libjpeg-turbo", "vcpkg::zlib", "vcpkg::liblzma" )
	elseif is_plat( "linux" ) then
		add_cxxflags( "$(shell wx-config --cppflags)" )
		set_policy("check.auto_ignore_flags", false)
		add_ldflags( "$(shell wx-config --libs)" )
		add_ldflags( "$(shell wx-config --optional-libs stc)" )
		add_ldflags( "$(shell wx-config --optional-libs aui)" )
	end
	add_includedirs( "source" )
	add_includedirs( "source/Aria" )
	add_files( "source/Aria/*.cpp" )
	add_files( "source/Aria/Aui/*.cpp" )
	add_files( "source/Aria/Database/*.cpp" )
	add_files( "source/Aria/Editor/*.cpp" )
	add_files( "source/Aria/Model/*.cpp" )
	add_files( "source/Aria/sqlite/*.c" )
