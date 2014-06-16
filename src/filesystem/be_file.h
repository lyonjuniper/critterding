#ifndef BE_FILEBUFFER_H_INCLUDED
#define BE_FILEBUFFER_H_INCLUDED

// #include <string>
#include <sstream>

class BeFile
{
	public:
		BeFile();
		~BeFile();

		void clear();
		const bool setFilename( const std::string& fullfilename );
		const bool setFilename( const std::string& directory, const std::string& filename );

		std::string getFilename( ) const;
		std::string getFullFilename( ) const;
		std::string getDirectory( ) const;
		const bool loadFile( );
		const std::stringstream& getContent( );
		bool getLine( std::string& line );

	protected:
	private:
		std::string m_filename;
		std::string m_directory;
		std::string m_fullfilename;
		std::stringstream m_content;
};

#endif
