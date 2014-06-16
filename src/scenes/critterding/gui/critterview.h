#ifndef CRITTERVIEW_H
#define CRITTERVIEW_H

#include "src/graphics/be_graphics_system.h"
#include "../entities/worldb.h"
#include "src/gui/panel.h"
#include "src/math/vector2f.h"
#include "../entities/critterb.h"
#include "../entities/critterselection.h"

using namespace std;

class Critterview : public Panel
{
	public:
		Critterview(boost::shared_ptr<BeGraphicsSystem> graphicsSystem);
		~Critterview();

		void draw();
		WorldB*		world;
	private:
		Critterselection* critterselection;
		BeWidgetPtr viewbutton;
		btScalar viewposition[16];
		CritterB* currentCritter;
		boost::shared_ptr<BeGraphicsSystem> m_graphicsSystem;

		// drawing helpers
		int v_spacer;
		int v_space;
// 		int v_radius;
// 		int v_diam;
// 		int spacing;
// 		int column;
// 		int row;
// 		int rowlength;
};

#endif
