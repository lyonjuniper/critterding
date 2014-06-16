#include "critterview.h"
#include "gui/text.h"
#include "gui/text_uintp.h"

Critterview::Critterview(boost::shared_ptr<BeGraphicsSystem> graphicsSystem) : Panel(false), m_graphicsSystem(graphicsSystem)
{
	critterselection = Critterselection::Instance();
	
	m_active = false;
	isMovable = true;

	// action button settings
	unsigned int bwidth = 90;
	unsigned int bheight = 18;
	unsigned int bspacing = 2;
	unsigned int totalbuttonheight = (2*bheight) + bspacing;

	m_dimensions.set(386, 150);

	int buttons_starty = m_dimensions.m_y - totalbuttonheight-10;

	m_localposition.set(2, 24);

	currentCritter = 0;

	// text widgets
	v_spacer = 14;
	v_space = 3;
	v_space += v_spacer; addWidget( "cv_cid", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Critter") ) );
	v_space += v_spacer; addWidget( "cv_ad", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Adam Distance") ) );
	v_space += v_spacer; addWidget( "cv_age", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Age") ) );
		addWidget( "cv_age_/", Vector2i(200, v_space), BeWidgetTextPtr(new BeWidgetText("/") ) );
		addWidget( "cv_age_max", Vector2i(210, v_space), BeWidgetTextCVARuintPtr(new BeWidgetTextCVARuint(settings->getCVarPtr("critter_maxlifetime"))));

	v_space += v_spacer; addWidget( "cv_energy", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Energy") ) );
		addWidget( "cv_energy_/", Vector2i(200, v_space), BeWidgetTextPtr(new BeWidgetText("/") ) );
		addWidget( "cv_energy_max", Vector2i(210, v_space), BeWidgetTextCVARuintPtr(new BeWidgetTextCVARuint(settings->getCVarPtr("critter_maxenergy"))));

	v_space += v_spacer; addWidget( "cv_neurons", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Neurons") ) );
	v_space += v_spacer; addWidget( "cv_synapses", Vector2i(10, v_space), BeWidgetTextPtr(new BeWidgetText("Synapses") ) );
	
	// view widgets
	viewbutton = addWidgetButton( "cv_view", Vector2i(0+m_dimensions.m_x-60, 35), Vector2i(50, 50), "", Vector2i(0, 0), BeCommand(""), EVENT_NOREPEAT, 0 );

	// close button
	addWidgetButton( "cv_close", Vector2i(m_dimensions.m_x-25, 10), Vector2i(15, 15), "x", BeCommand("gui_togglepanel", "critterview"), EVENT_NOREPEAT, 0 );

	// action buttons
	unsigned int c_width = 10;
	unsigned int c_height = buttons_starty;
	addWidgetButton( "cv_action_kill", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "kill", BeCommand("cs_kill"), EVENT_NOREPEAT, 0 );

	c_height += bspacing + bheight;
	addWidgetButton( "cv_action_brainview", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "brainview", BeCommand("gui_togglepanel", "brainview"), EVENT_NOREPEAT, 0 );
	
	c_width += bwidth + bspacing;
	c_height = buttons_starty;
	addWidgetButton( "cv_action_duplicate", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "duplicate", BeCommand("cs_duplicate"), EVENT_NOREPEAT, 0 );
	
	c_height += bspacing + bheight;
	addWidgetButton( "cv_action_spawnbrainmutant", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "brain mutant", BeCommand("cs_spawnbrainmutant"), EVENT_NOREPEAT, 0 );

	c_width += bwidth + bspacing;
	c_height = buttons_starty;
	addWidgetButton( "cv_action_spawnbodymutant", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "body mutant", BeCommand("cs_spawnbodymutant"), EVENT_NOREPEAT, 0 );

	c_height += bspacing + bheight;
	addWidgetButton( "cv_action_spawnbrainbodymutant", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "brain+body m", BeCommand("cs_spawnbrainbodymutant"), EVENT_NOREPEAT, 0 );

	c_width += bwidth + bspacing;
	c_height = buttons_starty;
	addWidgetButton( "cv_action_feed", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "feed", BeCommand("cs_feed"), EVENT_NOREPEAT, 0 );

	c_height += bspacing + bheight;
	addWidgetButton( "cv_action_resetage", Vector2i(c_width, c_height), Vector2i(bwidth, bheight), "reset age", BeCommand("cs_resetage"), EVENT_NOREPEAT, 0 );
}

void Critterview::draw()
{
	// deactive when selected critter goes missing
	if ( critterselection->selectedCritter == 0 )
	{
		m_active = false;
		currentCritter = 0;
	}

	if (m_active)
	{
		if ( critterselection->selectedCritter != currentCritter )
			currentCritter = critterselection->selectedCritter;

		drawBackground();
		drawBorders();
		// draw the rest
		drawChildren();
		
		// draw values of critter
		v_space = 3;
		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, critterselection->selectedCritter->critterID );
		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, critterselection->selectedCritter->genotype->adamdist );
 		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, critterselection->selectedCritter->totalFrames );
 		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, "%1.1f", critterselection->selectedCritter->energyLevel );
 		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, critterselection->selectedCritter->brain.totalNeurons );
 		v_space += v_spacer; textprinter->print( m_absposition.m_x+110, m_absposition.m_y+v_space, critterselection->selectedCritter->brain.totalSynapses );
		textprinter->print( m_absposition.m_x+200, m_absposition.m_y+v_space, "%5.2f avg", (float)critterselection->selectedCritter->brain.totalSynapses / critterselection->selectedCritter->brain.totalNeurons );

		// draw gl view
		{
			glEnable (GL_DEPTH_TEST);
			glEnable (GL_LIGHTING);
			glEnable (GL_CULL_FACE);

			critterselection->selectedCritter->place( viewbutton->getPosition().m_x+1, *settings->winHeight-49-viewbutton->getPosition().m_y, 49, 49 );

			world->drawWithinCritterSight(critterselection->selectedCritter);
		}

		// switch back to 2d 
		{
			glDisable (GL_DEPTH_TEST);
			glDisable (GL_LIGHTING);
			glDisable (GL_CULL_FACE);
			
			glViewport(0,0,*settings->winWidth,*settings->winHeight);
			m_graphicsSystem->matrixLoadIdentity(GL_PROJECTION);
			m_graphicsSystem->matrixOrtho(GL_PROJECTION, 0, *settings->winWidth, *settings->winHeight, 0,  0, 1);
			m_graphicsSystem->matrixLoadIdentity(GL_MODELVIEW);
			m_graphicsSystem->matrixTranslate(GL_MODELVIEW, 0.5f, -0.5f, 0.0f); // pixel precision offset			
		}
	}
	
}

Critterview::~Critterview()
{
}
