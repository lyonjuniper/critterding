#include "settingsbodypanel.h"

Settingsbodypanel::Settingsbodypanel()
{
	isMovable = true;

	m_localposition.set(2, 24);

	col2 = 370;
	col3 = 450;

	addSettingmutator("body_maxbodyparts", 0, 0);
	addSettingmutator("body_maxbodypartsatbuildtime", 0, 0);
	addSettingmutator("body_minbodypartsize", 0, 0);
	addSettingmutator("body_maxbodypartsize", 0, 0);
	addSettingmutator("body_minheadsize", 0, 0);
	addSettingmutator("body_maxheadsize", 0, 0);

	row+=8;

	addSettingmutator("body_maxconstraintangle", 0, 0);
	addSettingmutator("body_maxconstraintlimit", 0, 0);
	
	row+=8;

	addSettingmutator("body_percentmutateeffectchangecolor", 0, 0);
	addSettingmutator("body_percentmutateeffectchangecolor_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectaddbodypart", 0, 0);
	addSettingmutator("body_percentmutateeffectremovebodypart", 0, 0);
	addSettingmutator("body_percentmutateeffectresizebodypart", 0, 0);
	addSettingmutator("body_percentmutateeffectresizebodypart_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintlimits", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintlimits_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintangles", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintangles_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintposition", 0, 0);
	addSettingmutator("body_percentmutateeffectchangeconstraintposition_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectresizehead", 0, 0);
	addSettingmutator("body_percentmutateeffectresizehead_slightly", 0, 0);
	addSettingmutator("body_percentmutateeffectrepositionhead", 0, 0);
	
	row+=8;
	
	addSettingmutator("body_costhavingbodypart", 0, 0);
	addSettingmutator("body_costtotalweight", 0, 0);

	m_dimensions.set(col3 + 40, row-rowspacer+10+10);  // 10 = height of 1 line, 10 = horizontal margin
}

Settingsbodypanel::~Settingsbodypanel()
{
}
