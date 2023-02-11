#include "FileSystem/FileSystem.hpp"

#include "Database/DatabaseTest.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <filesystem>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	//*********************************************************************************************

	ThreadedFileSystemPlugin::ThreadedFileSystemPlugin( FileSystemPlugin * plugin
		, std::mutex * mutex )
		: m_mutex{ mutex }
		, m_plugin{ plugin }
	{
	}

	void ThreadedFileSystemPlugin::initialise()
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		m_plugin->initialise();
		run();
	}

	void ThreadedFileSystemPlugin::cleanup()
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		stop();
		m_plugin->cleanup();
	}

	bool ThreadedFileSystemPlugin::moveFolder( wxFileName const & base
		, wxString const & oldName
		, wxString const & newName )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->moveFolder( base, oldName, newName );
	}

	bool ThreadedFileSystemPlugin::removeFolder( wxFileName const & base
		, wxString const & name )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->removeFolder( base, name );
	}

	bool ThreadedFileSystemPlugin::moveFile( wxString const & testName
		, wxFileName const & src
		, wxFileName const & dst )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->moveFile( testName, src, dst );
	}

	bool ThreadedFileSystemPlugin::addFileMod( wxString const & testName
		, wxFileName const & file )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->addFileMod( testName, file );
	}

	bool ThreadedFileSystemPlugin::addFile( wxString const & testName
		, wxFileName const & file )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->addFile( testName, file );
	}

	bool ThreadedFileSystemPlugin::updateFile( wxString const & testName
		, wxFileName const & file )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->updateFile( testName, file );
	}

	bool ThreadedFileSystemPlugin::removeFile( wxString const & testName
		, wxFileName const & file )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->removeFile( testName, file );
	}

	bool ThreadedFileSystemPlugin::commit( wxString const & label )
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->commit( label );
	}

	bool ThreadedFileSystemPlugin::isEnabled()const
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->isEnabled();
	}

	bool ThreadedFileSystemPlugin::isRemoving()const
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return m_plugin->isRemoving();
	}

	//*********************************************************************************************
	
	void FileSystem::initialise()
	{
		for ( auto & plugin : m_plugins )
		{
			plugin->initialise();
		}
	}
	
	void FileSystem::cleanup()
	{
		for ( auto & plugin : m_plugins )
		{
			plugin->cleanup();
		}
	}

	bool FileSystem::addFile( wxString const & testName
		, wxFileName const & file )
	{
		auto result = true;

		for ( auto & plugin : m_plugins )
		{
			if ( result )
			{
				result = plugin->addFile( testName, file );
			}
		}

		return result;
	}

	void FileSystem::moveFolder( wxFileName const & base
		, wxString const & oldName
		, wxString const & newName
		, bool gitTracked )
	{
		auto files = listDirectoryFiles( base / oldName, true );
		auto o = ( base / oldName ).GetFullPath();
		auto n = ( base / newName ).GetFullPath();

		for ( auto & file : files )
		{
			auto relSrc = file;

			if ( relSrc.MakeRelativeTo( ( base / oldName ).GetFullPath() ) )
			{
				auto v = relSrc.GetFullName();
				moveFile( file.GetName()
					, base / oldName
					, base / newName
					, relSrc
					, relSrc
					, gitTracked );
			}
		}
	}

	void FileSystem::removeFolder( wxFileName const & base
		, wxString const & name
		, bool gitTracked )
	{
		if ( gitTracked )
		{
			bool removed = false;

			for ( auto & plugin : m_plugins )
			{
				if ( plugin->isRemoving()
					&& !removed )
				{
					plugin->removeFolder( base, name );
					removed = true;
				}
			}

			if ( removed )
			{
				return;
			}
		}

		for ( auto & plugin : m_plugins )
		{
			if ( !plugin->isRemoving() )
			{
				plugin->removeFolder( base, name );
			}
		}

		wxDir::Remove( ( base / name ).GetFullPath() );
	}

	bool FileSystem::updateFile( wxString const & testName
		, wxFileName const & srcFolder
		, wxFileName const & dstFolder
		, wxFileName const & srcFile
		, wxFileName const & dstFile )
	{
		if ( !wxCopyFile( ( srcFolder / srcFile ).GetFullPath()
			, ( dstFolder / dstFile ).GetFullPath()
			, true ) )
		{
			wxLogError( wxString() << "FS: " << "Couldn't copy file [" << ( srcFolder / srcFile ).GetFullPath() << "] to [" << ( dstFolder / dstFile ).GetFullPath() << "]" );
			return false;
		}

		auto result = true;

		for ( auto & plugin : m_plugins )
		{
			if ( result )
			{
				result = plugin->updateFile( testName, dstFolder / dstFile );
			}
		}

		return result;
	}

	void FileSystem::moveFile( wxString const & testName
		, wxFileName const & srcFolder
		, wxFileName const & dstFolder
		, wxFileName const & srcName
		, wxFileName const & dstName
		, bool gitTracked )
	{
		auto src = srcFolder / srcName;
		auto dst = dstFolder / dstName;

		if ( src == dst )
		{
			return;
		}

		if ( src.FileExists() )
		{
			if ( !dst.DirExists() )
			{
				if ( !dst.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
				{
					wxLogError( wxString() << "FS: " << "Couldn't create folder [" << dstFolder.GetPath() << "]" );
					return;
				}
			}

			if ( !doMoveFile( testName, src, dst, gitTracked ) )
			{
				wxLogError( wxString() << "FS: " << "Couldn't copy file [" << src.GetFullPath() << "]" );
				return;
			}
		}
		else
		{
			wxLogError( wxString() << "FS: " << "Couldn't copy file [" << src.GetFullPath() << "], file doesn't exist" );
		}
	}

	void FileSystem::removeFile( wxString const & testName
		, wxFileName const & fileName
		, bool gitTracked )
	{
		if ( gitTracked )
		{
			bool removed = false;

			for ( auto & plugin : m_plugins )
			{
				if ( plugin->isRemoving()
					&& !removed )
				{
					plugin->removeFile( testName, fileName );
					removed = true;
				}
			}

			if ( removed )
			{
				return;
			}
		}

		for ( auto & plugin : m_plugins )
		{
			if ( !plugin->isRemoving() )
			{
				plugin->removeFile( testName, fileName );
			}
		}

		wxRemoveFile( fileName.GetFullPath() );
	}

	bool FileSystem::touch( wxString const & testName
		, wxFileName const & file )
	{
		auto result = true;

		for ( auto & plugin : m_plugins )
		{
			if ( result )
			{
				result = plugin->addFileMod( testName, file );
			}
		}

		return result;
	}

	void FileSystem::touchDb( wxFileName const & file )
	{
		if ( m_touchedDb )
		{
			for ( auto & plugin : m_plugins )
			{
				plugin->addFileMod( "Database", file );
			}
		}
		else
		{
			for ( auto & plugin : m_plugins )
			{
				plugin->updateFile( "Database", file );
			}

			m_touchedDb = true;
		}
	}

	bool FileSystem::commit( wxString const & label )
	{
		m_touchedDb = false;
		auto result = true;

		for ( auto & plugin : m_plugins )
		{
			if ( result )
			{
				result = plugin->commit( label );
			}
		}

		return result;
	}

	void FileSystem::doRegisterPlugin( FileSystemPluginPtr plugin )
	{
		m_plugins.emplace_back( std::move( plugin ) );
	}

	bool FileSystem::doMoveFile( wxString const & testName
		, wxFileName const & src
		, wxFileName const & dst
		, bool gitTracked )
	{
		bool result = true;

		if ( gitTracked )
		{
			bool removed = false;

			for ( auto & plugin : m_plugins )
			{
				if ( plugin->isRemoving()
					&& !removed )
				{
					result = plugin->moveFile( testName, src, dst )
						&& plugin->addFileMod( testName, dst );
					removed = true;
				}
			}

			if ( removed )
			{
				return result;
			}
		}

		for ( auto & plugin : m_plugins )
		{
			if ( !plugin->isRemoving() )
			{
				result = plugin->moveFile( testName, src, dst );
			}
		}

		return wxRenameFile( src.GetFullPath(), dst.GetFullPath() );
	}

	//*********************************************************************************************
}
