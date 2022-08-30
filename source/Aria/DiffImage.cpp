#include "DiffImage.hpp"

#pragma warning( push )
#pragma warning( disable:4189 )
#pragma warning( disable:4201 )
#pragma warning( disable:4242 )
#pragma warning( disable:4365 )
#pragma warning( disable:4388 )
#pragma warning( disable:4389 )
#pragma warning( disable:4458 )
#pragma warning( disable:4706 )
#pragma warning( disable:4800 )
#pragma warning( disable:5219 )
#pragma warning( disable:5245 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wsource-uses-openmp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-zero"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <flip/FLIP.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop
#pragma warning( pop )

#include <iostream>
#include <string>

namespace aria
{
	//*********************************************************************************************

	namespace diff
	{
		static wxFileName initialiseDir( wxFileName const & basePath
			, wxString const & name )
		{
			auto result = basePath / wxT( "Compare" ) / name;

			if ( !result.Exists() )
			{
				wxMkDir( result.GetFullPath(), 0 );
			}

			return result;
		}

		static void processLog( wxFileName const & file
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

		static void moveOutput( wxFileName const & file
			, wxFileName const & directory
			, bool moveLog )
		{
			if ( file.FileExists() )
			{
				wxRenameFile( file.GetFullPath(), ( directory / file.GetFullName() ).GetFullPath() );
				processLog( file, directory, moveLog );
			}
		}

		struct Pixel
		{
			uint8_t r, g, b;
		};

		template< typename FuncT >
		static wxImage getImageDiffT( wxFileName const & refFile
			, wxFileName const & testFile
			, FuncT func )
		{
			wxImage reference = loadImage( refFile );
			wxImage toTest = loadImage( testFile );

			if ( toTest.GetSize() != reference.GetSize() )
			{
				wxLogError( "GetImagesDiff - Images dimensions don't match: " + testFile.GetFullPath() );
				return wxImage{};
			}

			wxImage diffImg{ toTest.GetWidth(), toTest.GetHeight() };
			diffImg.SetType( wxBitmapType::wxBITMAP_TYPE_BMP );
			auto size = reference.GetHeight() * reference.GetWidth();
			auto srcIt = reinterpret_cast< Pixel const * >( reference.GetData() );
			auto end = srcIt + size;
			auto dstIt = reinterpret_cast< Pixel * >( toTest.GetData() );
			auto diffIt = reinterpret_cast< Pixel * >( diffImg.GetData() );

			while ( srcIt != end )
			{
				auto dr = int16_t( dstIt->r - srcIt->r );
				auto dg = int16_t( dstIt->g - srcIt->g );
				auto db = int16_t( dstIt->b - srcIt->b );
				*diffIt = { func( dr, srcIt->r )
					, func( dg, srcIt->g )
					, func( db, srcIt->b ) };
				++srcIt;
				++dstIt;
				++diffIt;
			}

			return diffImg;
		}

		static wxImage getImageDiffRaw( wxFileName const & refFile
			, wxFileName const & testFile )
		{
			return getImageDiffT( refFile
				, testFile
				, []( int16_t diff, uint8_t src )
				{
					return uint8_t( std::min( 255.0, ( double( diff ) * 4.0 + double( src ) / 4.0 ) / 2.0 ) );
				} );
		}

		static wxImage getImageDiffLog( wxFileName const & refFile
			, wxFileName const & testFile )
		{
			return getImageDiffT( refFile
				, testFile
				, []( int16_t diff, uint8_t src )
				{
					auto ddiff = log2( diff );
					return uint8_t( std::min( 255.0, ( ddiff * 10.0 + double( src ) * 2.0 ) / 5.0 ) );
				} );
		}

		struct FLIPOptions
		{
			float PPD = 0;                          // If PPD==0.0, then it will be computed from the parameters below.
			float monitorDistance = 0.7f;           // Unit: meters.
			float monitorWidth = 0.7f;              // Unit: meters.
			float monitorResolutionX = 3840.0f;     // Unit: pixels.
		} gFLIPOptions;

		// Pixels per degree (PPD).
		static float calculatePPD( const float dist, const float resolutionX, const float monitorWidth )
		{
			return dist * ( resolutionX / monitorWidth ) * ( float( FLIP::PI ) / 180.0f );
		}

		static float fClamp( float value )
		{
			return std::max( 0.0f, std::min( 1.0f, value ) );
		}

		static wxImage convert( FLIP::image< float > & source )
		{
			FLIP::image< FLIP::color3 > magmaMap( FLIP::MapMagma, 256 );
			FLIP::image< FLIP::color3 > rgbResult( source.getWidth(), source.getHeight() );
			rgbResult.copyFloat2Color3( source );
			rgbResult.colorMap( source, magmaMap );
			wxImage result{ source.getWidth(), source.getHeight() };
			result.SetType( wxBitmapType::wxBITMAP_TYPE_BMP );
			auto resIt = reinterpret_cast< Pixel * >( result.GetData() );

			for ( int y = 0; y < rgbResult.getHeight(); y++ )
			{
				for ( int x = 0; x < rgbResult.getWidth(); x++ )
				{
					FLIP::color3 color = rgbResult.get( x, y );
					*resIt = {  uint8_t( 255.0f * fClamp( color.x ) + 0.5f )
						,uint8_t( 255.0f * fClamp( color.y ) + 0.5f )
						,uint8_t( 255.0f * fClamp( color.z ) + 0.5f ) };
					++resIt;
				}
			}

			return result;
		}

		static FLIP::image< float > getFlipDiff( wxFileName const & refFile
			, wxFileName const & testFile )
		{
			FLIP::image< FLIP::color3 > referenceImage( refFile.GetFullPath().ToStdString() );
			FLIP::image< FLIP::color3 > testImage( testFile.GetFullPath().ToStdString() );

			if ( testImage.getWidth() != referenceImage.getWidth()
				|| testImage.getHeight() != referenceImage.getHeight() )
			{
				wxLogError( "CompareImages - Images dimensions don't match: " + testFile.GetFullPath() );
				return FLIP::image< float >{ 1, 1 };
			}

			FLIP::image< float > errorMapFLIP( referenceImage.getWidth(), referenceImage.getHeight() );
			errorMapFLIP.FLIP( referenceImage
				, testImage
				, calculatePPD( gFLIPOptions.monitorDistance, gFLIPOptions.monitorResolutionX, gFLIPOptions.monitorWidth ) );
			return errorMapFLIP;
		}

		static wxImage getImageDiffFlip( wxFileName const & refFile
			, wxFileName const & testFile )
		{
			auto diff = getFlipDiff( refFile, testFile );
			return convert( diff );
		}

		static double compareImages( wxFileName const & refFile
			, wxFileName const & testFile )
		{
			FLIP::image< float > errorMapFLIP = getFlipDiff( refFile, testFile );
			pooling< float > pooledValues;

			for ( int y = 0; y < errorMapFLIP.getHeight(); y++ )
			{
				for ( int x = 0; x < errorMapFLIP.getWidth(); x++ )
				{
					pooledValues.update( uint32_t( x )
						, uint32_t( y )
						, errorMapFLIP.get( x, y ) );
				}
			}

			return pooledValues.getMean();
		}
	}

	//*********************************************************************************************

	DiffConfig::DiffConfig( DiffOptions const & options )
	{
		dirs[size_t( DiffResult::eUnprocessed )] = diff::initialiseDir( options.input.GetPath(), wxT( "Unprocessed" ) );
		dirs[size_t( DiffResult::eUnacceptable )] = diff::initialiseDir( options.input.GetPath(), wxT( "Unacceptable" ) );
		dirs[size_t( DiffResult::eAcceptable )] = diff::initialiseDir( options.input.GetPath(), wxT( "Acceptable" ) );
		dirs[size_t( DiffResult::eNegligible )] = diff::initialiseDir( options.input.GetPath(), wxT( "Negligible" ) );

		if ( !options.input.FileExists() )
		{
			wxLogWarning( wxString{} << "Reference image [" << options.input << "] does not exist." );

			for ( auto & output : options.outputs )
			{
				diff::moveOutput( output, dirs[size_t( DiffResult::eUnprocessed )], false );
			}

			throw std::runtime_error{ "Reference image does not exist." };
		}

		reference = wxImage{ options.input.GetFullPath() };
	}

	//*********************************************************************************************

	wxImage loadImage( wxFileName const & filePath )
	{
		if ( !wxFileExists( filePath.GetFullPath() ) )
		{
			return wxImage{};
		}

		wxImage result{ filePath.GetFullPath() };

		if ( result.IsOk() )
		{
			return result;
		}

		return wxImage{};
	}

	wxImage getImageDiff( DiffMode mode
		, wxFileName const & refFile
		, wxFileName const & testFile )
	{
		wxImage reference = loadImage( refFile );
		wxImage toTest = loadImage( testFile );

		if ( toTest.GetSize() != reference.GetSize() )
		{
			wxLogError( "CompareImages - Images dimensions don't match: " + testFile.GetFullPath() );
			return wxImage{};
		}

		wxImage result;

		switch ( mode )
		{
		case aria::DiffMode::eLogarithmic:
			return diff::getImageDiffLog( refFile, testFile );
		case aria::DiffMode::eFlip:
			return diff::getImageDiffFlip( refFile, testFile );
		default:
			return diff::getImageDiffRaw( refFile, testFile );
		}
	}

	DiffResult compareImages( DiffOptions const & options
		, DiffConfig const & config
		, wxFileName const & compFile )
	{
		if ( !compFile.FileExists() )
		{
			wxLogError( wxString{} << "Output image [" << compFile << "] does not exist." );
			diff::processLog( compFile, config.dirs[size_t( DiffResult::eUnacceptable )], true );
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
			auto ratio = diff::compareImages( options.input, compFile );
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

		diff::moveOutput( compFile, config.dirs[size_t( result )]
			, result == DiffResult::eUnacceptable );
		return result;
	}

	//*********************************************************************************************
}
