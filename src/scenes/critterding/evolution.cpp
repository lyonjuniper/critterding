#ifdef _MINGW
	#include <unistd.h>
#endif


#include "entities/displaylists.h"
#include "entities/dirlayout.h"
#include "gui/helpinfo.h"
#include "gui/statsgraph.h"
#include "gui/infobar.h"
#include "gui/infostats.h"
#include "gui/textverbosemessage.h"
#include "gui/mutationpanel.h"
#include "gui/globalsettingspanel.h"
#include "gui/settingsbrainpanel.h"
#include "gui/settingsbodypanel.h"
#include "gui/hud.h"
#include "gui/critterview.h"
#include "gui/brainview.h"
#include "gui/speciesview.h"
#include "gui/populationcontrolpanel.h"

#include "gui/exitpanel.h"
#include "gui/enginesettingspanel.h"

#include "utils/be_settings_loader.h"

#include "evolution.h"

#include <math.h>
#include <boost/bind/bind.hpp>

Evolution::Evolution(BeFilesystem& filesystem)
 : frameCounter(0)
 , benchmark_start(0)
{
	m_graphicsSystem = boost::shared_ptr<BeGraphicsSystem>(new BeGraphicsSystem);
	settings = Settings::Instance();
	eventsystem = BeEventSystem::Instance();

	benchmark = settings->getCVarPtr("benchmark");
	benchmark_frames = settings->getCVarPtr("benchmark_frames");
	headless = settings->getCVarPtr("headless");
	drawscene = settings->getCVarPtr("drawscene");
	m_cameraSensitivity_move = settings->getCVarPtr("camerasensitivity_move");
	m_cameraSensitivity_look = settings->getCVarPtr("camerasensitivity_look");

	m_logBuffer = boost::shared_ptr<Logbuffer>(new Logbuffer);

	boost::shared_ptr<Textverbosemessage> textverbosemessage;
	if ( settings->getCVar("headless") == 0 )
	{
		m_glwindow= new GLWindow(BeEventSystem::Instance());
		m_glwindow->create("Critterding beta13");

		m_canvas.reset( new Canvas(filesystem) );
		m_glwindow->setCanvas(m_canvas);
		textverbosemessage.reset( new Textverbosemessage() );
		m_canvas->addWidget( "textverbosemessage", textverbosemessage );
	}

	if ( settings->getCVar("race") == 1 )
		world = new WorldRace( m_graphicsSystem, textverbosemessage );
	else if ( settings->getCVar("roundworld") == 1 )
		world = new Roundworld( m_graphicsSystem, textverbosemessage );
	else if ( settings->getCVar("concavefloor") == 1 )
		world = new Concavefloor( m_graphicsSystem, textverbosemessage );
	else
		world = new WorldB( m_graphicsSystem, textverbosemessage );

	world->m_filesystem = &filesystem;
	world->setLogbuffer(m_logBuffer);
	settings->setLogbuffer(m_logBuffer);

	if ( !*world->headless )
	{
		// init gui panels
		m_canvas->addWidget( "enginesettingspanel", boost::shared_ptr<BeWidget>(new Enginesettingspanel()) );
		m_canvas->addWidget( "helpinfo", boost::shared_ptr<BeWidget>(new Helpinfo()) );
		m_canvas->addWidget( "logbuffermessage", boost::shared_ptr<BeWidget>(new Logbuffermessage( m_logBuffer )) );
		m_canvas->addWidget( "statsgraph", boost::shared_ptr<BeWidget>(new Statsgraph()) );
		m_canvas->addWidget( "infobar", boost::shared_ptr<BeWidget>(new Infobar(&m_timer)) );
// 		m_canvas->addWidget( "infobar",		BeWidgetPtr(new Infobar( m_currentFramesPerSecond, m_averageFramesPerSecond )) );
		
		m_canvas->addWidget( "infostats", boost::shared_ptr<BeWidget>(new Infostats()) );
		m_canvas->addWidget( "panel_exit", boost::shared_ptr<BeWidget>(new Exitpanel()) );
		m_canvas->addWidget( "mutationpanel", boost::shared_ptr<BeWidget>(new Mutationpanel()) );
		m_canvas->addWidget( "globalsettingspanel", boost::shared_ptr<BeWidget>(new Globalsettingspanel()) );
		m_canvas->addWidget( "settingsbrainpanel", boost::shared_ptr<BeWidget>(new Settingsbrainpanel()) );
		m_canvas->addWidget( "settingsbodypanel", boost::shared_ptr<BeWidget>(new Settingsbodypanel()) );
		m_canvas->addWidget( "critterview", boost::shared_ptr<BeWidget>(new Critterview(m_graphicsSystem)) );
		m_canvas->addWidget( "brainview", boost::shared_ptr<BeWidget>(new Brainview()) );
		m_canvas->addWidget( "hud", boost::shared_ptr<BeWidget>(new Hud(m_graphicsSystem)) );
		m_canvas->addWidget( "speciesview", boost::shared_ptr<BeWidget>(new Speciesview()) );
		m_canvas->addWidget( "populationcontrolpanel", boost::shared_ptr<BeWidget>(new Populationcontrolpanel()) );
		m_canvas->setDefaultZAxis();

		static_cast<Hud*>(m_canvas->children["hud"].get())->world = world;
		static_cast<Critterview*>(m_canvas->children["critterview"].get())->world = world;
	}
	else
	{
		// check if raycastvision is enabled, if not die
		if ( settings->getCVar("critter_raycastvision") == 0 )
		{
			cout << "headless mode requires critter_raycastvision to be enabled" << endl;
			exit(1);
		}
	}
	
 	BeCommandSystem::Instance()->registerVoidCommand("quit", boost::bind(&Evolution::quit, this));
	BeCommandSystem::Instance()->registerIntCommand("cs_unregister", boost::bind(&Evolution::unregisterCritterVID, this, _1));
	BeCommandSystem::Instance()->registerVoidCommand("cs_clear", boost::bind(&Evolution::clear, this));

	BeCommandSystem::Instance()->registerVoidCommand("decreaseenergy", boost::bind(&Evolution::decreaseenergy, this));
	BeCommandSystem::Instance()->registerVoidCommand("increaseenergy", boost::bind(&Evolution::increaseenergy, this));
	BeCommandSystem::Instance()->registerVoidCommand("dec_foodmaxenergy", boost::bind(&Evolution::decreasefoodmaxenergy, this));
	BeCommandSystem::Instance()->registerVoidCommand("inc_foodmaxenergy", boost::bind(&Evolution::increasefoodmaxenergy, this));
	BeCommandSystem::Instance()->registerVoidCommand("dec_worldsizex", boost::bind(&Evolution::dec_worldsizex, this));
	BeCommandSystem::Instance()->registerVoidCommand("inc_worldsizex", boost::bind(&Evolution::inc_worldsizex, this));
	BeCommandSystem::Instance()->registerVoidCommand("dec_worldsizey", boost::bind(&Evolution::dec_worldsizey, this));
	BeCommandSystem::Instance()->registerVoidCommand("inc_worldsizey", boost::bind(&Evolution::inc_worldsizey, this));
// 	BeCommandSystem::Instance()->registerVoidCommand("dec_worldsizez", boost::bind(&Evolution::dec_worldsizez, this));
// 	BeCommandSystem::Instance()->registerVoidCommand("inc_worldsizez", boost::bind(&Evolution::inc_worldsizez, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_moveup", boost::bind(&Evolution::camera_moveup, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_movedown", boost::bind(&Evolution::camera_movedown, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_moveforward", boost::bind(&Evolution::camera_moveforward, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_movebackward", boost::bind(&Evolution::camera_movebackward, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_moveleft", boost::bind(&Evolution::camera_moveleft, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_moveright", boost::bind(&Evolution::camera_moveright, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_lookup", boost::bind(&Evolution::camera_lookup, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_lookdown", boost::bind(&Evolution::camera_lookdown, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_lookleft", boost::bind(&Evolution::camera_lookleft, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_lookright", boost::bind(&Evolution::camera_lookright, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_rollleft", boost::bind(&Evolution::camera_rollleft, this));
	BeCommandSystem::Instance()->registerVoidCommand("camera_rollright", boost::bind(&Evolution::camera_rollright, this));
// 	BeCommandSystem::Instance()->registerVoidCommand("camera_lookhorizontal", &Evolution::camera_lookhorizontal);
// 	BeCommandSystem::Instance()->registerVoidCommand("camera_lookvertical", &Evolution::camera_lookvertical);
// 	BeCommandSystem::Instance()->registerVoidCommand("camera_movehorizontal", &Evolution::camera_movehorizontal);
// 	BeCommandSystem::Instance()->registerVoidCommand("camera_movevertical", &Evolution::camera_movevertical);
	BeCommandSystem::Instance()->registerVoidCommand("cs_selectall", boost::bind(&Evolution::selectCritterAll, this));

	BeCommandSystem::Instance()->registerVoidCommand("loadallcritters", boost::bind(&WorldB::loadAllCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("saveallcritters", boost::bind(&WorldB::saveAllCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("insertcritter", boost::bind(&WorldB::insertCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("killhalfofcritters", boost::bind(&WorldB::killHalfOfCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("camera_resetposition", boost::bind(&WorldB::resetCamera, world));
	BeCommandSystem::Instance()->registerVoidCommand("toggle_pause", boost::bind(&WorldB::togglePause, world));
// 	BeCommandSystem::Instance()->registerVoidCommand("toggle_sleeper", &WorldB::toggleSleeper);
	BeCommandSystem::Instance()->registerVoidCommand("toggle_mouselook", boost::bind(&WorldB::toggleMouselook, world));
	BeCommandSystem::Instance()->registerVoidCommand("critter_select", boost::bind(&WorldB::selectBody, world));
	BeCommandSystem::Instance()->registerVoidCommand("critter_deselect", boost::bind(&WorldB::deselectBody, world));
	BeCommandSystem::Instance()->registerVoidCommand("critter_pick", boost::bind(&WorldB::pickBody, world));
	BeCommandSystem::Instance()->registerVoidCommand("critter_unpick", boost::bind(&WorldB::unpickBody, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_kill", boost::bind(&WorldB::removeSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_killall", boost::bind(&WorldB::removeAllSelectedCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_duplicate", boost::bind(&WorldB::duplicateSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbrainmutant", boost::bind(&WorldB::spawnBrainMutantSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbodymutant", boost::bind(&WorldB::spawnBodyMutantSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbrainbodymutant", boost::bind(&WorldB::spawnBrainBodyMutantSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_duplicateall", boost::bind(&WorldB::duplicateAllSelectedCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbrainmutantall", boost::bind(&WorldB::spawnBrainMutantAllSelectedCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbodymutantall", boost::bind(&WorldB::spawnBodyMutantAllSelectedCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_spawnbrainbodymutantall", boost::bind(&WorldB::spawnBrainBodyMutantAllSelectedCritters, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_feed", boost::bind(&WorldB::feedSelectedCritter, world));
	BeCommandSystem::Instance()->registerVoidCommand("cs_resetage", boost::bind(&WorldB::resetageSelectedCritter, world));

	BeCommandSystem::Instance()->registerIntCommand("cs_select", boost::bind(&Evolution::gui_selectCritter, this, _1));
	BeCommandSystem::Instance()->registerStringCommand("gui_togglepanel", boost::bind(&Evolution::canvas_swapchild, this, _1));
	BeCommandSystem::Instance()->registerVoidCommand("gui_toggle", boost::bind(&Evolution::gui_swap, this));

	BeCommandSystem::Instance()->registerFloatCommand("movepickedbodyX", boost::bind(&Evolution::movePickedBodyX, this, _1));
	BeCommandSystem::Instance()->registerFloatCommand("movepickedbodyY", boost::bind(&Evolution::movePickedBodyY, this, _1));
/*	BeCommandSystem::Instance()->registerVoidCommand("canvas_press", boost::bind(&Evolution::canvas_press, this));
	BeCommandSystem::Instance()->registerVoidCommand("canvas_release", boost::bind(&Evolution::canvas_release, this));
	BeCommandSystem::Instance()->registerVoidCommand("canvas_pressAlt", boost::bind(&Evolution::canvas_pressAlt, this));
	BeCommandSystem::Instance()->registerVoidCommand("canvas_releaseAlt", boost::bind(&Evolution::canvas_releaseAlt, this));*/
	
	//Load control definitions
	BeFile befileControls;
	if ( filesystem.load(befileControls, "settings.xml") )
	{
		BeSettingsLoader settingsLoader;
		settingsLoader.load( BeEventSystem::Instance(), &befileControls );
	}
	
	if ( *benchmark == 1 )
	{
		m_canvas->deactivate();
// 		settings->setCVar("startseed", 11);
		settings->setCVar("fpslimit", 0);
	}

	world->init();
}

void Evolution::process()
{
	m_timer.mark();
	world->framelimiter.mark( &m_timer );
	++frameCounter;

	m_logBuffer->deleteExpiredMsg( m_timer.getSeconds() );
	
	const float timeDelta=m_timer.getSeconds();
	m_cameraTranslateSpeed=1.0f*timeDelta * *m_cameraSensitivity_move; //0.5f**m_cameraSensitivity;
	m_cameraRotateSpeed=0.012f*timeDelta * *m_cameraSensitivity_look; //0.0001f**m_cameraSensitivity;

	if ( *benchmark == 1 )
	{
		if ( frameCounter == 3 )
		{
			benchmark_start = m_timer.getTotalMilliSeconds();
// 			cout << "time skipped" << benchmark_start << endl;
		}

		else if ( frameCounter == *benchmark_frames )
		{
			const unsigned int ms = m_timer.getTotalMilliSeconds() - benchmark_start;

			cout << std::setprecision(8);
			cout << "BENCHMARK RESULT : ";
			cout << "(seed:" << world->randgen->Instance()->getSeed() << ",checksum:" << fabs(world->m_freeEnergy) + world->critters.size() + world->entities.size() << ") ";
// 			cout << std::setprecision(8);
			cout << frameCounter << "f/" << 0.001 * ms << "s " << " = ";
			cout << (1000.0 * frameCounter) / ms << " fps";
			cout << endl;
// 			cout << "and btw, freeEnergy: " << world->m_freeEnergy << ", critters: " << world->critters.size() << endl;
			exit(0);
		}
	}

	if ( !*world->headless )
	{
		m_glwindow->process();
		eventsystem->processEvents( m_timer.getMilliSeconds() );
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 			glPushMatrix();
		
		// LIGHT
			GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

// 			GLfloat modelSpecular[] = {0.2f, 0.2f, 0.2f, 1.0f};
// 			glMaterialfv(GL_FRONT, GL_SPECULAR, modelSpecular);

// 			GLfloat modelShininess[] = {124.55f};
// 			glMaterialfv(GL_FRONT, GL_SHININESS, modelShininess);
			
			
			GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
			glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
			
			GLfloat lightDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
			
// 			GLfloat lightSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
// 			glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
			
			
// 			GLfloat lightPos[] = { 0.5f * *world->worldsizeX, 50.0f, 0.5f * *world->worldsizeY, 1.0f };
// 			GLfloat lightPos[] = { 0.0f, 50.0f, 0.0f, 1.0f };
			
			if ( settings->getCVar("roundworld") == 1 )
			{
				GLfloat lightPos[] = { 0.0f, 3.0f * *world->worldsizeX, 0.0f, 1.0f };
	// 			GLfloat lightPos[] = { 0.25f * *world->worldsizeX, 0.5f * *world->worldsizeX, 0.25f * *world->worldsizeY, 1.0f };
				glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
				
				GLfloat lightAttenuation[] = { 0.02f };
				glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, lightAttenuation);
				
				GLfloat lightLAttenuation[] = { 0.002f };
				glLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, lightLAttenuation);
		
				GLfloat lightQAttenuation[] = { 0.00003f };
				glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, lightQAttenuation);
			}
			else
			{
				GLfloat lightPos[] = { 0.47f * *world->worldsizeX, 0.3f * (*world->worldsizeX + *world->worldsizeY), 0.047f * *world->worldsizeY, 1.0f };
// 				GLfloat lightPos[] = { 0.0f, 0.1f * *world->worldsizeX, 0.0f, 1.0f };
	// 			GLfloat lightPos[] = { 0.25f * *world->worldsizeX, 0.5f * *world->worldsizeX, 0.25f * *world->worldsizeY, 1.0f };
				glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
				
				GLfloat lightAttenuation[] = { 0.6f };
				glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, lightAttenuation);
		
				GLfloat lightLAttenuation[] = { 0.00001f };
				glLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, lightLAttenuation);
		
				GLfloat lightQAttenuation[] = { 0.000000f };
				glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, lightQAttenuation);
				
			}

			GLfloat lightDir[] = { 0.0f, -1.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
			
// 			GLfloat lightCutoff[] = { m_spot_cutoff };
// 			glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, lightCutoff);
// 
// 			GLfloat lightExponent[] = { m_spot_exponent };
// 			glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, lightExponent);
// 			
			
// 		glPopMatrix();
			
			
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

// 		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);
		
		glEnable(GL_CULL_FACE);
		glCullFace( GL_BACK );

// 		// Hint for everything to be nicest
// 		{
// 			glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
// 			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
// 			glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
// 
// 			glHint(GL_FOG_HINT, GL_NICEST);
// 			glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
// 			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
// 			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
// 			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
// 
// 			glShadeModel(GL_SMOOTH);
// 			glEnable(GL_DITHER);
// 			glEnable(GL_POLYGON_SMOOTH);
// 	// 		glEnable(GL_BLEND);
// 	// 
// 			glEnable(GL_MULTISAMPLE);
// 			glEnable(GL_LINE_SMOOTH);
// 		}
			
		
	}

	world->process(m_timer);

	if ( !*world->headless )
	{
			// 			if (world->critters.size() > 1 )
			// 			{
			// 				world->camera.follow( (btDefaultMotionState*)world->critters[0]->body.mouths[0]->body->getMotionState() );
			// 				world->drawWithoutFaces();
			// 				world->critters[0]->printVision();
			// 			}

			// 		world->camera.place(1);
			// 		world->m_sceneNodeCamera.setOrigin( btVector3( 0, 0, 0 ) );



				{ //NEW
					glViewport(0,0,*settings->winWidth,*settings->winHeight);
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					
					world->m_camera.m_aspect = (float) *settings->winWidth / *settings->winHeight;
					const float frustumHalfHeight = tan( world->m_camera.m_fovy * 180 / M_PI * M_PI / 360.0 ) * world->m_camera.m_zNear;
					const float frustumHalfWidth = frustumHalfHeight * world->m_camera.m_aspect;
					glFrustum(-frustumHalfWidth, frustumHalfWidth, -frustumHalfHeight, frustumHalfHeight, world->m_camera.m_zNear, world->m_camera.m_zFar);

					world->m_sceneNodeCamera.getTransform().inverse().getOpenGLMatrix(cam_position);

					glMultMatrixf(cam_position);

					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
				}

					if ( *drawscene == 1 )
					{
			// 		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
			// 		glHint(GL_FOG_HINT, GL_FASTEST);
			// 		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			// 		glShadeModel(GL_FLAT);
			// 		glShadeModel(GL_SMOOTH);

						world->drawWithGrid();

						// draw selected info
						btTransform trans;
						trans.setIdentity();

						btTransform up;
						up.setIdentity();
						up.setOrigin( btVector3(0.0f, 0.2f, 0.0f) );

						for ( unsigned int i=0; i < world->critterselection->clist.size(); i++ )
						{
							trans.setOrigin(world->critterselection->clist[i]->body.mouths[0]->ghostObject->getWorldTransform().getOrigin());
							trans.getOrigin().setY(trans.getOrigin().getY()+0.5f);
							trans.setBasis(world->m_sceneNodeCamera.getTransform().getBasis());
							trans *= up;
							trans.getOpenGLMatrix(cam_position);

							glPushMatrix(); 
			// 				glMultMatrixf(position);
			// 				glMultMatrixd(position); // FIXME WATCHIT OK WENT TO DOUBLE HERE 
							Displaylists::Instance()->glMultiMatrix(cam_position);

								glColor3f(1.5f, 1.5f, 1.5f);
								glBegin(GL_LINES);
									glVertex2f(-0.2f, 0.05f);
									glVertex2f(-0.2f,-0.05f);

									glVertex2f(-0.2f,-0.05f);
									glVertex2f(0.2f, -0.05f);

									glVertex2f(0.2f, -0.05f);
									glVertex2f(0.2f,  0.05f);

									glVertex2f(0.2f,  0.05f);
									glVertex2f(-0.2f, 0.05f);
								glEnd();

							glPopMatrix();
						}
					}

		// 2D
			if ( m_canvas->isActive() )
			{
				glPushMatrix(); 
				{
					glDisable(GL_DEPTH_TEST);
					glDisable (GL_LIGHTING);
					glDisable(GL_CULL_FACE);

					world->m_graphicsSystem->matrixLoadIdentity(GL_PROJECTION);
					
					world->m_graphicsSystem->matrixOrtho(GL_PROJECTION, 0, *settings->winWidth, *settings->winHeight, 0,  0, 1);
					world->m_graphicsSystem->matrixLoadIdentity(GL_MODELVIEW);
					world->m_graphicsSystem->matrixTranslate(GL_MODELVIEW, 0.5f, -0.5f, 0.0f); // pixel precision offset

					m_canvas->draw();

					if ( *drawscene == 1 )
					{
						if (!world->mouselook && !m_canvas->mouseFocus )
						{
							world->castMouseRay();

							// hover test
							if ( world->mouseRayHit )
							{
								unsigned int margin = 20;
								unsigned int rmargindistance = 70;
								unsigned int vspacer = 12;
								glColor3f(1.0f, 1.0f, 1.0f);
								if ( world->mouseRayHitEntity->type == 1 )
								{
									Textprinter::Instance()->print( world->mousex+margin, world->mousey,    "food");
									Textprinter::Instance()->print( world->mousex+margin, world->mousey+vspacer, "energy");
									Textprinter::Instance()->print(world->mousex+rmargindistance, world->mousey+vspacer, "%1.1f", static_cast<const Food*>(world->mouseRayHitEntity)->energyLevel);
								}
								else if ( world->mouseRayHitEntity->type == 0 )
								{
									const CritterB* c = static_cast<const CritterB*>(world->mouseRayHitEntity);
									Textprinter::Instance()->print( world->mousex+margin, world->mousey,    "critter");
									Textprinter::Instance()->print(world->mousex+rmargindistance, world->mousey, "%1i", c->critterID);
									Textprinter::Instance()->print( world->mousex+margin, world->mousey+vspacer, "energy");
									Textprinter::Instance()->print(world->mousex+rmargindistance, world->mousey+vspacer, "%1.1f", c->energyLevel);
								}
							}
						}
					}
				}
				glPopMatrix(); 
			}

// 		#ifdef _DEBUG
			GLenum err (glGetError());
			while(err!=GL_NO_ERROR)
			{
				std::string error;

				switch(err)
				{
					case GL_INVALID_OPERATION:	error="INVALID_OPERATION";	break;
					case GL_INVALID_ENUM:		error="INVALID_ENUM";		break;
					case GL_INVALID_VALUE:		error="INVALID_VALUE";		break;
					case GL_OUT_OF_MEMORY:		error="OUT_OF_MEMORY";		break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:	error="INVALID_FRAMEBUFFER_OPERATION";	break;
				}

				std::cout<<"GL_"<<error<<" - "<<':'<<std::endl;
				err=glGetError();
			}
// 		#endif


		SDL_GL_SwapBuffers();		
	}

	if ( world->critters.size() == 0 && settings->getCVar("exit_if_empty") )
	{
		cout << "world is empty, exiting..." << endl;
// 		Cdcommands::Instance()->quit();
		quit();
	}
}

void Evolution::handleMouseMotionAbs(int x, int y)
{
	std::cout << "abs" << std::endl;
	if ( !world->mouselook )
	{
		world->mousex = x;
		world->mousey = y;
		
		// world mouse dynamics
		world->calcMouseDirection();
		world->movePickedBodyTo();
	}
}

void Evolution::handleMouseMotionRel(int x, int y)
{
	std::cout << "rel" << std::endl;
	if ( world->mouselook )
	{
		world->relx = x;
		world->rely = y;
		eventsystem->activateKeystate(2248+0);
		eventsystem->activateKeystate(2248+1);
	}
}

// COMMANDS
	void Evolution::selectCritterAll()
	{
		world->critterselection->clear();
		for ( unsigned int i=0; i < world->critters.size(); i++ )
			world->critterselection->registerCritter(world->critters[i]);
	}

	// FIXME Move this shit to evolution.cpp?
	void Evolution::selectCritter(const unsigned int& c)
	{
	// 	m_canvas->swapChild("critterview");
		world->critterselection->selectCritterVID(c);
	}

	void Evolution::decreaseenergy()
	{
		if ( ( (int)settings->getCVar("energy") - 1 ) >= 0 )
		{
			settings->setCVar("energy", settings->getCVar("energy")-1 );
// 			world->m_freeEnergy -= settings->getCVar("food_maxenergy");
			
// 			stringstream buf;
// 			buf << "energy: " << settings->getCVar("energy");
// 			m_logBuffer->add(buf);
		}
	}

	void Evolution::increaseenergy()
	{
		settings->setCVar("energy", settings->getCVar("energy")+1 );
// 		world->m_freeEnergy += settings->getCVar("food_maxenergy");

// 		stringstream buf;
// 		buf << "energy: " << settings->getCVar("energy");
// 		m_logBuffer->add(buf);
	}

	void Evolution::decreasefoodmaxenergy()
	{
		if ( ( (int)settings->getCVar("food_maxenergy") - 1 ) >= 0 )
		{
// 			world->m_freeEnergy -= settings->getCVar("energy");
			settings->setCVar("food_maxenergy", settings->getCVar("food_maxenergy")-1 );
		}
	}

	void Evolution::increasefoodmaxenergy()
	{
// 		world->m_freeEnergy += settings->getCVar("energy");
		settings->setCVar("food_maxenergy", settings->getCVar("food_maxenergy")+1 );
	}

	void Evolution::dec_worldsizex() { settings->decreaseCVar("worldsizeX", 1); world->makeFloor(); }
	void Evolution::inc_worldsizex() { settings->increaseCVar("worldsizeX", 1); world->makeFloor(); }
	void Evolution::dec_worldsizey() { settings->decreaseCVar("worldsizeY", 1); world->makeFloor(); }
	void Evolution::inc_worldsizey() { settings->increaseCVar("worldsizeY", 1); world->makeFloor(); }
// 	void Evolution::dec_worldsizez() { settings->decreaseCVar("worldsizeZ", 1); world->makeFloor(); }
// 	void Evolution::inc_worldsizez() { settings->increaseCVar("worldsizeZ", 1); world->makeFloor(); }

	void Evolution::camera_moveup()
	{
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(0,m_cameraTranslateSpeed,0));
		}
		world->movePickedBodyFrom();
	}
	void Evolution::camera_movedown()
	{
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(0,-m_cameraTranslateSpeed,0));
		}
		world->movePickedBodyFrom();
	}
	void Evolution::camera_moveforward()
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(0,0,-m_cameraTranslateSpeed));
		}
		world->movePickedBodyFrom(); 
	}
	void Evolution::camera_movebackward() 	
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(0,0,m_cameraTranslateSpeed));
		}
		world->movePickedBodyFrom(); 
	}
	void Evolution::camera_moveleft() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(-m_cameraTranslateSpeed,0,0));
		}
		world->movePickedBodyFrom(); 
	}
	void Evolution::camera_moveright() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->translateLocal(btVector3(m_cameraTranslateSpeed,0,0));
		}
		world->movePickedBodyFrom(); 
	}
	void Evolution::camera_lookup() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->pitch(m_cameraRotateSpeed);
		}
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	void Evolution::camera_lookdown() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->pitch(-m_cameraRotateSpeed);
		}
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	void Evolution::camera_lookleft() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->yaw(m_cameraRotateSpeed); 
		}
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	void Evolution::camera_lookright() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->yaw(-m_cameraRotateSpeed);
		}
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	void Evolution::camera_rollleft() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->roll(m_cameraRotateSpeed); 		
		}
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	void Evolution::camera_rollright() 		
	{ 
		BeSceneNode* const sceneNode=world->m_camera.getSceneNode();
		if(sceneNode)
		{
			sceneNode->roll(-m_cameraRotateSpeed); 
		}	
		world->calcMouseDirection(); 
		world->movePickedBodyTo(); 
	}
	
	
// 	void Evolution::camera_movedown() { world->camera.moveDown(0.01f); world->movePickedBodyFrom(); }
// 	void Evolution::camera_moveforward() { world->camera.moveForward(0.01f); world->movePickedBodyFrom(); }
// 	void Evolution::camera_movebackward() { world->camera.moveBackward(0.01f); world->movePickedBodyFrom(); }
// 	void Evolution::camera_moveleft() { world->camera.moveLeft(0.01f); world->movePickedBodyFrom(); }
// 	void Evolution::camera_moveright() { world->camera.moveRight(0.01f); world->movePickedBodyFrom(); }
// 	void Evolution::camera_lookup() { world->camera.lookUp(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_lookdown() { world->camera.lookDown(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_lookleft() { world->camera.lookLeft(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_lookright() { world->camera.lookRight(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_rollleft() { world->camera.rollLeft(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_rollright() { world->camera.rollRight(0.001f); world->calcMouseDirection(); world->movePickedBodyTo(); }
// /*/*/*	void Evolution::camera_lookhorizontal() { world->camera.lookRight((float)world->relx/3000); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_lookvertical() { world->camera.lookDown((float)world->rely/3000); world->calcMouseDirection(); world->movePickedBodyTo(); }
// 	void Evolution::camera_movehorizontal() { world->camera.moveRight((float)world->relx/300); world->movePickedBodyFrom(); }
// 	void Evolution::camera_movevertical() { world->camera.moveDown((float)world->rely/300); world->movePickedBodyFrom(); }*/*/*/

	void Evolution::unregisterCritterVID(int vectorID)
	{
		world->critterselection->unregisterCritterVID(vectorID);
	}

	void Evolution::clear()
	{
		world->critterselection->clear();
	}

	void Evolution::gui_selectCritter(int c)
	{
		m_canvas->children["critterview"]->activate();
		m_canvas->raisePanel(m_canvas->children["critterview"].get());
		world->critterselection->selectCritterVID(c);
	}

	void Evolution::gui_swap()
	{
		m_canvas->swap();
	}
	void Evolution::movePickedBodyX( float value )
	{
		if ( !world->mouselook )
		{
			world->mousex = value;
			world->calcMouseDirection();
			world->movePickedBodyTo();
		}
	}

	void Evolution::movePickedBodyY( float value )
	{
		if ( !world->mouselook )
		{
			world->mousey = value;
			// world mouse dynamics
			world->calcMouseDirection();
			world->movePickedBodyTo();
		}		
	}
	
// 	void Evolution::canvas_press()
// 	{
// 		m_canvas->buttonPress( 1 );
// 		world->selectBody();
// 	}
// 	void Evolution::canvas_release()
// 	{
// 		m_canvas->buttonRelease( 1 );
// 	}
// 	void Evolution::canvas_pressAlt()
// 	{
// 		m_canvas->buttonPress( 3 );
// 		world->pickBody();
// 	}
// 	void Evolution::canvas_releaseAlt()
// 	{
// 		m_canvas->buttonRelease( 3 );
// 		world->unpickBody();
// 	}
	void Evolution::canvas_swapchild(const string& child)
	{
		m_canvas->swapChild( child );
	}

	

Evolution::~Evolution()
{
	delete m_glwindow;
	delete world;
}
