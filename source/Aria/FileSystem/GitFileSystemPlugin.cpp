#include "FileSystem/GitFileSystemPlugin.hpp"

#include <wx/sizer.h>

#define ARIA_Git_UseAsync 1

namespace aria
{
	//*********************************************************************************************

	namespace git
	{
		wxString const prefix = _( "Git - " );
#if ARIA_Git_UseAsync
		auto constexpr ExecMode = wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE;
#else
		auto constexpr ExecMode = wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE;
#endif
		static int constexpr TimerWaitSeconds = 60 * 10;
		static uint32_t constexpr MaxModifsToCommit = 50u;

#if ARIA_GitSupport
		static bool isGitRootDir( wxFileName const & curDir )
		{
			return ( curDir.GetPath() / wxFileName{ ".git" } ).DirExists();
		}

		static void getGitRootDirRec( wxFileName const & curDir
			, wxFileName & result )
		{
			if ( curDir.IsOk() )
			{
				if ( isGitRootDir( curDir ) )
				{
					result = curDir;
				}
				else
				{
					getGitRootDirRec( curDir.GetPath(), result );
				}
			}
		}

		static wxFileName getGitRootDir( wxFileName const & curDir )
		{
			wxFileName result;
			getGitRootDirRec( curDir, result );
			return result;
		}
#endif
	}

	//*********************************************************************************************

	GitProcess::GitProcess( wxString const & name
		, Git * git
		, int flags )
		: wxProcess{ flags }
		, m_name{ name }
		, m_git{ git }
	{
	}

	void GitProcess::OnTerminate( int pid, int status )
	{
		wxLogMessage( wxString() << "Git: [" << m_name << "] ended " << pid << "(" << status << ")" );
		auto event = new wxProcessEvent{ m_git->getHandlerID(), pid, status };
		m_git->getParent()->GetEventHandler()->QueueEvent( event );
	}

	using wxAsyncExecuteGitCommandCallback = std::function< void() >;
	using wxAsyncExecuteGitCommand = wxAsyncMethodCallEventFunctor< wxAsyncExecuteGitCommandCallback >;

	//*********************************************************************************************

	Git::Git( wxFrame * parent
		, wxWindowID handlerID
		, FileSystem * fileSystem
		, wxFileName const & curDir
		, std::mutex * mutex )
		: FileSystemPlugin{ mutex }
		, wxEvtHandler{}
		, m_parent{ parent }
		, m_handlerID{ handlerID }
		, m_processes{ getProcesses( this, wxPROCESS_DEFAULT, eRemove + 1 ) }
#if ARIA_GitSupport
		, m_rootGitDir{ git::getGitRootDir( curDir ) }
		, m_gitCommand{ ARIA_GitPath }
		, m_enabled{ git::isGitRootDir( m_rootGitDir ) }
		, m_timer{ new wxTimer{ m_parent, m_handlerID } }
#else
		, m_enabled{ false }
#endif
	{
	}

	void Git::initialise()
	{
		if ( !m_enabled )
		{
			return;
		}

		auto statusBar = m_parent->GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );

		auto size = wxSize( 100, statusBar->GetSize().GetHeight() - 6 );
		m_gitProgress = new wxGauge{ statusBar, wxID_ANY, 100, wxPoint( 410, 3 ), size, wxGA_SMOOTH, wxDefaultValidator };
		m_gitProgress->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_gitProgress->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_gitProgress->SetValue( 0 );
		m_gitProgress->SetMinSize( size );
		m_gitProgress->Hide();
		sizer->Add( m_gitProgress, wxSizerFlags{}.Border( wxLEFT, 10 ).FixedMinSize().ReserveSpaceEvenIfHidden() );

		m_gitText = new wxStaticText{ statusBar, wxID_ANY, _( "Idle" ), wxPoint( 520, 5 ), wxDefaultSize, 0 };
		m_gitText->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_gitText->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_gitText->Hide();
		sizer->Add( m_gitText, wxSizerFlags{}.Border( wxLEFT, 5 ) );

#	if ARIA_Git_UseAsync
		m_parent->Bind( wxEVT_END_PROCESS
			, [this]( wxProcessEvent & evt )
			{
				if ( evt.GetId() == m_handlerID )
				{
					doEnd( evt.GetExitCode() );
				}
				else
				{
					evt.Skip();
				}
			} );
#	endif
		m_parent->Bind( wxEVT_TIMER
			, [this]( wxTimerEvent & evt )
			{
				if ( evt.GetId() == m_handlerID )
				{
					commit( "Auto save" );
				}
				else
				{
					evt.Skip();
				}
			} );
		m_parent->Bind( wxEVT_ASYNC_METHOD_CALL
			, [this]( wxAsyncMethodCallEvent & evt )
			{
				if ( evt.GetId() == m_handlerID )
				{
					evt.Execute();
				}
				else
				{
					evt.Skip();
				}
			} );
		m_timer->Start( git::TimerWaitSeconds * 1000 );

		commit( "Launch" );
	}

	void Git::cleanup()
	{
		if ( !m_enabled )
		{
			return;
		}

		for ( auto & process : m_processes )
		{
			if ( process->GetPid()
				&& wxProcess::Exists( int( process->GetPid() ) ) )
			{
				wxMilliSleep( 1 );
			}
		}
	}

	void Git::step()
	{
		if ( !m_enabled )
		{
			return;
		}

		if ( m_pendingEvents
			&& !m_pendingEvents->empty() )
		{
			wxEvtHandler::ProcessPendingEvents();
		}
	}

	bool Git::moveFile( wxString const & testName
		, wxFileName const & src
		, wxFileName const & dst )
	{
		if ( !m_enabled )
		{
			return true;
		}

		auto relSrc = src;
		auto relDst = dst;

		if ( !relSrc.MakeRelativeTo( m_rootGitDir.GetFullPath() ) )
		{
			wxLogError( wxString() << "Git: " << "Couldn't find relative path from [" << m_rootGitDir << "] to [" << src << "]" );
			return false;
		}

		if ( !relDst.MakeRelativeTo( m_rootGitDir.GetFullPath() ) )
		{
			wxLogError( wxString() << "Git: " << "Couldn't find relative path from [" << m_rootGitDir << "] to [" << dst << "]" );
			return false;
		}

		return doPushLoggedCommand( "test " + testName
			, makeCommand( eMove, { relSrc.GetFullPath(), relDst.GetFullPath() } )
			, eMove );
	}

	bool Git::addFileMod( wxString const & testName
		, wxFileName const & file )
	{
		return doAddFileMod( testName, file, eTouch );
	}

	bool Git::addFile( wxString const & testName
		, wxFileName const & file )
	{
		return doAddFileMod( testName, file, eAdd );
	}

	bool Git::updateFile( wxString const & testName
		, wxFileName const & file )
	{
		return doAddFileMod( testName, file, eUpdate );
	}

	bool Git::removeFile( wxString const & testName
		, wxFileName const & file )
	{
		if ( !m_enabled )
		{
			return true;
		}

		auto relFile = file;

		if ( !relFile.MakeRelativeTo( m_rootGitDir.GetFullPath() ) )
		{
			wxLogError( wxString() << "Git: " << "Couldn't find relative path from [" << m_rootGitDir << "] to [" << file << "]" );
			return false;
		}

		return doPushLoggedCommand( "test " + testName
			, makeCommand( eRemove, { relFile.GetFullPath() } )
			, eRemove );
	}

	bool Git::commit( wxString const & label )
	{
		if ( !m_enabled )
		{
			return true;
		}

		return doPushLoggedCommand( label
			, [this, label]()
			{
				auto command = wxString{} << getCommandName( eCommit ) << " -m\"" << makeCommitEntry( label );

				if ( !m_modifs.empty() )
				{
					command << "\n\n" << makeCommitSubEntries( std::move( m_modifs ) );
				}

				command << "\"";
				return command;
			}
			, eCommit );
	}

	size_t Git::doPushCommand( Command command )
	{
		auto commandType = command.type;
		m_commands.emplace_back( std::move( command ) );

		if ( commandType > eCommit )
		{
			m_modifCommandCount++;
		}
		else if ( commandType == eCommit )
		{
			m_modifCommandCount = 0u;
		}

		if ( m_modifCommandCount >= git::MaxModifsToCommit )
		{
			commit( "Intermediate save" );
		}

		return m_commands.size();
	}

	Git::Command Git::doGetNextCommand()
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		return doGetNextCommandNL();
	}

	Git::Command Git::doGetNextCommandNL()
	{
		return m_commands.front();
	}

	size_t Git::doPopCommand()
	{
		auto lock( makeUniqueLock( *m_mutex ) );
		m_commands.erase( m_commands.begin() );
		return m_commands.size();
	}

	bool Git::doAddFileMod( wxString const & testName
		, wxFileName const & file
		, Git::CommandType commandType )
	{
		if ( !m_enabled )
		{
			return true;
		}

		if ( !file.FileExists() )
		{
			file.Touch();
		}

		auto relFile = file;

		if ( !relFile.MakeRelativeTo( m_rootGitDir.GetFullPath() ) )
		{
			wxLogError( wxString() << "Git: " << "Couldn't find relative path from [" << m_rootGitDir << "] to [" << file << "]" );
			return false;
		}

		return doPushLoggedCommand( "test " + testName
			, makeCommand( commandType, { relFile.GetFullPath() } )
			, commandType );
	}

	void Git::doPushExecuteCommand( size_t count )
	{
		auto event = new wxAsyncExecuteGitCommand{ this
			, [this, count]()
			{
				doExecuteCommand( doGetNextCommand(), count );
			} };
		event->SetId( m_handlerID );
		m_parent->GetEventHandler()->QueueEvent( event );
	}

	bool Git::doPushCommand( CommandType commandType
		, GetCommandCallback getCommand
		, OnEndCallback callback
		, wxProcess * process
		, wxString const & label )
	{
		bool result = true;
		auto count = doPushCommand( { commandType, getCommand, callback, process, label } );

		if ( count == 1u )
		{
			doPushExecuteCommand( count );
		}

		return result;
	}

	bool Git::doPushLoggedCommand( wxString const & testName
		, wxString const & command
		, Git::CommandType commandType )
	{
		return doPushLoggedCommand( testName
			, [command](){ return command; }
			, commandType );
	}

	bool Git::doPushLoggedCommand( wxString const & testName
		, Git::GetCommandCallback getCommand
		, Git::CommandType commandType )
	{
		return doPushCommand( commandType
			, getCommand
			, [this, commandType, testName]( int result )
			{
				if ( result >= 0 )
				{
					if ( commandType )
					{
						doLogModif( { commandType, testName } );
					}
				}
			}
			, m_processes[commandType].get()
			, getProcessName( commandType ) + " " + testName );
	}

	bool Git::doExecuteCommand( Command cmd, size_t count )
	{
		m_gitProgress->SetRange( std::max( m_gitProgress->GetRange(), int( count ) ) );
		m_gitProgress->SetValue( m_gitProgress->GetValue() + 1 );
		m_gitProgress->Show();
		wxString command;
		command << m_gitCommand << " " << cmd.getCommand();
		wxExecuteEnv execEnv;
		execEnv.cwd = m_rootGitDir.GetFullPath();
		auto result = wxExecute( command
			, git::ExecMode
			, cmd.process
			, &execEnv );

		if ( cmd.type > eTouch )
		{
			m_gitText->SetLabel( git::prefix + cmd.label );
			m_gitText->Show();

			if ( cmd.type == eCommit )
			{
				m_timer->Start( git::TimerWaitSeconds * 1000 );
			}
		}

		auto statusBar = m_parent->GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->Layout();
		doRegisterOnEnd( int( result )
			, command
			, cmd.callback );
		return result >= 0;
	}

	void Git::doRegisterOnEnd( int result
		, wxString const & command
		, Git::OnEndCallback callback )
	{
		onEnd = [command, callback]( int res )
		{
			if ( res < 0 )
			{
				wxLogError( wxString() << "Git: " << "Command [" << command << "] failed (" << res << ")." );
			}
			else if ( res > 0 )
			{
				wxLogWarning( wxString() << "Git: " << "Command [" << command << "] successful with warning (" << res << ")." );
			}

			callback( res );
		};
#if !ARIA_Git_UseAsync
		doEnd( result );
#endif
	}

	void Git::doEnd( int result )
	{
		onEnd( result );
		auto count = doPopCommand();

		if ( count )
		{
			doPushExecuteCommand( count );
		}
		else
		{
			m_gitText->Hide();
			m_gitProgress->Hide();
			m_gitProgress->SetValue( 0 );
			m_gitProgress->SetRange( 0 );
			auto statusBar = m_parent->GetStatusBar();
			auto sizer = statusBar->GetSizer();
			assert( sizer != nullptr );
			sizer->Layout();
		}
	}

	void Git::doLogModif( Git::CommandModif const & entry )
	{
		if ( entry.type > eCommit )
		{
			m_modifs.push_back( entry );
		}

		auto formattedEntry = makeCommitSubEntry( entry );
		wxLogMessage( wxString() << "Git: " << formattedEntry );
	}

	wxString Git::makeCommitSubEntries( std::vector< CommandModif > modifs )
	{
		wxString result;

		for ( auto & modif : modifs )
		{
			result += makeCommitSubEntry( modif );
		}

		return result;
	}

	wxString Git::getEntryName( CommandType type )
	{
		switch ( type )
		{
		case eTouch:
			return wxString{};
		case eCommit:
			return "Commit";
		case eAdd:
			return "Added";
		case eUpdate:
			return "Updated";
		case eMove:
			return "Moved";
		case eRemove:
			return "Removed";
		default:
			assert( false
				&& "Git::getEntryName - Unsupported CommandType" );
			return "Unknown";
		}
	}

	wxString Git::makeCommitSubEntry( Git::CommandModif const & entry )
	{
		return makeCommitSubEntry( wxString{} << getEntryName( entry.type ) << " " << entry.test << "." );
	}

	wxString Git::makeCommitSubEntry( wxString const & entry )
	{
		return wxString{} << entry << "\n";
	}

	wxString Git::makeCommitEntry( wxString const & entry )
	{
		return wxString{} << "[Aria] " << entry;
	}

	wxString Git::getProcessName( CommandType type )
	{
		switch ( type )
		{
		case eTouch:
			return "Touch";
		case eCommit:
			return "Commit";
		case eAdd:
			return "Add";
		case eUpdate:
			return "Update";
		case eMove:
			return "Move";
		case eRemove:
			return "Remove";
		default:
			assert( false
				&& "Git::getProcessName - Unsupported CommandType" );
			return "Unknown";
		}
	}

	std::vector< std::unique_ptr< GitProcess > > Git::getProcesses( Git * git
		, int flags
		, int end )
	{
		std::vector< std::unique_ptr< GitProcess > > result;

		for ( int i = 0; i < end; ++i )
		{
			result.push_back( std::make_unique< GitProcess >( getProcessName( CommandType( i ) ), git, flags ) );
		}

		return result;
	}

	//*********************************************************************************************
}
