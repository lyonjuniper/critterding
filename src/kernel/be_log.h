#ifndef BE_LOG_H_INCLUDED
#define BE_LOG_H_INCLUDED

#include <string>
#include <boost/lexical_cast.hpp>

#define ANSI_NORMAL		"\033[0;39m"
#define ANSI_RED		"\033[0;31m"
#define ANSI_RED_BOLD		"\033[1;31m"
#define ANSI_GREEN		"\033[0;32m"
#define ANSI_GREEN_BOLD		"\033[1;32m"
#define ANSI_YELLOW		"\033[0;33m"
#define ANSI_YELLOW_BOLD	"\033[1;33m"
#define ANSI_BLUE		"\033[0;34m"
#define ANSI_BLUE_BOLD		"\033[1;34m"
#define ANSI_MAGENTA		"\033[0;35m"
#define ANSI_MAGENTA_BOLD	"\033[1;35m"
#define ANSI_CYAN		"\033[0;36m"
#define ANSI_CYAN_BOLD		"\033[1;36m"

#define BE_ERROR(msg)								\
{										\
	BeLogDebug beError("ERROR");						\
	beError << msg << "\n";							\
	beError << __FILE__ << " : ";						\
	beError << __LINE__ << "\n";						\
	exit(1);								\
}

#define BE_LOG(msg)									\
{													\
	std::stringstream s;								\
	s << "::" << m_logDebug.m_shortChapter << ":" << __FUNCTION__ << " | "<< msg << "\n";							\
	m_logDebug << s.str();											\
}

class BeLog
{
	public:
		template<typename T>
		BeLog& operator<<(const T& t)
		{
			log(boost::lexical_cast<std::string>(t));
			return *this;
		}
	protected:
		virtual ~BeLog() {};
	private:
		virtual void log(const std::string& message)=0;
};

class BeLogDebug : public BeLog
{
	public:
		BeLogDebug( const std::string& chapter ) : m_shortChapter(chapter), m_chapter("debug_" + chapter) {};
		virtual ~BeLogDebug() {};
		
	public:
		std::string m_shortChapter;
		
	private:
		void log(const std::string& message);
		std::string m_chapter;
};

#endif

