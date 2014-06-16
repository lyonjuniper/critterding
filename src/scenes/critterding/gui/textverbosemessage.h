#ifndef TEXTVERBOSEMESSAGE_H
#define TEXTVERBOSEMESSAGE_H

// #include "src/utils/timer.h" // FIXME howso this is here?
#include "src/gui/panel.h"

using namespace std;

class Textverbosemessage : public Panel
{
	public:
struct vmsg
{
	string str;
	unsigned int	appeartime;
};

		Textverbosemessage();
		virtual ~Textverbosemessage();

		virtual void		draw();

		void		addBirth(stringstream& streamptr);
		void		addDeath(stringstream& streamptr);
		unsigned int	maxMessages;
		unsigned int	msgLifetime;
	protected:
	private:
		vector<vmsg*>	births;
		vector<vmsg*>	deaths;

		void		getLongestMsg();

		void		deleteExpiredMsg();
		
		unsigned int	col2;
};

#endif
