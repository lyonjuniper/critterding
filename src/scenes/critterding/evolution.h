#ifndef EVOLUTION_H_INCLUDED
#define EVOLUTION_H_INCLUDED

#include "gl/glwindow.h"
// #include <SDL/SDL.h>
#include "common/be_scene.h"
#include "common/be_event_system.h"

#include "entities/worldb.h"
#include "modes/race.h"
#include "modes/roundworld.h"
#include "modes/concavefloor.h"
#include "gui/canvas.h"
#include "gui/logbuffermessage.h"

class Evolution : public  BeScene {

	public:
		Evolution(BeFilesystem& filesystem);
		~Evolution();

		void process();

		void handleMouseMotionRel(int x, int y);
		void handleMouseMotionAbs(int x, int y);

		WorldB* world;

		// COMMANDS
			void decreaseenergy();
			void increaseenergy();
			void decreasefoodmaxenergy();
			void increasefoodmaxenergy();
			void dec_worldsizex();
			void inc_worldsizex();
			void dec_worldsizey();
			void inc_worldsizey();
// 			void dec_worldsizez();
// 			void inc_worldsizez();
			void selectCritterAll();
			void selectCritter(const unsigned int& c);

			void camera_moveup();
			void camera_movedown();
			void camera_moveforward();
			void camera_movebackward();
			void camera_moveleft();
			void camera_moveright();
			void camera_lookup();
			void camera_lookdown();
			void camera_lookleft();
			void camera_lookright();
			void camera_rollleft();
			void camera_rollright();
// 			void camera_lookhorizontal();
// 			void camera_lookvertical();
// 			void camera_movehorizontal();
// 			void camera_movevertical();
			void unregisterCritterVID(int vectorID);
			void clear();
			void gui_selectCritter(int c);
			void gui_swap();

// 			void canvas_setx( float value );
// 			void canvas_sety( float value );
			void movePickedBodyX( float value );
			void movePickedBodyY( float value );

// 			void canvas_press();
// 			void canvas_release();
// 			void canvas_pressAlt();
// 			void canvas_releaseAlt();
			void canvas_swapchild(const string& child);

	private:
		Settings *settings;
		BeEventSystem *eventsystem;

		long long frameCounter;

		unsigned int benchmark_start;
		const int* benchmark;
		const int* benchmark_frames;
		const int* drawscene;
		const int* headless;
		const int* m_cameraSensitivity_move;
		const int* m_cameraSensitivity_look;
 		boost::shared_ptr<Canvas> m_canvas;

		float m_cameraTranslateSpeed;
		float m_cameraRotateSpeed;

		// GL WINDOW
		GLWindow*	m_glwindow;
		BeTimer		m_timer;
		boost::shared_ptr<BeGraphicsSystem> m_graphicsSystem;
		
		boost::shared_ptr<Logbuffer>	m_logBuffer;
		float cam_position[16];
		
		unsigned int		m_currentFramesPerSecond;
		unsigned int		m_averageFramesPerSecond;
};
#endif
