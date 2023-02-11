/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_GitFileSystemPlugin_HPP___
#define ___ARIA_GitFileSystemPlugin_HPP___

#include <AriaLib/FileSystem/FileSystem.hpp>

#include <AriaLib/Database/DatabaseTest.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/process.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	class Git;
	class GitProcess;

	class GitProcess
		: public wxProcess
	{
	public:
		GitProcess( wxString const & name
			, Git * git
			, int flags );
		void OnTerminate( int pid, int status )override;

	private:
		wxString m_name;
		Git * m_git;
	};

	class Git
		: public FileSystemPlugin
		, public wxEvtHandler
	{
	private:
		using OnEndCallback = std::function< void( int ) >;
		using GetCommandCallback = std::function< wxString() >;

		enum CommandType
		{
			eTouch,
			eCommit,
			eAdd,
			eUpdate,
			eMove,
			eRemove,
			eCommandTypeCount,
			eMin = eTouch,
			eMax = eRemove,
		};

		struct CommandModif
		{
			CommandModif( CommandType type
				, wxString const & test )
				: type{ type }
				, test{ test }
			{
			}

			CommandType type;
			wxString test;
		};

		struct Command
		{
			CommandType type;
			GetCommandCallback getCommand;
			OnEndCallback callback;
			wxProcess * process;
			wxString label;
		};

		static wxString getCommandName( CommandType type )
		{
			switch ( type )
			{
			case eTouch:
				return "add";
			case eCommit:
				return "commit";
			case eAdd:
				return "add";
			case eUpdate:
				return "add";
			case eMove:
				return "mv";
			case eRemove:
				return "rm";
			default:
				assert( false
					&& "Git: getCommandName - Unsupported CommandType" );
				return "unknown";
			}
		}

		static wxString makeCommand( CommandType type
			, std::vector< wxString > const & params )
		{
			wxString result;
			result << getCommandName( type );

			for ( auto & param : params )
			{
				result << " " << param;
			}

			return result;
		}

	public:
		Git( wxFrame * parent
			, wxWindowID handlerID
			, FileSystem * fileSystem
			, wxFileName const & curDir
			, std::mutex * mutex );

		void initialise()override;
		void cleanup()override;
		void step();

		bool moveFolder( wxFileName const & base
			, wxString const & oldName
			, wxString const & newName )override;
		bool removeFolder( wxFileName const & base
			, wxString const & name )override;
		bool moveFile( wxString const & testName
			, wxFileName const & src
			, wxFileName const & dst )override;
		bool addFileMod( wxString const & testName
			, wxFileName const & file )override;
		bool addFile( wxString const & testName
			, wxFileName const & file )override;
		bool updateFile( wxString const & testName
			, wxFileName const & file )override;
		bool removeFile( wxString const & testName
			, wxFileName const & file )override;
		bool commit( wxString const & label )override;

		bool isEnabled()const override
		{
			return m_enabled;
		}

		bool isRemoving()const override
		{
			return m_enabled;
		}

		wxWindowID getHandlerID()const
		{
			return m_handlerID;
		}

		wxWindow * getParent()const
		{
			return m_parent;
		}

	private:
		size_t doPushCommand( Command command );
		Command doGetNextCommand();
		Command doGetNextCommandNL();
		size_t doPopCommand();
		void doPushExecuteCommand( size_t count );

		bool doAddFileMod( wxString const & testName
			, wxFileName const & file
			, CommandType commandType );
		bool doPushCommand( CommandType commandType
			, GetCommandCallback getCommand
			, OnEndCallback callback
			, wxProcess * process
			, wxString const & label );
		bool doPushLoggedCommand( wxString const & testName
			, wxString const & command
			, CommandType commandType );
		bool doPushLoggedCommand( wxString const & testName
			, GetCommandCallback getCommand
			, CommandType commandType );

		bool doExecuteCommand( Command cmd, size_t count );
		void doRegisterOnEnd( int result
			, wxString const & command
			, OnEndCallback callback );
		void doEnd( int result );
		void doLogModif( CommandModif const & entry );

		static wxString getEntryName( CommandType type );
		static wxString makeCommitSubEntry( CommandModif const & entry );
		static wxString makeCommitSubEntries( std::vector< CommandModif > modifs );
		static wxString makeCommitSubEntry( wxString const & entry );
		static wxString makeCommitEntry( wxString const & entry );

		static wxString getProcessName( CommandType type );
		static std::vector< std::unique_ptr< GitProcess > > getProcesses( Git * git
			, int flags
			, int end );

	private:
		wxFrame * m_parent{};
		wxStaticText * m_gitText{};
		wxGauge * m_gitProgress{};
		wxWindowID m_handlerID{};
		std::vector< std::unique_ptr< GitProcess > > m_processes;
		wxFileName m_rootGitDir;
		wxFileName m_gitCommand;
		bool m_enabled{};
		std::vector< CommandModif > m_modifs;
		std::vector< Command > m_commands;
		uint32_t m_modifCommandCount{};
		OnEndCallback onEnd;
		wxTimer * m_timer{};
	};
}

#endif
