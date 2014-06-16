#ifndef BE_FRAME_LIMITER_H_INCLUDED
#define BE_FRAME_LIMITER_H_INCLUDED

#include "utils/settings.h"
#include "be_timer.h"

class BeFrameLimiter
{
	public:
		BeFrameLimiter() :
		 settings(Settings::Instance()),
		 active(Settings::Instance()->getCVarPtr("fpslimit")),
		 optimal(Settings::Instance()->getCVarPtr("fpslimit_frames")),
		 stepsize(50),
		 sleeptime(0)
		{
		}
		~BeFrameLimiter() {};

		void mark(BeTimer *t)
		{
			if ( *active == 1 )
			{
			// new cps
                int cpsi= *optimal;
				float cpsf;
				if ( t->getMilliSeconds() == 0 ) cpsf=static_cast<float>(cpsi) ;
				else cpsf = 1000.f / t->getMilliSeconds();
		
			// calc sleeptime & sleep
				if ( cpsf > *optimal )
					sleeptime += stepsize;
				else if ( cpsf < *optimal )
					sleeptime -= stepsize;

				if (sleeptime > 0 ) SDL_Delay(static_cast<Uint32>(0.01f*sleeptime));
			}
		}

	private:
		Settings*		settings;
		const int*		active;
		const int*		optimal;
		unsigned int		stepsize;
		int			sleeptime;
};

#endif
