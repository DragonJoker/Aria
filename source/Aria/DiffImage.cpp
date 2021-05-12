#include "DiffImage.hpp"

#include <iostream>
#include <string>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		wxFileName initialiseDir( wxFileName const & basePath
			, wxString const & name )
		{
			auto result = basePath / wxT( "Compare" ) / name;

			if ( !result.Exists() )
			{
				wxMkDir( result.GetFullPath() );
			}

			return result;
		}

		void processLog( wxFileName const & file
			, wxFileName const & directory
			, bool moveLog )
		{
			auto logFile = file.GetPath() / ( file.GetName() + wxT( ".log" ) );

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

		void moveOutput( wxFileName const & file
			, wxFileName const & directory
			, bool moveLog )
		{
			if ( file.FileExists() )
			{
				wxRenameFile( file.GetFullPath(), ( directory / file.GetFullName() ).GetFullPath() );
				processLog( file, directory, moveLog );
			}
		}
	}

	//*********************************************************************************************

	DiffConfig::DiffConfig( DiffOptions const & options )
	{
		dirs[size_t( DiffResult::eUnprocessed )] = initialiseDir( options.input.GetPath(), wxT( "Unprocessed" ) );
		dirs[size_t( DiffResult::eUnacceptable )] = initialiseDir( options.input.GetPath(), wxT( "Unacceptable" ) );
		dirs[size_t( DiffResult::eAcceptable )] = initialiseDir( options.input.GetPath(), wxT( "Acceptable" ) );
		dirs[size_t( DiffResult::eNegligible )] = initialiseDir( options.input.GetPath(), wxT( "Negligible" ) );

		if ( !options.input.FileExists() )
		{
			wxLogWarning( wxString{} << "Reference image [" << options.input << "] does not exist." );

			for ( auto & output : options.outputs )
			{
				moveOutput( output, dirs[size_t( DiffResult::eUnprocessed )], false );
			}

			throw std::runtime_error{ "Reference image does not exist." };
		}

		reference = wxImage{ options.input.GetFullPath() };
	}

	//*********************************************************************************************

	double compareImages( wxImage const & reference
		, wxImage const & toTest
		, wxImage & diffImg )
	{
		diffImg = wxImage{ toTest.GetWidth(), toTest.GetHeight() };
		diffImg.SetType( wxBitmapType::wxBITMAP_TYPE_BMP );
		struct Pixel
		{
			uint8_t r, g, b;
		};
		auto size = reference.GetHeight() * reference.GetWidth();
		auto srcIt = reinterpret_cast< Pixel const * >( reference.GetData() );
		auto end = srcIt + size;
		auto dstIt = reinterpret_cast< Pixel * >( toTest.GetData() );
		auto diffIt = reinterpret_cast< Pixel * >( diffImg.GetData() );
		uint32_t diff{ 0u };

		while ( srcIt != end )
		{
			auto dr = int16_t( dstIt->r - srcIt->r );
			auto dg = int16_t( dstIt->g - srcIt->g );
			auto db = int16_t( dstIt->b - srcIt->b );

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

		return ( double( diff ) / size );
	}

	DiffResult compareImages( DiffOptions const & options
		, DiffConfig const & config
		, wxFileName const & compFile )
	{
		if ( !compFile.FileExists() )
		{
			wxLogError( wxString{} << "Output image [" << compFile << "] does not exist." );
			processLog( compFile, config.dirs[size_t( DiffResult::eUnacceptable )], true );
			return DiffResult::eUnprocessed;
		}

		wxImage toTest{ compFile.GetFullPath() };
		bool carryOn = config.reference.GetSize() == toTest.GetSize();
		DiffResult result = DiffResult::eUnacceptable;

		if ( !carryOn )
		{
			wxLogError( wxString{} << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." );
		}
		else
		{
			wxImage diffImg;
			auto ratio = compareImages( config.reference
				, toTest
				, diffImg );
			result = ( ratio < options.acceptableThreshold
				? ( ratio < options.negligibleThreshold
					? DiffResult::eNegligible
					: DiffResult::eAcceptable )
				: DiffResult::eUnacceptable );

			if ( result == DiffResult::eUnacceptable )
			{
				wxLogError( wxString{} << "Output image [" << compFile.GetFullName() << "] doesn't match reference image [" << options.input.GetFullName() << "]." );
			}
		}

		moveOutput( compFile, config.dirs[size_t( result )]
			, result == DiffResult::eUnacceptable );
		return result;
	}

	//*********************************************************************************************
}
