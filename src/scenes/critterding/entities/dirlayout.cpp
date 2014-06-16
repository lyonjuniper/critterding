#ifdef _WIN32
	#include <windows.h>
	#include <shlobj.h>
#endif
#include "dirlayout.h"

Dirlayout* Dirlayout::Instance () 
{
	static Dirlayout t;
	return &t; // _instance isn't needed in this case
}

Dirlayout::Dirlayout()
{
	createDirs();

}

void Dirlayout::createDirs()
{
	homedir.assign(getHomedir());
#if !defined(WIN32)	//On Windows expect empty home dir, why not move this to within getHomeDir() on Linux?
	if ( homedir.empty() ) {
		std::cerr << "environment variable HOME not defined/detected" << std::endl;
		exit(0);
	}
#endif

// #ifndef _WIN32
	progdir = homedir + "/.critterding";
	savedir = progdir + "/save";
	loaddir = progdir + "/load";
	exchangedir = savedir + "/exchange";
	profiledir = progdir + "/profiles";
	
// #else
// 	progdir = homedir;	progdir.append("\\critterding");
// 	savedir = progdir;	savedir.append("\\save");
// 	loaddir = progdir;	loaddir.append("\\load");
// #endif

//	cerr << progdir << endl;

	if ( !dirH.exists(progdir) )
		dirH.make(progdir);
	if ( !dirH.exists(loaddir) )
		dirH.make(loaddir);
	if ( !dirH.exists(savedir) )
		dirH.make(savedir);
	if ( !dirH.exists(exchangedir) )
		dirH.make(exchangedir);
	if ( !dirH.exists(profiledir) )
		dirH.make(profiledir);
}

std::string Dirlayout::getHomedir()
{

#ifndef _WIN32
	return getenv("HOME");
#else
	//char mydoc[256];
	//memset(mydoc, 0, sizeof(mydoc));
	//SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, mydoc);
	//string a(mydoc);
	//return a;
	return "";
#endif
}

std::string Dirlayout::findInData( const std::string& file )
{
	return "";
}
