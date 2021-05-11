#include <DiffImageLib.hpp>

#include <iostream>
#include <string>

namespace
{
	diffimg::Path operator/( wxString const & lhs, wxString const & rhs )
	{
		return lhs + wxFileName::GetPathSeparator() + rhs;
	}

	diffimg::Path operator/( diffimg::Path const & lhs, wxString const & rhs )
	{
		return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs;
	}

	diffimg::Path operator/( diffimg::Path const & lhs, diffimg::Path const & rhs )
	{
		return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs.GetFullPath();
	}
}

void printUsage()
{
	std::cout << "Diff Image is a tool that allows you to compare images issued from TestLauncher app to a reference image." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "DiffImage {RENDERERS} -f FILE" << std::endl;
	std::cout << "  FILE must be a scene file supported by the tested engine." << std::endl;
	std::cout << "  RENDERERS is a list of renderers for which resulting images will be compared to the reference image." << std::endl;
	std::cout << "            They can be any renderer recognised by the tested engine." << std::endl;
}

bool doParseArgs( int argc
	, char * argv[]
	, diffimg::Options & options )
{
	using StringArray = std::vector< std::string >;
	StringArray args{ argv + 1, argv + argc };

	if ( args.empty() )
	{
		std::cerr << "Missing parameters." << std::endl << std::endl;
		printUsage();
		return false;
	}

	auto it = std::find( args.begin(), args.end(), "-h" );

	if ( it == args.end() )
	{
		it = std::find( args.begin(), args.end(), "--help" );
	}

	if ( it != args.end() )
	{
		args.erase( it );
		printUsage();
		return false;
	}

	it = std::find( args.begin(), args.end(), "-f" );

	if ( std::distance( args.begin(), it ) == args.size() - 1 )
	{
		std::cerr << "-f option is missing file parameter" << std::endl << std::endl;
		printUsage();
		return false;
	}

	auto fileIt = it;
	++fileIt;
	auto file = diffimg::Path{ *fileIt };
	options.input = file.GetPath() / ( file.GetName() + wxT( "_ref.png" ) );
	args.erase( fileIt );
	args.erase( it );

	if ( args.empty() )
	{
		std::cerr << "No renderer was given." << std::endl << std::endl;
		printUsage();
		return false;
	}

	for ( auto & arg : args )
	{
		options.outputs.emplace_back( file.GetPath() / wxT( "Compare" ) / ( file.GetName() + wxT( "_" ) + arg + wxT( ".png" ) ) );
	}

	return true;
}

int main( int argc, char * argv[] )
{
	diffimg::Options options;

	if ( !doParseArgs( argc, argv, options ) )
	{
		return -1;
	}

	try
	{
		diffimg::Config config{ options };
		std::atomic_int failures = 0;

		for ( auto & output : options.outputs )
		{
			auto compare = diffimg::compareImages( options, config, output );
			failures += ( compare == diffimg::DiffResult::eUnacceptable
				? 1
				: 0 );
		}

		return failures;
	}
	catch ( std::exception & exc )
	{
		std::cerr << "Failure " << exc.what() << std::endl;
		return -1;
	}
	catch ( ... )
	{
		std::cerr << "Unknown Failure" << std::endl;
		return -1;
	}
}

//******************************************************************************
