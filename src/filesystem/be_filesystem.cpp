// boost::shared_ptr<BeGraphicsModel> m(new BeGraphicsModel( m_fileSystem.getDataRoot(path) ));
#include "be_filesystem.h"

using namespace boost::filesystem;

#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>


BeFilesystem::BeFilesystem() : m_logDebug("FILE")
{
}

bool BeFilesystem::load( BeFile& befile, const std::string& file )
{
	// search root paths for file
	for ( unsigned int i=0; i < berootpaths.list.size(); i++ )
	{
		std::string fullfilename( berootpaths.list[i] );
		fullfilename.append( file );

		befile.setFilename( fullfilename );

		BE_LOG( "checking " << fullfilename );
// 		std::cerr << "checking " << fullfilename << std::endl;
		
		if ( boost::filesystem::exists( fullfilename ) )
		{
			BE_LOG( "loading " << fullfilename );
			return true;
		}
	}

	BE_LOG( "warning: cannot find file '" << file );
	return false;
}

std::string BeFilesystem::getPath( const std::string& file )
{
	// search root paths for file
	for ( unsigned int i=0; i < berootpaths.list.size(); i++ )
	{
		std::string fullfilename( berootpaths.list[i] );
		fullfilename.append( file );
		
		if ( boost::filesystem::exists( fullfilename ) )
		{
			return fullfilename;
		}
	}

	BE_LOG( "warning: cannot find file '" << file );
	return "";
}

void BeFilesystem::save(const std::string &filename, const std::string& content)
{
	std::fstream file_op(filename.c_str(),std::ios::out);
	if (file_op.is_open())
	{
		if (file_op.good())
		{
			file_op << content;
			file_op.close();
		}
	}
}

void BeFilesystem::save_bz2(const std::string& filename, const std::string& content)
{
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	in.push(boost::iostreams::bzip2_compressor());

	std::istringstream uncompressed_string(content);
	
	if (uncompressed_string.good())
	{
		in.push(uncompressed_string);

		std::ostringstream compressed_string;
		boost::iostreams::copy(in, compressed_string);

		if (compressed_string.good())
		{
			const std::string& newfilename( filename+".bz2" );
			std::fstream file_op(newfilename.c_str(),std::ios::out | std::ios::binary);

			if (file_op.is_open())
			{
				if (file_op.good())
				{
					file_op << compressed_string.str();
				}
				file_op.close();
			}
		}
	}
}

void BeFilesystem::rename(const std::string& filename, const std::string& filename_new)
{
	boost::filesystem::rename( filename, filename_new );
	
}

void BeFilesystem::rm(const std::string& filename)
{
	if( remove( filename.c_str() ) != 0 )
		BE_LOG( "error: deleting file " << filename );
// 	std::cerr << "Error deleting file" << std::endl;
// 	else
// 		cerr << "file succesfully deleted" << endl;
}

BeFilesystem::~BeFilesystem()
{
}
