#include "settings.h"
#include "gui/logbuffermessage.h"

Settings* Settings::Instance () 
{
	static Settings t;
	return &t;
}

Settings::Settings() :
 winWidth(0),
 winHeight(0),
 m_logDebug("SETTINGS")
{
	// default profile name, which should be updated during profile load
	m_profileName = "default";
	
	 // home & program directory
	 dirlayout = Dirlayout::Instance();

// 	registerCVar("fullscreen",		new CVar(	0,	0,	1,		true,	"enable fullscreen mode") );
	registerCVar("headless",		new CVar(	0,	0,	1,		true,	"do not open gl context") );
	registerCVar("drawscene",		new CVar(	1,	0,	1,		true,	"draw the scene") );
// 	registerCVar("hdr",			new CVar(	0,	0,	1,		true,	"enable HDR") );
	registerCVar("fsX",			new CVar(	1920,	1,	2147483647,	false,	"fullscreen resolution X") );
	registerCVar("fsY",			new CVar(	1200,	1,	2147483647,	false,	"fullscreen resolution Y") );
	registerCVar("fpslimit_frames",		new CVar(	60,	1,	1000,		false,	"frames per second for the fps limiter") );
	registerCVar("fpslimit",		new CVar(	0,	0,	1,		true,	"enable fps limiter") );
// 	registerCVar("sound",			new CVar(	0,	0,	1,		true,	"enable sound") );

// 	registerCVar("window_safemode",		new CVar(	0,	0,	1,		true,	"enable safemode window settings") );
	registerCVar("window_width",		new CVar(	900,	1,	2147483647,		false,	"window client width in pixels") );
	registerCVar("window_height",		new CVar(	600,	1,	2147483647,		false,	"window client height in pixels") );
	registerCVar("window_color_bits",	new CVar(	24,	16,	32,		false,	"window framebuffer color bits per pixel") );
	registerCVar("window_depth_bits",	new CVar(	24,	8,	32,		false,	"window framebuffer depth bits per pixel") );
	registerCVar("window_stencil_bits",	new CVar(	0,	0,	32,		false,	"window framebuffer stencil bits per pixel") );
	registerCVar("window_multisamples",	new CVar(	0,	0,	16,		false,	"window framebuffer multisamples") );
	registerCVar("window_doublebuffer",	new CVar(	1,	0,	1,		true,	"double buffered window") );
	registerCVar("window_vsync",		new CVar(	0,	0,	1,		true,	"vsynced window") );
	registerCVar("window_resizable",	new CVar(	1,	0,	1,		true,	"enable resizable window") );
	registerCVar("window_fullscreen",	new CVar(	0,	0,	1,		true,	"enable fullscreen window") );
	registerCVar("window_hardware_gl",	new CVar(	1,	0,	1,		true,	"enable hardware GL window") );

// 	registerCVar("texture_aniostropy",	new CVar(	1,	1,	16,		false,	"set anisotropic texture level") );
// 	registerCVar("texture_compression",	new CVar(	1,	1,	16,		false,	"set texture compression") );
// 	registerCVar("texture_filtering_min",	new CVar(	0,	0,	0,		false,	"set texture minification filter") );
// 	registerCVar("texture_filtering_mag",	new CVar(	0,	0,	0,		false,	"set texture magnification filter") );

	//lyon: register local cvar (not saved in profile, but can alter in commandline), for debug switches
	registerLocalCVar("debug_ALL",	new CVar(	0,	0,	1,		true,	"enable debug ALL") );
	registerLocalCVar("debug_FILE",	new CVar(	0,	0,	1,		true,	"enable debug FILE") );
	registerLocalCVar("debug_SETTINGS",	new CVar(	0,	0,	1,		true,	"enable debug SETTINGS") );
	registerLocalCVar("debug_OBJ",	new CVar(	0,	0,	1,		true,	"enable debug OBJ") );
	registerLocalCVar("debug_PHYSTICS",	new CVar(	0,	0,	1,		true,	"enable debug PHYSTICS") );
	registerLocalCVar("debug_SHADER",	new CVar(	0,	0,	1,		true,	"enable debug SHADER") );
	registerLocalCVar("debug_WINDOW",	new CVar(	0,	0,	1,		true,	"enable debug WINDOW") );

}

void Settings::registerCVar(const std::string& name, CVar* cvar)
{
	unregisterCVar(name);
	cvarlist[name] = cvar;
}

void Settings::registerLocalCVar(const std::string& name, CVar* cvar)
{
	cvar->setLocal();
	unregisterCVar(name);
	cvarlist[name] = cvar;
}

int Settings::getCVar(const std::string& name)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
		return cvar->getIntValue();
	return 0;
}

const std::string& Settings::getCVarS(const std::string& name)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
		return cvar->getStringValue();
	else
		BE_ERROR("::SETTINGS error unknown cvar: " << name);
}

const int* Settings::getCVarPtr(const std::string& name)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
		return cvar->getIntValuePointer();
	else
		BE_ERROR("::SETTINGS error unknown cvar: " << name);
}

bool Settings::setCVar(const std::string& name, const unsigned int& value)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
	{
		if ( cvar->set(value) )
			return true;
		else
			BE_LOG( "warning failed to set value for cvar '" << name << "': value must be >=" << cvarlist[name]->getIntMin() << " and <=" << cvarlist[name]->getIntMax() );
	}
	else
		BE_LOG( "unknown cvar: '" << name << "'");
	return false;
}

bool Settings::setCVar(const std::string& name, const std::string& value)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
	{
		if ( cvar->set(value) )
			return true;
		else if ( cvarlist[name]->getType() == T_INT ) {
			BE_LOG( "warning failed to set value for cvar '" << name << "': value must be >=" << cvarlist[name]->getIntMin() << " and <=" << cvarlist[name]->getIntMax() << "'" ); 
		}	else	{ 
			BE_LOG( "warning failed to set value for cvar: '" << name << "'" ); 
		}
	}
	else { 
		BE_LOG( "unknown cvar: '" << name << "'" ); 
	}
	return false;
}

void Settings::increaseCVar(const std::string& name, const unsigned int& value)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
	{
		cvar->increase(value);

		std::stringstream v;
		v << cvar->getIntValue();
		m_logBuffer->add(name + ": " + v.str());
	}
	else
		BE_LOG( "unknown cvar: '" << name << "'" );
}

void Settings::decreaseCVar(const std::string& name, const unsigned int& value)
{
	CVar* cvar = cvarlist[name];
	if ( cvar )
	{
		cvar->decrease(value);

		std::stringstream v;
		v << cvar->getIntValue();
		m_logBuffer->add(name + ": " + v.str());
	}
	else
		BE_LOG( "unknown cvar: '" << name << "'" );
}

void Settings::unregisterCVar(const std::string& name)
{
	const auto& cvarit = cvarlist.find(name);
	if ( cvarit != cvarlist.end() )
	{
		delete cvarit->second;
		cvarlist.erase(cvarit->first);
	}
}

bool Settings::isCVar(const std::string& name)
{
	const auto& cvarit = cvarlist.find(name);
	if ( cvarit != cvarlist.end() )
		return true;

	return false;
}

void Settings::loadProfile(const std::string& filename)
{
	//m_logDebug << "::SETTINGS loading from '" << filename << "'\n";
	BE_LOG("::SETTINGS loading from '" << filename << "'");


	BeFile befileProfile;
	if ( m_filesystem.load( befileProfile, filename ) )
	{
		// update profilename
		unsigned found = filename.find_last_of("/\\");
		std::string file;
		//strip path
		if ( found != std::string::npos ) {
			file = filename.substr(found+1);
		}
		else {
			// no path, all are file
			file = filename;
		}
		//strip ".pro"
		found = file.rfind(".pro");
		if (found != std::string::npos ) {
			m_profileName = file.erase(found);
		}
		else {
			m_profileName = file;
		}
		BE_LOG( "profile name is updated to '" << m_profileName << "'" );
		
		std::string line;
		while ( befileProfile.getLine(line) )
		{
			parseH.reset();
			// trim spaces and comment lines
			parseH.trimWhitespaces(line);
			parseH.removeCommentlines(line);

			if ( !line.empty() )
			{
				std::string sw = parseH.returnUntillStrip( ' ', line );
				if ( !sw.empty() )
				{
					if ( isCVar(sw) )
					{
						while ( parseH.beginMatchesStrip( " ", line ) ) {};
						std::string arg = parseH.returnUntillStrip( ' ', line );
						if ( !setCVar(sw, arg) )
							BE_ERROR("::SETTINGS could not set cvar '" << sw <<  "', value '" << arg <<  "'");
					}
					else
						BE_LOG( "warning: unknown option in profile '" << sw << "'" );
				}
				else
					BE_ERROR("::SETTINGS error: Option without an argument '" << line);
			}
		}
	}
	else
		BE_LOG( "warning: cannot open profile '" << filename << "'" );
}

void Settings::saveProfile()
{
	std::stringstream s;
	s << m_profileName << "-" << time(0);
	std::string profileName( s.str() );
	
// 	std::string fulldir(savedir + "/" + profileName);
	//FIXME: savedir get from settings.xml for string "profiles". 
	//            thus fulldir is effectively "${PWD}/profiles"
	//            should fix it as to Dirlayout::progdir, which is "${HOME}/.critterding"
	//bypass the input parameter savedir
	std::string fulldir(dirlayout->profiledir);

	if ( !dirH.exists(fulldir) )
		dirH.make(fulldir);

	std::string filename(fulldir + "/" + profileName + ".pro");
	std::stringstream buf;
	
	for( auto cvarit = cvarlist.begin(); cvarit != cvarlist.end(); ++cvarit )
	{
		if ( cvarit->second && !cvarit->second->isLocal() )
		{
			if ( cvarit->second->getType() == T_INT )
				buf << cvarit->first << " " << cvarit->second->getIntValue() << std::endl;
			else if ( cvarit->second->getType() == T_STRING )
				buf << cvarit->first << " " << cvarit->second->getStringValue() << std::endl;
		}
	}
	m_filesystem.save(filename, buf.str());

	std::string msg("Profile saved to: " + filename);
	m_logBuffer->add(msg);
	std::cerr << msg << std::endl;
}

void Settings::checkCommandLineOptions(int argc, char *argv[])
{
	// check if --help occurs, overrides the rest
	for (int i=1; i < argc; ++i )
	{
		std::string sw = argv[i];
		if ( sw == "--help" )
		{
			createHelpInfo();
			std::cout << helpinfo.str() << std::endl;
			exit(0);
		}
	}

	// decode arguments
	int optind=1;
	while ((optind < argc)) //  && (argv[optind][0]=='-')
	{
		if ( argv[optind][0]=='-' )
		{
			std::string sw = argv[optind];

			if ( sw=="--profile" )
			{
				if ( argv[++optind] )
					loadProfile(argv[optind]);
				else
					BE_ERROR("::SETTINGS --profile expects a filename");
			}

			else
			{
				parseH.reset();
				if ( parseH.beginMatchesStrip( "--", sw ) )
				{
					sw.append(" ");
					std::string purecmd = parseH.returnUntillStrip( ' ', sw );

					if ( isCVar(purecmd) )
					{
						if ( argv[++optind] )
						{
							if ( !setCVar(purecmd, argv[optind]) )
								BE_ERROR("::SETTINGS error: could not set cvar '" << purecmd << "', value '" << argv[optind] << "'");
						}
						else
							BE_ERROR("::SETTINGS error: option '" << purecmd << "' expects an argument");
					}
					else
						BE_LOG( "warning: unknown commandline option: '" << purecmd << "'" );
				}
			}
		}
		++optind;
	}
 
	if ( optind < argc )
		BE_LOG( "warning: unknown commandline option: '" << argv[optind] << "'" );
}

void Settings::createHelpInfo()
{
	helpinfo << std::endl << "  option                                                    [default] [range]      [comment]" << std::endl << std::endl;
	for( auto cvarit = cvarlist.begin(); cvarit != cvarlist.end(); ++cvarit )
	{
		std::stringstream buf("");
		buf << "  --" << cvarit->first;
		
		for ( unsigned int i=buf.str().size(); i < 62; ++i )
			buf << " ";
		
		if ( cvarit->second->getType() == T_INT )
		{
			buf << cvarit->second->getIntValue();

			for ( unsigned int i=buf.str().size(); i < 72; ++i )
				buf << " ";

			buf << cvarit->second->getIntMin() << "-" << cvarit->second->getIntMax();
		}
		else if ( cvarit->second->getType() == T_STRING )
		{
			buf << cvarit->second->getStringValue();
		}

		for ( unsigned int i=buf.str().size(); i < 85; ++i )
			buf << " ";

		buf << cvarit->second->getComment();

		helpinfo << buf.str() << std::endl;
	}

	helpinfo << std::endl << " To save the default settings to a profile, press \"s\" in the simulation.  \n It will be saved to ./default and can be loaded by using \"--profile ./default\"" << std::endl ;
	helpinfo << std::endl << " Use F1 in the simulation for more information about keys." << std::endl;
}

Settings::~Settings()
{
	for( auto cvarit = cvarlist.begin(); cvarit != cvarlist.end(); ++cvarit )
		delete cvarit->second;
}
