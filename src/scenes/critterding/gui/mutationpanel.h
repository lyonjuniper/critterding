#ifndef MUTATIONPANEL_H
#define MUTATIONPANEL_H

#include "src/gui/settingspanel.h"

using namespace std;

class Mutationpanel : public Settingspanel
{
	public:
		Mutationpanel();
		~Mutationpanel();

	private:
		unsigned int vspace;
		unsigned int hspace;
};

#endif
