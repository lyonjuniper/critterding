// #include <cstring>
#include "be_file.h"

#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

BeFile::BeFile()
{
}

const bool BeFile::setFilename( const std::string& fullfilename )
{
	size_t pos = fullfilename.find_last_of("/", fullfilename.size());
	if ( pos != std::string::npos )
	{
		m_directory = fullfilename.substr( 0, pos+1 );
		m_filename = fullfilename.substr( pos+1, fullfilename.size() );
		return setFilename( fullfilename.substr( 0, pos+1 ), fullfilename.substr( pos+1, fullfilename.size() ) );
	}
	return false;
}

const bool BeFile::setFilename( const std::string& directory, const std::string& filename )
{
	m_directory = directory;
	m_filename = filename;

	m_fullfilename = m_directory + m_filename;
	return loadFile( );
}

std::string BeFile::getFilename( ) const
{
	return m_filename;
}

std::string BeFile::getFullFilename( ) const
{
	return m_fullfilename;
}

std::string BeFile::getDirectory( ) const
{
	return m_directory;
}

const std::stringstream& BeFile::getContent( )
{
	return m_content;
}

const bool BeFile::loadFile( )
{
	size_t pos = m_fullfilename.find(".bz2");
	if ( pos != std::string::npos )
	{
		std::ifstream fstream( m_fullfilename.c_str(), std::ios::in | std::ios::binary );
		if ( fstream.is_open() )
		{
			if ( fstream.good() )
			{
				boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(fstream);

				boost::iostreams::copy(in, m_content);
				return true;
			}
		}
	}
	else
	{
		std::ifstream fstream( m_fullfilename.c_str(), std::ios::in );
		if ( fstream.is_open() )
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			in.push(fstream);
			boost::iostreams::copy(in, m_content);
			return true;
		}
	}

	return false;
}

bool BeFile::getLine( std::string& line )
{
	if ( !m_content.eof() )
	{
		char str[500000];
		memset(str, 0, 500000);
		
		m_content.getline(str,500000);
		line = str;

		if ( line.size() > 0 )
		{
			if ( line.substr( line.size()-1, 1 ) == "\r" )
				line = line.substr( 0, line.size()-1 );
			if ( line.size() > 0 )
				if ( line.substr( line.size()-1, 1 ) == "\0" )
					line = line.substr( 0, line.size()-1 );
		}
		return true;
	}
	return false;
}

BeFile::~BeFile()
{
}
