/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_FileSystem_HPP___
#define ___ARIA_FileSystem_HPP___

#include "Aria/Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/event.h>
#include <wx/filename.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#pragma warning( pop )

namespace aria
{
	template< typename MutexT >
	std::unique_lock< MutexT > makeUniqueLock( MutexT & mutex )
	{
		return std::unique_lock< MutexT >( mutex );
	}

	class FileSystemPlugin
	{
	protected:
		FileSystemPlugin( std::mutex * mutex = nullptr )
			: m_mutex{ mutex }
		{
		}

	public:
		Aria_API virtual ~FileSystemPlugin() = default;

		Aria_API virtual void initialise() = 0;
		Aria_API virtual void cleanup() = 0;
		Aria_API virtual bool moveFolder( wxFileName const & base
			, wxString const & oldName
			, wxString const & newName ) = 0;
		Aria_API virtual bool removeFolder( wxFileName const & base
			, wxString const & name ) = 0;
		Aria_API virtual bool moveFile( wxString const & testName
			, wxFileName const & src
			, wxFileName const & dst ) = 0;
		Aria_API virtual bool addFileMod( wxString const & testName
			, wxFileName const & file ) = 0;
		Aria_API virtual bool addFile( wxString const & testName
			, wxFileName const & file ) = 0;
		Aria_API virtual bool updateFile( wxString const & testName
			, wxFileName const & file ) = 0;
		Aria_API virtual bool removeFile( wxString const & testName
			, wxFileName const & file ) = 0;
		Aria_API virtual bool commit( wxString const & label ) = 0;
		Aria_API virtual bool isEnabled()const = 0;
		Aria_API virtual bool isRemoving()const = 0;

	protected:
		std::mutex * m_mutex;
	};
	using FileSystemPluginPtr = std::unique_ptr< FileSystemPlugin >;
	using FileSystemPluginArray = std::vector< FileSystemPluginPtr >;

	class ThreadedFileSystemPlugin
		: public FileSystemPlugin
	{
	protected:
		ThreadedFileSystemPlugin( FileSystemPlugin * plugin
			, std::mutex * mutex );

	public:
		void initialise()override;
		void cleanup()override;
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
		bool isEnabled()const override;
		bool isRemoving()const override;

		virtual void run() = 0;
		virtual void stop() = 0;
		virtual bool isRunning()const = 0;
		virtual bool isStopped()const = 0;

	protected:
		std::mutex * m_mutex;

	private:
		FileSystemPlugin * m_plugin;
	};
	using ThreadedFileSystemPluginPtr = std::unique_ptr< ThreadedFileSystemPlugin >;
	using ThreadedFileSystemPluginArray = std::vector< ThreadedFileSystemPluginPtr >;

	template< typename TypeT >
	struct HolderT
	{
		template< typename ... ParamsT >
		HolderT( ParamsT... params )
			: value{ params... }
		{
		}

		TypeT value;
	};

	template< typename FileSystemPluginT >
	class ThreadedFileSystemPluginT
		: public HolderT< std::mutex >
		, public HolderT< FileSystemPluginT >
		, public ThreadedFileSystemPlugin
	{
	public:
		using MutexHolder = HolderT< std::mutex >;
		using PluginHolder = HolderT< FileSystemPluginT >;

	public:
		template< typename ... ParamsT >
		ThreadedFileSystemPluginT( ParamsT... params )
			: MutexHolder{}
			, PluginHolder{ params ..., &getMutex() }
			, ThreadedFileSystemPlugin{ &getPlugin(), &getMutex() }
		{
		}

		template< typename TypeT >
		TypeT & getMember()
		{
			return HolderT< TypeT >::value;
		}

		template< typename TypeT >
		TypeT const& getMember()const
		{
			return HolderT< TypeT >::value;
		}

		std::mutex const & getMutex()const
		{
			return getMember< std::mutex >();
		}

		std::mutex & getMutex()
		{
			return getMember< std::mutex >();
		}

		FileSystemPluginT const & getPlugin()const
		{
			return getMember< FileSystemPluginT >();
		}

		FileSystemPluginT & getPlugin()
		{
			return getMember< FileSystemPluginT >();
		}

		void run()override
		{
			if ( !isRunning() )
			{
				m_stopped = false;
				m_thread = std::thread{ [this]()
					{
						m_running = true;

						while ( !isStopped() )
						{
							getPlugin().step();
							wxMilliSleep( 5 );
						}

						m_running = false;
				} };
			}
		}

		void stop()override
		{
			if ( isRunning() )
			{
				m_stopped = true;
				m_thread.join();
			}
		}

		bool isRunning()const override
		{
			return m_running;
		}

		bool isStopped()const override
		{
			return m_stopped;
		}

	private:
		std::atomic_bool m_running{ false };
		std::atomic_bool m_stopped{ false };
		std::thread m_thread;
	};
	template< typename FileSystemPluginT >
	using ThreadedFileSystemPluginPtrT = std::unique_ptr< ThreadedFileSystemPluginT< FileSystemPluginT > >;

	class FileSystem
	{
	public:
		Aria_API void initialise();
		Aria_API void cleanup();

		template< typename PluginT, typename ... ParamsT >
		void registerPlugin( ParamsT... params )
		{
			doRegisterPlugin( std::make_unique< PluginT >( params... ) );
		}
		
		template< typename PluginT, typename ... ParamsT >
		void registerThreadedPlugin( ParamsT... params )
		{
			doRegisterPlugin( std::make_unique< ThreadedFileSystemPluginT< PluginT > >( params... ) );
		}

		Aria_API bool addFile( wxString const & testName
			, wxFileName const & file );
		Aria_API void moveFolder( wxFileName const & base
			, wxString const & oldName
			, wxString const & newName
			, bool gitTracked );
		Aria_API void removeFolder( wxFileName const & base
			, wxString const & name
			, bool gitTracked );
		Aria_API bool updateFile( wxString const & testName
			, wxFileName const & srcFolder
			, wxFileName const & dstFolder
			, wxFileName const & srcFile
			, wxFileName const & dstFile );
		Aria_API void moveFile( wxString const & testName
			, wxFileName const & srcFolder
			, wxFileName const & dstFolder
			, wxFileName const & srcName
			, wxFileName const & dstName
			, bool gitTracked );
		Aria_API void removeFile( wxString const & testName
			, wxFileName const & fileName
			, bool gitTracked );
		Aria_API bool touch( wxString const & testName
			, wxFileName const & file );
		Aria_API void touchDb( wxFileName const & file );
		Aria_API bool commit( wxString const & label );

	private:
		void doRegisterPlugin( FileSystemPluginPtr plugin );

		bool doMoveFile( wxString const & testName
			, wxFileName const & src
			, wxFileName const & dst
			, bool gitTracked );

	private:
		FileSystemPluginArray m_plugins;
		bool m_touchedDb;
	};
}

#endif
