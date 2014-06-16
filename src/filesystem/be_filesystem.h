#ifndef BE_FILESYSTEM_H_INCLUDED
#define BE_FILESYSTEM_H_INCLUDED

#if defined(WIN32)
#	include <shlobj.h> // for SHGetFolderPath and CSIDL_PERSONAL
#endif

#include <boost/filesystem.hpp>
#include "kernel/be_log.h"
#include "be_rootpaths.h"
#include "be_file.h"

#include <string>

class BeFilesystem
{
	public:
		BeFilesystem();
		~BeFilesystem();

		void createDirs();

		std::string findInData( const std::string& file );
		bool load( BeFile& befile, const std::string& file );
		std::string getPath( const std::string& file );
		void save(const std::string& filename, const std::string& content);
		void save_bz2(const std::string& filename, const std::string& content);
		void rm(const std::string& filename);
		void rename(const std::string& filename, const std::string& filename_new);

		BeRootPaths& getRootPaths() { return berootpaths; }

		std::string getHomedir()
		{
			std::string home;
			#if !defined(WIN32)
				home = getenv("HOME");
				home.append( "/.stuntcoureur/" );
			#else
				char mydoc[MAX_PATH];
				memset(mydoc, 0, sizeof(mydoc));
				SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, mydoc);
				home = std::string(mydoc) + "\\stuntcoureur\\";
			#endif
			return home;
		}
	private:
		static BeFilesystem* _instance;

		BeRootPaths berootpaths;
		BeLogDebug m_logDebug;
};

#endif
