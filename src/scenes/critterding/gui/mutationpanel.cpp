#include "mutationpanel.h"

Mutationpanel::Mutationpanel()
{
	isMovable = true;

	m_localposition.set(2, 24);

	col2 = 150;
	col3 = 200;
	
	addSettingmutator("body_maxmutations", 0, 0);
	addSettingmutator("body_mutationrate", 0, 0);

	row += rowspacer;
	addSettingmutator("brain_maxmutations", 0, 0);
	addSettingmutator("brain_mutationrate", 0, 0);

	m_dimensions.set(col3 + 40, row-rowspacer+10+10);  // 10 = height of 1 line, 10 = horizontal margin
}

Mutationpanel::~Mutationpanel()
{
}
