#ifndef HUD_H
#define HUD_H

#include "src/graphics/be_graphics_system.h"
#include <vector>
#include "src/gui/panel.h"
#include "../entities/worldb.h"
#include "../entities/critterselection.h"

using namespace std;

class Hud : public Panel
{
	public:
		Hud(boost::shared_ptr<BeGraphicsSystem> graphicsSystem);
		~Hud();
		void		draw();
		WorldB*		world;
		
	private:
		void drawBorders();
		Critterselection* critterselection;
		vector<BeWidgetPtr> cbuttons; // FIXME save as buttons
		float m_viewposition[16];
		boost::shared_ptr<BeGraphicsSystem> m_graphicsSystem;
		
		const unsigned int m_bwidth;
		const unsigned int m_bheight;
};

#endif
