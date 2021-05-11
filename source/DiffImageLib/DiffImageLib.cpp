#include "DiffImageLib.hpp"

#include <iostream>
#include <string>

namespace diffimg
{
	//*********************************************************************************************

	namespace
	{
		Path operator/( wxString const & lhs, wxString const & rhs )
		{
			return lhs + wxFileName::GetPathSeparator() + rhs;
		}

		Path operator/( Path const & lhs, wxString const & rhs )
		{
			return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs;
		}

		Path operator/( Path const & lhs, Path const & rhs )
		{
			return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs.GetFullPath();
		}

		Path initialiseDir( Path const & basePath
			, wxString const & name )
		{
			auto result = basePath / wxT( "Compare" ) / name;

			if ( !result.DirExists() )
			{
				result.Mkdir();
			}

			return result;
		}

		void processLog( Path const & file
			, Path const & directory
			, bool moveLog )
		{
			auto logFile = file.GetPath() / ( file.GetFullName() + wxT( ".log" ) );

			if ( logFile.FileExists() )
			{
				if ( moveLog )
				{
					wxRenameFile( logFile.GetFullPath(), ( directory / logFile.GetFullName() ).GetFullPath() );
				}
				else
				{
					wxRemoveFile( logFile.GetFullPath() );
				}
			}
		}

		void moveOutput( Path const & file
			, Path const & directory
			, bool moveLog )
		{
			if ( file.FileExists() )
			{
				wxRenameFile( file.GetFullPath(), ( directory / directory.GetFullName() ).GetFullPath() );
				processLog( file, directory, moveLog );
			}
		}

		std::ostream & operator<<( std::ostream & stream, wxFileName const & value )
		{
			stream << value.GetFullPath();
			return stream;
		}
	}

	//*********************************************************************************************

	Config::Config( Options const & options )
	{
		dirs[size_t( diffimg::DiffResult::eUnprocessed )] = initialiseDir( options.input.GetPath(), wxT( "Unprocessed" ) );
		dirs[size_t( diffimg::DiffResult::eUnacceptable )] = initialiseDir( options.input.GetPath(), wxT( "Unacceptable" ) );
		dirs[size_t( diffimg::DiffResult::eAcceptable )] = initialiseDir( options.input.GetPath(), wxT( "Acceptable" ) );
		dirs[size_t( diffimg::DiffResult::eNegligible )] = initialiseDir( options.input.GetPath(), wxT( "Negligible" ) );

		if ( !options.input.FileExists() )
		{
			std::cout << "Reference image [" << options.input << "] does not exist." << std::endl << std::endl;

			for ( auto & output : options.outputs )
			{
				moveOutput( output, dirs[size_t( diffimg::DiffResult::eUnprocessed )], false );
			}

			throw std::runtime_error{ "Reference image does not exist." };
		}

		reference = wxImage{ options.input.GetFullPath() };
	}

	//*********************************************************************************************

	DiffResult compareImages( Options const & options
		, Config const & config
		, Path const & compFile )
	{
		if ( !compFile.FileExists() )
		{
			std::cerr << "Output image [" << compFile << "] does not exist." << std::endl;
			processLog( compFile, config.dirs[size_t( diffimg::DiffResult::eUnacceptable )], true );
			return DiffResult::eUnprocessed;
		}

		wxImage toTest{ compFile.GetFullPath() };
		bool carryOn = config.reference.GetSize() == toTest.GetSize();
		DiffResult result = DiffResult::eUnacceptable;

		if ( !carryOn )
		{
			std::cerr << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." << std::endl;
		}
		else
		{
			wxImage diffImg{ toTest.GetWidth(), toTest.GetHeight() };
			diffImg.SetType( wxBitmapType::wxBITMAP_TYPE_BMP );
			struct Pixel
			{
				uint8_t r, g, b;
			};
			auto size = config.reference.GetHeight() * config.reference.GetWidth();
			auto srcIt = reinterpret_cast< Pixel const * >( config.reference.GetData() );
			auto end = srcIt + size;
			auto dstIt = reinterpret_cast< Pixel * >( toTest.GetData() );
			auto diffIt = reinterpret_cast< Pixel * >( diffImg.GetData() );
			uint32_t diff{ 0u };

			while ( srcIt != end )
			{
				int16_t dr = int16_t( dstIt->r - srcIt->r );
				int16_t dg = int16_t( dstIt->g - srcIt->g );
				int16_t db = int16_t( dstIt->b - srcIt->b );

				if ( dr || dg || db )
				{
					++diff;
				}

				*diffIt = { uint8_t( std::min( 255, ( dr * 4 + srcIt->r / 4 ) / 2 ) )
					, uint8_t( std::min( 255, ( dg * 4 + srcIt->g / 4 ) / 2 ) )
					, uint8_t( std::min( 255, ( db * 4 + srcIt->b / 4 ) / 2 ) ) };

				++srcIt;
				++dstIt;
				++diffIt;
			}

			auto ratio = ( double( diff ) / size );
			result = ( ratio < options.acceptableThreshold
				? ( ratio < options.negligibleThreshold
					? DiffResult::eNegligible
					: DiffResult::eAcceptable )
				: DiffResult::eUnacceptable );
			diffImg.SaveFile( ( config.dirs[size_t( result )] / ( compFile.GetName() + wxT( ".diff.png" ) ) ).GetFullPath() );

			if ( result == DiffResult::eUnacceptable )
			{
				std::cerr << "Output image [" << compFile.GetFullName() << "] doesn't match reference image [" << options.input.GetFullName() << "]." << std::endl;
			}
		}

		moveOutput( compFile, config.dirs[size_t( result )]
			, result == diffimg::DiffResult::eUnacceptable );
		return result;
	}

	//*********************************************************************************************
}
