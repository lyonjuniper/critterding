//         //update shape sizes
//         for (unsigned i = 0; i < numNeurons; ++i) {
//             btCollisionShape* bt = neuronShape[i];
//             float s = neuronSize;
//             float w = s * (1.0 + (fabs(brain->neurons[i]->getOutput())));
//             float h = s * (1.0 + sqrt(fabs(brain->neurons[i]->potential)));
//             bt->setLocalScaling(btVector3(w, h, (w + h) / 2.0));
//         }

// FIXME benchmark NOT checking distance for every bodypart.
// FIXME when picking body, not mouving mouse but moving, bad cam->screendirection

#include "src/filesystem/be_filesystem.h"
#include "worldb.h"
#ifdef HAVE_OPENMP
	#include <omp.h>
#endif

void WorldB::CollisionNearOverride(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo)
{
	btRigidBody* b1 = static_cast<btRigidBody*>(collisionPair.m_pProxy0->m_clientObject);
	btRigidBody* b2 = static_cast<btRigidBody*>(collisionPair.m_pProxy1->m_clientObject);
	Entity* e1 = static_cast<Entity*>(b1->getUserPointer());
	Entity* e2 = static_cast<Entity*>(b2->getUserPointer());
	if(e1 == NULL || e2 == NULL)
	{
		//cout << "Entity cast error in collide callback." << endl;
		return dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
	}
// 	else if(e1->type == CRITTER && e2->type == CRITTER)
// 	{
		else if(e1 == e2)
		{
			//cout << "Not colliding." << endl;
			return;
		}
	//cout << "Colliding." << endl;
// 	}
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

WorldB::WorldB(  boost::shared_ptr<BeGraphicsSystem> system, boost::shared_ptr<Textverbosemessage> textverbosemessage )
 : m_camera(0.73f, 1024.0f/768.0f, 0.05f, 5000)
 , retinasperrow(119)
 , m_threads_last(1)
 , m_graphicsSystem(system)
 , m_panel_textverbosemessage(textverbosemessage)
 , mouseRayTo(0.0f, 0.0f, 0.0f)
//  , m_extinctionmode(false)
//  , m_extinctionmode_until(0)
{
	
	const unsigned int m_starttime(time(0));

	// hostname
	char hn[256] = "";
	gethostname(hn, sizeof(hn));

	std::stringstream ident;
	ident << hn << "(" << m_starttime << ")";
	m_starttime_s = ident.str();
	
// 	std::stringstream s;
// 	s << ident.str();
// 	m_starttime_s = s.str();

	m_camera.setSceneNode(&m_sceneNodeCamera);

	// settings and pointers
	settings = Settings::Instance();
// 		retinasperrow = settings->getCVarPtr("retinasperrow");
		critter_maxlifetime = settings->getCVarPtr("critter_maxlifetime");
		critter_maxenergy = settings->getCVarPtr("critter_maxenergy");
		critter_autosaveinterval = settings->getCVarPtr("critter_autosaveinterval");
		critter_autoexchangeinterval = settings->getCVarPtr("critter_autoexchangeinterval");
		population_double = settings->getCVarPtr("population_double");
		critter_retinasize = settings->getCVarPtr("critter_retinasize");
		critter_sightrange = settings->getCVarPtr("critter_sightrange");
		critter_raycastvision = settings->getCVarPtr("critter_raycastvision");
		critter_enableomnivores = settings->getCVarPtr("critter_enableomnivores");
		critter_startenergy = settings->getCVarPtr("critter_startenergy");

		brain_mutationrate = settings->getCVarPtr("brain_mutationrate");
		body_mutationrate = settings->getCVarPtr("body_mutationrate");

		food_maxlifetime = settings->getCVarPtr("food_maxlifetime");
		food_maxenergy = settings->getCVarPtr("food_maxenergy");
		energy = settings->getCVarPtr("energy");
		headless = settings->getCVarPtr("headless");
		threads = settings->getCVarPtr("threads");
		mincritters = settings->getCVarPtr("mincritters");
		insertcritterevery = settings->getCVarPtr("critter_insertevery");
		worldsizeX = settings->getCVarPtr("worldsizeX");
		worldsizeY = settings->getCVarPtr("worldsizeY");

		population_limit_energy = settings->getCVarPtr("population_limit_energy");
		population_limit_energy_percent = settings->getCVarPtr("population_limit_energy_percent");

		population_eliminate_portion = settings->getCVarPtr("population_eliminate_portion");
		population_eliminate_portion_percent = settings->getCVarPtr("population_eliminate_portion_percent");
		population_eliminate_portion_decrenergy = settings->getCVarPtr("population_eliminate_portion_decrenergy");
		population_eliminate_portion_incrworldsizeX = settings->getCVarPtr("population_eliminate_portion_incrworldsizeX");
		population_eliminate_portion_incrworldsizeY = settings->getCVarPtr("population_eliminate_portion_incrworldsizeY");
		population_eliminate_portion_decrmaxlifetimepct = settings->getCVarPtr("population_eliminate_portion_decrmaxlifetimepct");

		population_eliminate_portion_brainmutationratechange = settings->getCVarPtr("population_eliminate_portion_brainmutationratechange");
		population_eliminate_portion_brainmutationratemin = settings->getCVarPtr("population_eliminate_portion_brainmutationratemin");
		population_eliminate_portion_brainmutationratemax = settings->getCVarPtr("population_eliminate_portion_brainmutationratemax");
		population_eliminate_portion_bodymutationratechange = settings->getCVarPtr("population_eliminate_portion_bodymutationratechange");
		population_eliminate_portion_bodymutationratemin = settings->getCVarPtr("population_eliminate_portion_bodymutationratemin");
		population_eliminate_portion_bodymutationratemax = settings->getCVarPtr("population_eliminate_portion_bodymutationratemax");

		
	statsBuffer = Statsbuffer::Instance();
	critterselection = Critterselection::Instance();
	// home & program directory
	dirlayout = Dirlayout::Instance();

	m_freeEnergy = *food_maxenergy * *energy;
		
	currentCritterID	= 1;
	insertCritterCounter	= 0;
	foodIntervalCounter = 0;
	critterIntervalCounter = 0;
	autosaveCounter		= 0.0f;
	autoexchangeCounter	= 0.0f;
	insertHight		= 10.0f;
	pause = false;
	mouselook = false;
	mousex = -100;
	mousey = -100;

	
	// vision retina allocation
	items = 4 * 2048 * 2048;
	retina = (unsigned char*)malloc(items);
	memset(retina, 0, items);

	// THREADED BULLET
// 	int maxNumOutstandingTasks = 6;
// 	m_collisionConfiguration = new btDefaultCollisionConfiguration();
// 	PosixThreadSupport::ThreadConstructionInfo constructionInfo("collision", processCollisionTask, createCollisionLocalStoreMemory, maxNumOutstandingTasks);
// 	m_threadSupportCollision = new PosixThreadSupport(constructionInfo);
// 	m_dispatcher = new SpuGatheringCollisionDispatcher(m_threadSupportCollision,maxNumOutstandingTasks,m_collisionConfiguration);
// 	
// 	btVector3 worldAabbMin(-10000,-10000,-10000);
// 	btVector3 worldAabbMax(10000,10000,10000);
// 	m_broadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);
// 	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
// // 	m_solver = new btSequentialImpulseConstraintSolver;
// // 	m_solver = new SpuMinkowskiPenetrationDepthSolver();
// 	m_solver = new btParallelConstraintSolver();
// 	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
// 
// // 		m_dynamicsWorld->getSolverInfo().m_numIterations = 10;
// // 		m_dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_SIMD+SOLVER_USE_WARMSTARTING;
// 
// // 		m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	// stop threaded bullet

	// NOT THREADED
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	if(settings->getCVar("body_selfcollisions") == 0)
		m_dispatcher->setNearCallback(&WorldB::CollisionNearOverride);
	
	btVector3 worldAabbMin(-1000,-1000,-1000);
	btVector3 worldAabbMax(1000,1000,1000);
	m_broadphase = new btAxisSweep3 (worldAabbMin, worldAabbMax);
	m_ghostpaircallback = new btGhostPairCallback();
	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostpaircallback);
	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	// END NOT THREADED
	
// 	m_dynamicsWorld->setGravity( btVector3(0.0f, -50.0f, 0.0f) );

// 	m_dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_USE_WARMSTARTING | SOLVER_SIMD;
	m_dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_USE_WARMSTARTING | SOLVER_SIMD | SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;
	m_dynamicsWorld->getSolverInfo().m_numIterations = 8;

	// raycast
	raycast = new Raycast(m_dynamicsWorld);

	// mousepicker
	mousepicker = new BeMousePicker(m_dynamicsWorld);

	// determine vision width
	picwidth = retinasperrow * (*critter_retinasize+1);

	// reset cam
		resetCamera();

	debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe|btIDebugDraw::DBG_DrawWireframe|btIDebugDraw::DBG_DrawAabb|btIDebugDraw::DBG_DrawContactPoints|btIDebugDraw::DBG_DrawText|btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits);
// 	debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawConstraints+btIDebugDraw::DBG_DrawConstraintLimits);
// 	debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawConstraints);
// 	debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawConstraintLimits);
	m_dynamicsWorld->setDebugDrawer(&debugDrawer);

	// threading locks
// 	omp_init_lock(&my_lock1);
// 	omp_init_lock(&my_lock2);

	// create the needed ext bundings for fbos
	if ( !*headless )
	{
		// generate namespace for the frame buffer, colorbuffer and depthbuffer
		glGenFramebuffersEXT(1, &fb);
		glGenTextures(1, &color_tex); 
		glGenRenderbuffersEXT(1, &depth_rb);

		//switch to our fbo so we can bind stuff to it
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
		//create the colorbuffer texture and attach it to the frame buffer
		glBindTexture(GL_TEXTURE_2D, color_tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_INT, NULL);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0); 
		
		// create a render buffer as our depthbuffer and attach it
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 2048, 2048);
// 		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 2048, 2048);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
		
		// Go back to regular frame buffer rendering
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		
// 		//RGBA8 2D texture, 24 bit depth texture, 256x256
// 		glGenTextures(1, &color_tex);
// 		glBindTexture(GL_TEXTURE_2D, color_tex);
// // 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// // 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// // 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// // 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 		//NULL means reserve texture memory, but texels are undefined
// // 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
// 		//-------------------------
// 		glGenFramebuffersEXT(1, &fb);
// 		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
// 		//Attach 2D texture to this FBO
// 		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
// 		//-------------------------
// 		glGenRenderbuffersEXT(1, &depth_rb);
// 		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
// 		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 2048, 2048);
// 		//-------------------------
// 		//Attach depth buffer to FBO
// 		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
// 		//-------------------------




		//Does the GPU support current FBO configuration?
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status)
		{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
// 				std::cout << "FRAMEBUFFER : GOOD" << std::endl;
				break;
			default:
				std::cout << "FRAMEBUFFER : NOT GOOD" << std::endl;
				break;
		}
	}

}

void WorldB::init()
{
	makeFloor();

	if ( settings->getCVar("autoload") )
		loadAllCritters();
	if ( settings->getCVar("autoloadlastsaved") )
		loadAllLastSavedCritters();
}

void WorldB::castMouseRay()
{
// 	cout << "casting" << endl;

	mouseRay = raycast->cast( m_sceneNodeCamera.getOrigin(), mouseRayTo );
// 	mouseRay = raycast->cast( camera.position.getOrigin(), mouseRayTo );
	mouseRayHit = false;
	if ( mouseRay.m_hit )
	{
		if ( !( mouseRay.m_hitBody->isStaticObject() || mouseRay.m_hitBody->isKinematicObject() ) )
		{
			Entity* e = static_cast<Entity*>(mouseRay.m_hitBody->getUserPointer());
			if ( e->type == FOOD || e->type == CRITTER )
			{
				mouseRayHit = true;
				mouseRayHitEntity = e;
			}
		}
	}
}

void WorldB::calcMouseDirection()
{
// 	cout << "updating mouserayto" << endl;
	mouseRayTo = m_camera.getScreenDirection(mousex, mousey);
}

void WorldB::moveInMouseDirection(bool towards)
{
// 	cout << "updating mouserayto" << endl;
// 	if ( towards )
// 		camera.moveTowards(mouseRayTo.normalized());
// 	else
// 		camera.moveAwayFrom(mouseRayTo.normalized());
}

void WorldB::unpickBody()
{
	mousepicker->detach();
}

void WorldB::pickBody()
{
	mousepicker->detach();

	if ( mouseRayHit )
	{
		if ( mouseRayHitEntity->type == FOOD || mouseRayHitEntity->type == CRITTER )
		{
			const btRigidBody* b = static_cast<const btRigidBody*>(mouseRay.m_hitBody);

			// if critter, and it's the head's ghostobject we touch, overwrite with head's body
			if ( mouseRayHitEntity->type == CRITTER )
			{
				const btCollisionObject* co = static_cast<const btCollisionObject*>(b);
				CritterB* c = static_cast<CritterB*>(mouseRayHitEntity);
				if ( co == c->body.mouths[0]->ghostObject )
					b = c->body.mouths[0]->body;
			}
			
			btRigidBody* new_b(0);
			for( unsigned int j(0); j < entities.size() && new_b == 0; ++j)
			{	
				if ( entities[j]->type == FOOD )
				{
					Food* f = static_cast<Food*>( entities[j] );
					for( unsigned int i(0); i < f->body.bodyparts.size(); ++i)
					{
						if ( f->body.bodyparts[i]->body == b )
						{
							new_b = f->body.bodyparts[i]->body;
							break;
						}
					}
				}
			}
			for( unsigned int j=0; j < critters.size() && new_b == 0; ++j)
			{
				CritterB* f = critters[j];
				for( unsigned int i(0); i < f->body.bodyparts.size(); ++i)
				{
					if ( f->body.bodyparts[i]->body == b )
					{
						new_b = f->body.bodyparts[i]->body;
						break;
					}
				}
			}
			
			if ( new_b )
			{
				mousepicker->attach( new_b, mouseRay.m_hitPosition, m_sceneNodeCamera.getOrigin(), mouseRayTo );
// 				mousepicker->attach( new_b, mouseRay.m_hitPosition, m_sceneNodeCamera.getOrigin(), mouseRay.m_hitPosition );
				mousepicker->m_pickedBoolP = &mouseRayHitEntity->isPicked;
				*mousepicker->m_pickedBoolP = true;
	// 			cout << "picked body" << endl;
			}
		}
	}
}

void WorldB::selectBody()
{
	if ( mouseRayHit )
		if ( mouseRayHitEntity->type == CRITTER )
			critterselection->registerCritter(static_cast<CritterB*>(mouseRayHitEntity));
}

void WorldB::deselectBody()
{
	if ( mouseRayHit )
		if ( mouseRayHitEntity->type == CRITTER )
			critterselection->unregisterCritter(static_cast<CritterB*>(mouseRayHitEntity));
}

void WorldB::movePickedBodyTo()
{
	if ( mousepicker->m_active )
	{
		calcMouseDirection();
		mousepicker->moveTo( m_sceneNodeCamera.getOrigin(), mouseRayTo );
	}
}

void WorldB::movePickedBodyFrom()
{
	if ( mousepicker->m_active )
	{
		calcMouseDirection();
		mousepicker->moveFrom( m_sceneNodeCamera.getOrigin() );
	}
}

void WorldB::calcFreeEnergy()
{
	
	// determine percentage of energy we want to maintain
	
	// normal ratio when population is lower than limit
	float m_energy_ratio(1.0f);
	
	// if we are over the limit, use 10%
	if ( critters.size() >= (unsigned int)*population_limit_energy )
	{
		m_energy_ratio = 0.01f * *population_limit_energy_percent;
// 		std::cout << m_energy_ratio << std::endl;
	}
	
	m_freeEnergy = *food_maxenergy * *energy * m_energy_ratio;

	const auto& end(critters.end());
	for( auto it(critters.begin()); it != end; ++it)
	{
		m_freeEnergy -= (*it)->energyLevel;
	}

	const auto& e_end(entities.end());
	for( auto it(entities.begin()); it != e_end; ++it)
	{
		if ( (*it)->type == FOOD )
		{
			Food* f = static_cast<Food*>(*it);
			m_freeEnergy -= f->energyLevel;
		}
	}
	
	
}

void WorldB::process(const BeTimer& timer)
{
	if ( !pause )
	{
		// recalc freeEnergy
			calcFreeEnergy();
		
		#ifdef HAVE_OPENCL
// 			nbody.process();
		#endif
	
		// kill half?
			killHalforDouble();

		// Remove food
			expireFood();

		// Autoinsert Food
			autoinsertFood();

		// remove all dead critters
			expireCritters();

		// Autosave Critters?
			autosaveCritters(timer);

		// Autoexchange Critters?
			autoexchangeCritters(timer);

		// Autoinsert Critters?
			autoinsertCritters();

		if ( *critter_raycastvision == 0 )
		{
			renderVision();
			grabVision();
		}

		// do a bullet step
// 			m_dynamicsWorld->stepSimulation(0.016667f, 0, 0.016667f);


			m_dynamicsWorld->stepSimulation(0.016667f, 2, 0.00833334f);
			
			
			
	// 		m_dynamicsWorld->stepSimulation(Timer::Instance()->bullet_ms / 1000.f);
	// cout << Timer::Instance()->bullet_ms << " : " << Timer::Instance()->bullet_ms / 1000.f << endl;

			
			
			
			
			
			
			
			
		// process all critters
		unsigned int lmax = critters.size();
		unsigned int i;
		float freeEnergyc = 0.0f;

		
// 		for( i=0; i < lmax; ++i)
// 		{
// 			CritterB* c = critters[i];
// 			
// 				checkCollisions( c );
// 
// 			// process
// 				c->process();
// 
// 			// record critter used energy
// 				freeEnergyc += c->energyUsed;
// 
// 			// process Output Neurons
// 				eat(c);
// 
// 			// procreation if procreation energy trigger is hit
// 			procreate(c);
// 			c->beingTouched = false;
// 			c->beingEaten   = false;
// 		}
		
		
		
		for( i=0; i < lmax; ++i)
			checkCollisions( critters[i] );
		
#ifdef HAVE_OPENMP
// 		std::cout << "found" << std::endl;
	// process
		if ( m_threads_last != *threads )
		{
			m_threads_last = *threads;
			omp_set_num_threads(m_threads_last);
		}

		if ( m_threads_last > 1 )
		{
// 			unsigned int t_lmax = lmax;
			#pragma omp parallel for private(i)
			for( i=0; i < lmax; ++i)
			{
				critters[i]->process();
// 				freeEnergyc += c->energyUsed;
			}
		}
		else
#endif
		{
			for( i=0; i < lmax; ++i)
				critters[i]->process();
		}

		for( i=0; i < lmax; ++i)
		{
			CritterB* c = critters[i];

			// record critter used energy
				freeEnergyc += c->energyUsed;

			// process Output Neurons
				eat(c);

			// procreation if procreation energy trigger is hit
				procreate(c);
				
				c->beingTouched = false;
				c->beingEaten   = false;
		}
		
		m_freeEnergy += freeEnergyc;

		getGeneralStats();
	}
}
// 			omp_set_lock(&my_lock1);

void WorldB::childPositionOffset(btTransform* v)
{
	v->getOrigin().setY(insertHight);
}

void WorldB::procreate( CritterB* c )
{
	if ( c->procreate && c->canProcreate )
	{
		bool brainmutant = false;
		bool bodymutant = false;
		
		int brainMutationrate = *brain_mutationrate;
		int bodyMutationrate = *body_mutationrate;
		
		// mutationratemax - ((( numcritters-critterrange ) / critterrange) * mutationraterange)
		if ( *population_eliminate_portion_bodymutationratechange && *population_eliminate_portion_bodymutationratemax > *population_eliminate_portion_bodymutationratemin )
		{
			float critter_range = 0.5f * *population_eliminate_portion;
			const int mutation_range = *population_eliminate_portion_bodymutationratemax - *population_eliminate_portion_bodymutationratemin;
			brainMutationrate = *population_eliminate_portion_bodymutationratemax - ((((float)critters.size()-critter_range)/critter_range)*mutation_range);
			// control
			if ( brainMutationrate > *population_eliminate_portion_bodymutationratemax )
				brainMutationrate = *population_eliminate_portion_bodymutationratemax;
			if ( brainMutationrate < *population_eliminate_portion_bodymutationratemin )
				brainMutationrate = *population_eliminate_portion_bodymutationratemin;
// 			cout << "brain:" << brainMutationrate << endl;
		}
		
		if ( *population_eliminate_portion_brainmutationratechange && *population_eliminate_portion_brainmutationratemax > *population_eliminate_portion_brainmutationratemin )
		{
			float critter_range = 0.5f * *population_eliminate_portion;
			const int mutation_range = *population_eliminate_portion_brainmutationratemax - *population_eliminate_portion_brainmutationratemin;
			bodyMutationrate = *population_eliminate_portion_brainmutationratemax - ((((float)critters.size()-critter_range)/critter_range)*mutation_range);
			// control
			if ( bodyMutationrate > *population_eliminate_portion_brainmutationratemax )
				bodyMutationrate = *population_eliminate_portion_brainmutationratemax;
			if ( bodyMutationrate < *population_eliminate_portion_brainmutationratemin )
				bodyMutationrate = *population_eliminate_portion_brainmutationratemin;
// 			cout << "body:" << bodyMutationrate << endl;
		}

		if ( randgen->Instance()->get(1,1000) <= brainMutationrate )
			brainmutant = true;

		if ( randgen->Instance()->get(1,1000) <= bodyMutationrate )
			bodymutant = true;

		btTransform np = c->body.bodyparts[0]->myMotionState->m_graphicsWorldTrans;

		// position offset
		childPositionOffset(&np);
// 		np.setY(insertHight);


// 		if ( np.getX() > *worldsizeX/2 )
// 			np.setX(np.getX()-1.0f);
// 		else
// 			np.setX(np.getX()+1.0f);

		CritterB *nc = new CritterB(*c, currentCritterID++, np, brainmutant, bodymutant);
		//CritterB *nc = new CritterB(*c, currentCritterID++, findPosition(), mutant);

		// display message of birth
			stringstream buf;
			buf << setw(4) << c->critterID << " : " << setw(4) << nc->critterID;
			buf << " ad: " << setw(4) << nc->genotype->adamdist;
			buf << " n: " << setw(4) << nc->brain.totalNeurons << " s: " << setw(5) << nc->brain.totalSynapses;

			if ( brainmutant || bodymutant )
			{
				buf << " ";
				if ( brainmutant ) buf << "brain";
				if ( brainmutant && bodymutant ) buf << "+";
				if ( bodymutant ) buf << "body";
				buf << " mutant";
			}

			m_panel_textverbosemessage->addBirth(buf);

			if (*headless)
				cout << buf.str()<< endl;

		// split energies in half
			nc->energyLevel = c->energyLevel/2.0f;
			c->energyLevel -= nc->energyLevel;

		// reset procreation energy count
			critters.push_back( nc );
			nc->calcFramePos(critters.size()-1);
			
		// rejuvenate 20
			c->totalFrames = (c->totalFrames  * 4) / 5;
	}
}

void WorldB::eat( CritterB* c )
{
	if ( c->eat )
	{
		if ( c->touchingFood )
		{
			Food* f = static_cast<Food*>(c->touchedEntity);
			float eaten = *critter_maxenergy / 20.0f;
			if ( c->energyLevel + eaten > *critter_maxenergy )
				eaten -= (c->energyLevel + eaten) - *critter_maxenergy;
			if ( f->energyLevel - eaten < 0 )
				eaten = f->energyLevel;

			c->energyLevel += eaten;
			f->energyLevel -= eaten;
		}
		else if ( c->touchingCritter && *critter_enableomnivores )
		{
			CritterB* ct = static_cast<CritterB*>(c->touchedEntity);
			float eaten = *critter_maxenergy / 20.0f;
			if ( c->energyLevel + eaten > *critter_maxenergy )
				eaten -= (c->energyLevel + eaten) - *critter_maxenergy;
			if ( ct->energyLevel - eaten < 0 )
				eaten = ct->energyLevel;

			c->energyLevel += eaten;
			ct->energyLevel -= eaten;
			ct->eaten = true;  // This is unused. :P
			ct->beingEaten = true;
		}
	}
}

void WorldB::killHalforDouble()
{
	if ( *population_double != 0 && critters.size() <= (unsigned int)*population_double )
	{
		unsigned int number_of_critters = critters.size();
		for ( unsigned int c = 0; c < number_of_critters; c++ )
			duplicateCritter(c, false, false);
	}
	else if ( critters.size() >= (unsigned int)*population_eliminate_portion )
	{
		killHalfOfCritters();
		
		// reduce energy
// 		if ( *population_eliminate_portion_decrenergypct > 0 )
// 		{
// 			float dec_amount = ((*food_maxenergy * *energy) - *food_maxenergy) / *food_maxenergy;
// 			if ( dec_amount >= 0.0f )
// 			{
// 				int dec = (dec_amount / 100) * *population_eliminate_portion_decrenergypct;
// 				settings->setCVar("energy", *energy - dec );
// 				//*energy -= dec;
// 				freeEnergy -= dec * *food_maxenergy;
// 			}
// 		}

		if ( *population_eliminate_portion_decrenergy > 0 )
		{
			settings->setCVar("energy", *energy - *population_eliminate_portion_decrenergy );
			//*energy -= dec;
			m_freeEnergy -= *population_eliminate_portion_decrenergy * *food_maxenergy;
		}
		
		// increase worldsizes
		if ( *population_eliminate_portion_incrworldsizeX > 0 )
		{
			settings->increaseCVar("worldsizeX", *population_eliminate_portion_incrworldsizeX);
			makeFloor();
		}
		if ( *population_eliminate_portion_incrworldsizeY > 0 )
		{
			settings->increaseCVar("worldsizeY", *population_eliminate_portion_incrworldsizeY);
			makeFloor();
		}
		
		// decrease critter_maxlifetime
		if ( *population_eliminate_portion_decrmaxlifetimepct > 0 )
		{
			int dec = (*critter_maxlifetime / 100) * *population_eliminate_portion_decrmaxlifetimepct;
			settings->setCVar("critter_maxlifetime", *critter_maxlifetime-dec );
		}
	}
}

void WorldB::autoinsertCritters()
{
	
	// insert critter if < minimum
	if ( *mincritters > 0 )
	{
		const unsigned int interval( *critter_maxlifetime / *mincritters );
		if ( ++critterIntervalCounter >= interval / 10 )
		{
			unsigned int number_to_insert = 1;

			if ( interval == 0 )
				number_to_insert += *mincritters / *critter_maxlifetime;
			
			for ( unsigned int i(0); i < number_to_insert; ++i )
			{
				if ( critters.size() < (unsigned int)*mincritters )
					insertCritter();
			}
			critterIntervalCounter = 0;
		}
	}

// 	// insert critter if < minimum
// 	if ( critters.size() < (unsigned int)*mincritters )
// 		insertCritter();

	// insert critter if insertcritterevery is reached
	if ( *insertcritterevery > 0 )
	{
		if ( insertCritterCounter >= (unsigned int)*insertcritterevery )
		{
			insertCritter();
			insertCritterCounter = 0;
		}
		else
		{
			insertCritterCounter++;
		}
	}
}

void WorldB::autosaveCritters(const BeTimer& timer)
{
	if ( *critter_autosaveinterval > 0 )
	{
		autosaveCounter += timer.getSeconds();
		if ( autosaveCounter > *critter_autosaveinterval )
		{
			autosaveCounter = 0.0f;
			saveAllCritters();
		}
	}
}

void WorldB::autoexchangeCritters(const BeTimer& timer)
{
	if ( *critter_autoexchangeinterval > 0 )
	{
		autoexchangeCounter += timer.getSeconds();
		if ( autoexchangeCounter > *critter_autoexchangeinterval )
		{
			autoexchangeCounter = 0.0f;

			// determine exchange directory
			
			// save or load? :)
			const unsigned int mode = randgen->Instance()->get( 0, 10001 );
			
			// always load if critters == 0
			if ( critters.size() == 0 || mode < 5000 )
			{
				vector<string> files;
				dirH.listContentsFull(dirlayout->exchangedir, files);

// 				std::cout << std::endl << "exchange load: " << std::endl;
				
				if ( files.size() > 0 )
				{
					unsigned int t_sum( 0 );
					const unsigned int load_max( 1+ (files.size() / 10) );

					for ( int i = 0; i < (int)files.size() && t_sum < load_max; ++i )
					{
						const unsigned int loadf(randgen->Instance()->get( 0, files.size()-1 ));
						auto& f(files[loadf]);
						
	// 				for ( unsigned int i = 0; i < files.size() && t_sum < load_max; ++i )
	// 				{
	// 					auto& f(files[i]);

// 						std::cout << "exchange try: " << f << " ";

						if ( ( parseH.endMatches( ".cr", f ) || parseH.endMatches(".cr.bz2", f) ) && f.find(m_starttime_s) == std::string::npos )
						{
							BeFile befileCritter;
							if ( m_filesystem->load( befileCritter, f ) )
							{
								std::string content( befileCritter.getContent().str() );

								if ( !content.empty() )
								{
// 									std::cout << "OK" << std::endl;
									
									// remove the file
// 									std::cout << "loaded: " << f << std::endl;
									m_filesystem->rm( f );
			
									btTransform t;
									t.setIdentity();
									t.setOrigin(findPosition());
									
									CritterB *c = new CritterB(content, m_dynamicsWorld, t, retina);

									if ( !c->loadError)
									{
										critters.push_back( c );

										c->critterID = currentCritterID++;
										c->calcFramePos(critters.size()-1);

										// start energy
										m_freeEnergy -= c->energyLevel;
										
										++t_sum;
									}
									else
										delete c;
								}
// 								else
// 									std::cout << "NOT OK, load fail" << std::endl;
							}
						}
// 						else
// 							std::cout << "NOT OK" << std::endl;
						
						files.erase( files.begin()+loadf );
						i = -1;

					}
					if ( t_sum > 0 )
					{
						stringstream buf;
		// 				buf << "Loaded critters from " << exchangedir;
						buf << "autoexchange: loaded critters (" << t_sum << ")";
						if ( !*headless )
						{
							m_logBuffer->add(buf.str());
						}
						else
							cout << buf.str()<< endl;
					}
					else
						autoExchangeSaveCritter();
				}
			}
			else
			{
				autoExchangeSaveCritter();
			}
		}
	}
}

void WorldB::autoExchangeSaveCritter()
{
	if ( critters.size() > 0 )
	{
		// pick one to save
		const unsigned int savec(randgen->Instance()->get( 0, critters.size()-1 ));

		stringstream buf;
		buf << dirlayout->exchangedir << "/" << m_starttime_s << "." << time(0) << ".cr";
		const string& filename(buf.str());

		// save critter
		m_filesystem->save_bz2(filename, critters[savec]->genotype->saveGenotype());

		const string buf2("autoexchange: saved critter");
		if ( !*headless )
			m_logBuffer->add(buf2);
		else
			cout << buf2<< endl;

		removeCritter(savec);
	}
}

void WorldB::autoinsertFood()
{
// 	200     @     100    = 0.5			>   1/0.5 = 2
// 	200     @     32000  = 160			>   1/160
	
							
	if ( m_freeEnergy >= *food_maxenergy )
	{
		const int queue(m_freeEnergy / *food_maxenergy);

		const unsigned int interval( *food_maxlifetime / *energy );
		const unsigned int exp_interval_div = (float)interval / (((float)queue+10) / 10);

		if ( ++foodIntervalCounter >= exp_interval_div || queue <= 2 )
		{
			// std::cout  << exp_interval_div << " queue: " << m_freeEnergy / *food_maxenergy << std::endl;

			unsigned int number_to_insert = 1;

			if ( interval == 0 )
				number_to_insert += *energy / *food_maxlifetime;

			for ( unsigned int i(0); i < number_to_insert; ++i )
			{
				if ( m_freeEnergy >= *food_maxenergy )
				{
					insertRandomFood(1, *food_maxenergy);
					m_freeEnergy -= *food_maxenergy;

					foodIntervalCounter = 0;
				}
			}

		}
	}
}

void WorldB::expireCritters()
{
	for( unsigned int i=0; i < critters.size(); ++i)
	{
		// see if energy level isn't below 0 -> die, or die of old age
		if ( critters[i]->energyLevel <= 0.0f )
		{
			stringstream buf;
			if ( critters[i]->eaten )
				buf << setw(4) << critters[i]->critterID << " got eaten";
			else
				buf << setw(4) << critters[i]->critterID << " starved";
			m_panel_textverbosemessage->addDeath(buf);
			
			if (*headless)
				cout << buf.str()<< endl;

			removeCritter(i);
			i--;
		}
		// die of old age
		else if ( critters[i]->totalFrames > (unsigned int)*critter_maxlifetime )
		{
			stringstream buf;
			buf << setw(4) << critters[i]->critterID << " died of age";
			m_panel_textverbosemessage->addDeath(buf);

			if (*headless)
				cout << buf.str()<< endl;

			removeCritter(i);
			i--;
		}
		// die if y < 200
		else
		{
			btVector3 pos = critters[i]->body.bodyparts[0]->myMotionState->m_graphicsWorldTrans.getOrigin();
			
			if ( pos.getY() < -200.0f )
			{
				stringstream buf;
				buf << setw(4) << critters[i]->critterID << " fell in the pit";
				m_panel_textverbosemessage->addDeath(buf);

				if (*headless)
					cout << buf.str()<< endl;

				removeCritter(i);
				i--;
			}
		}
	}
}

void WorldB::expireFood()
{
// 	for( unsigned int i=0; i < food.size(); ++i)
	for( unsigned int i=0; i < entities.size(); ++i)
	{
		if ( entities[i]->type == FOOD )
		{
			Food* f = static_cast<Food*>(entities[i]);
			// food was eaten
			if ( f->energyLevel <= 0 )
			{
				m_freeEnergy += f->energyLevel;
				if ( f->isPicked )
					mousepicker->detach();
				delete f;
				entities.erase(entities.begin()+i);
				i--;
			}
			// old food, this should remove stuff from corners
			else if ( ++f->totalFrames >= (unsigned int)*food_maxlifetime )
			{
				m_freeEnergy += f->energyLevel;
				if ( f->isPicked )
					mousepicker->detach();
				delete f;
				entities.erase(entities.begin()+i);
				i--;
			}
			// die if y < 200
			else
			{
				btVector3 pos = f->body.bodyparts[0]->myMotionState->m_graphicsWorldTrans.getOrigin();

				if ( pos.getY() < -200.0f )
				{
					m_freeEnergy += f->energyLevel;
					if ( f->isPicked )
						mousepicker->detach();
					delete f;
					entities.erase(entities.begin()+i);
					i--;
				}
			}
		}
	}
}

void WorldB::getGeneralStats()
{
// 	settings->info_totalNeurons = 0;
// 	settings->info_totalSynapses = 0;
// 	settings->info_totalAdamDistance = 0;
// 	settings->info_totalBodyparts = 0;
// 	settings->info_totalWeight = 0;

// 	settings->info_critters = critters.size();
// 	settings->info_food = food.size();

// 	int info_totalNeurons = 0;
// 	int info_totalSynapses = 0;
// 	int info_totalAdamDistance = 0;
// 	int info_totalBodyparts = 0;
// 	int info_totalWeight = 0;
// 	CritterB* c;

// #pragma omp parallel for shared (info_totalNeurons, info_totalSynapses, info_totalAdamDistance, info_totalBodyparts, info_totalWeight, crittersize) private(i, c)
// 	for( unsigned int i=0; i < critters.size(); ++i )
// 	{
// 		c = critters[i];
// 		info_totalNeurons		+= c->brain.totalNeurons;
// 		info_totalSynapses		+= c->brain.totalSynapses;
// 		info_totalAdamDistance		+= c->genotype->adamdist;
// 		info_totalBodyparts		+= c->body.bodyparts.size();
// 		info_totalWeight		+= c->body.totalWeight;
// 	}
// 
// 	settings->info_totalNeurons		+= info_totalNeurons;
// 	settings->info_totalSynapses		+= info_totalSynapses;
// 	settings->info_totalAdamDistance	+= info_totalAdamDistance;
// 	settings->info_totalBodyparts		+= info_totalBodyparts;
// 	settings->info_totalWeight		+= info_totalWeight;

	statsBuffer->add( critters, entities );
}

void WorldB::checkCollisions( CritterB* c )
{
	// set inputs to false and recheck
		c->touchingFood = false;
		c->touchingCritter = false;

	if ( c->body.mouths.size() > 0 )
	{
		btBroadphasePairArray& pairArray = c->body.mouths[0]->ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for ( int i=0; i < numPairs; ++i )
		{
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];

			//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
			btBroadphasePair* collisionPair = m_dynamicsWorld->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
			if (!collisionPair)
				continue;

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			for ( int j = 0; j < manifoldArray.size(); j++ )
			{
				btPersistentManifold* manifold = manifoldArray[j];
				
				const btCollisionObject* object1 = static_cast<const btCollisionObject*>(manifold->getBody0());
				const btCollisionObject* object2 = static_cast<const btCollisionObject*>(manifold->getBody1());

				if ( object1->getUserPointer() == c && object2->getUserPointer() == c )
					continue;

				for ( int p = 0; p < manifold->getNumContacts(); p++ )
				{
					const btManifoldPoint &pt = manifold->getContactPoint(p);
					if ( pt.getDistance() < 0.0f )
					{
						void* Collidingobject;
						if ( object1->getUserPointer() != c && object1->getUserPointer() != 0 )
							Collidingobject = object1->getUserPointer();
						else if ( object2->getUserPointer() != c && object2->getUserPointer() != 0 )
							Collidingobject = object2->getUserPointer();
						else 
							continue;

						// Touching Food
						Entity* e = static_cast<Entity*>(Collidingobject);
						if ( e->type == FOOD )
						{
// 							cout << "touches food" << endl;
							c->touchingFood = true;
							c->touchedEntity = e;
							return;
						}
						// Touching Critter
						else if ( e->type == CRITTER )
						{
// 							cout << "touches critter" << endl;
							c->touchingCritter = true;
							c->touchedEntity = e;
              CritterB* ct = static_cast<CritterB*>(e);
              ct->beingTouched = true;
							return;
						}

/*						// Touching Food
						Food* f = static_cast<Food*>(Collidingobject);
						if ( f > -1 )
						{
							if ( f->type == 1 )
							{
								stop = true;
								c->touchingFood = true;
								c->touchedFoodID = f;
							}
							else
							{
								// Touching Critter
								CritterB* b = static_cast<CritterB*>(Collidingobject);
								if ( b->type == 0 )
								{
									stop = true;
									c->touchingCritter = true;
									c->touchedCritterID = b;
								}
							}
						}*/
					}
				}
			}
		}
	}
}

void WorldB::insertRandomFood(int amount, float energy)
{
	for ( int i=0; i < amount; ++i )
	{
		Food *f = new Food();
		f->energyLevel = energy;
		//f->resize();
		f->createBody( m_dynamicsWorld, findPosition() );
		
		entities.push_back( f );
	}
}

void WorldB::insertCritter()
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(findPosition());
	
	CritterB *c = new CritterB(m_dynamicsWorld, currentCritterID++, t, retina);
	critters.push_back( c );
	c->calcFramePos(critters.size()-1);

	// start energy
	m_freeEnergy -= c->energyLevel;
}

btVector3 WorldB::findPosition()
{
	return btVector3( (float)randgen->Instance()->get( 0, 100**worldsizeX ) / 100, insertHight, (float)randgen->Instance()->get( 0, 100**worldsizeY ) / 100 );
}

void WorldB::removeCritter(unsigned int cid)
{
	m_freeEnergy += critters[cid]->energyLevel;

	if ( critters[cid]->isPicked )
		mousepicker->detach();
	
	critterselection->unregisterCritterID(critters[cid]->critterID);
	critterselection->deselectCritter(critters[cid]->critterID);

	delete critters[cid];
	critters.erase(critters.begin()+cid);

	// update higher retina frame positions
	int c;
// 	omp_set_num_threads( *threads );
// 	#pragma omp parallel for private(c)
	for ( c = cid; c < (int)critters.size(); c++ )
		critters[c]->calcFramePos(c);
}

void WorldB::killHalfOfCritters()
{
// 	for ( unsigned int c = 0; c < critters.size(); c++ )
// 		removeCritter(c);

	// kill oldest
	if ( critters.size() > 0 )
	{
		const unsigned int eliminate_amount = (0.01f * critters.size()) * *population_eliminate_portion_percent;
		for ( unsigned int c = 0; c < eliminate_amount; c++ )
		{
			removeCritter(0);
		}
	}
}

void WorldB::renderVision()
{
	// render critter vision
	if ( !*critter_raycastvision )
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb); 
		glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 		const auto& end(critters.end());
// 		for( auto it(critters.begin()); it != end; ++it)
		for( unsigned int i=0; i < critters.size(); ++i)
		{
			CritterB* c( critters[i] );
			if ( c->body.mouths.size() > 0 )
			{
				c->place();
// 				drawWithinCritterSight( i );
				drawWithinCritterSight(c);
				
// // 				critters[i]->releaseFBO();
			}
		}
	}
}

void WorldB::grabVision()
{
	// Read pixels into retina
	if ( !*critter_raycastvision && !critters.empty() )
	{
		// determine height
		unsigned int picheight = *critter_retinasize;
		int rows = critters.size();
		while ( rows > retinasperrow )
		{
			picheight += *critter_retinasize;
			rows -= retinasperrow;
		}
// 		glReadBuffer(GL_BACK);
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glReadPixels(0, 0, picwidth, picheight, GL_RGBA, GL_UNSIGNED_BYTE, retina);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
// 	glBindTexture(GL_TEXTURE_2D, color_tex);
}

void WorldB::drawWithoutFaces()
{
// 	cout << "hi" << endl;
	drawfloor();

	const auto& end(critters.end());
	for( auto it(critters.begin()); it != end; ++it)
		(*it)->draw(false);
// 	for( unsigned int i=0; i < critters.size(); ++i)
// 		critters[i]->draw(false);

// 	for( unsigned int i=0; i < food.size(); ++i)
// 		food[i]->draw();
	const auto& e_end(entities.end());
	for( auto it(entities.begin()); it != e_end; ++it)
		(*it)->draw();
// 	for( unsigned int i=0; i < entities.size(); ++i)
// 		entities[i]->draw();
}


void WorldB::drawWithGrid()
{
// 	glClear(GL_STENCIL_BUFFER_BIT);
// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_BACK);
	drawfloor();

	const auto& end(critters.end());
	for( auto it(critters.begin()); it != end; ++it)
		(*it)->draw(true);
// 	for( unsigned int i=0; i < critters.size(); ++i)
// 		critters[i]->draw(true);

	
	drawfood();
// 	const auto& e_end(entities.end());
// 	for( auto it(entities.begin()); it != e_end; ++it)
// 	{
// // 		if ( (*it)->type == WALL )
// // 			(*it)->draw(false, false);
// // 		else
// 		if ( (*it)->type != WALL )
// 			(*it)->draw();
// 	}
	
	
	
// 	for( unsigned int i=0; i < entities.size(); ++i)
// 		entities[i]->draw();

// 	glDisable(GL_STENCIL_TEST);
// 	glDisable(GL_CULL_FACE);


// 	// SHADOWS
// 	if ( 1 == 1 )
// 	{
// 		btVector3 aabbMin,aabbMax;
// 		m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);
// 		aabbMin-=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
// 		aabbMax+=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
// 		btScalar mmatrix[16];
// 		btVector3 sundir = btVector3(0.3f, -1.0f, 0.3f);
// 
// 
// 		glDisable(GL_LIGHTING);
// 		glDepthMask(GL_FALSE);
// 		glDepthFunc(GL_LEQUAL);
// 		glEnable(GL_STENCIL_TEST);
// 		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
// 		glStencilFunc(GL_ALWAYS,1,0xFFFFFFFFL);
// 		glFrontFace(GL_CCW);
// 		glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
// 
// // 		for( unsigned int i=0; i < critters.size(); i++)
// // 		{
// // 			for ( unsigned int bp = 0; bp < critters[i]->body.bodyparts.size(); bp++ )
// // 			{
// // 				critters[i]->body.bodyparts[bp]->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(mmatrix);
// // 				drawShadow(mmatrix, sundir * critters[i]->body.bodyparts[bp]->myMotionState->m_graphicsWorldTrans.getBasis() ,critters[i]->body.bodyparts[bp]->shape, critters[i]->body.bodyparts[bp], aabbMin,aabbMax);
// // 			}
// // 		}
// 		for( unsigned int i=0; i < entities.size(); i++)
// 		{
// 			if ( entities[i]->type == FOOD )
// 			{
// 				Food* f = static_cast<Food*>(entities[i]);
// 				f->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(mmatrix);
// 				drawShadow(mmatrix,sundir * f->myMotionState->m_graphicsWorldTrans.getBasis(),f->body.bodyparts[0]->shape, f->body.bodyparts[0], aabbMin, aabbMax);
// 
// 			}
// 		}
// 
// 		glFrontFace(GL_CW);
// 		glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
// 
// 		for( unsigned int i=0; i < critters.size(); i++)
// 		{
// 			for ( unsigned int bp = 0; bp < critters[i]->body.bodyparts.size(); bp++ )
// 			{
// 				critters[i]->body.bodyparts[bp]->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(mmatrix);
// 				drawShadow(mmatrix, sundir * critters[i]->body.bodyparts[bp]->myMotionState->m_graphicsWorldTrans.getBasis() ,critters[i]->body.bodyparts[bp]->shape, critters[i]->body.bodyparts[bp], aabbMin,aabbMax);
// 			}
// 		}
// 		for( unsigned int i=0; i < entities.size(); i++)
// 		{
// 			if ( entities[i]->type == FOOD )
// 			{
// 				Food* f = static_cast<Food*>(entities[i]);
// 				f->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(mmatrix);
// 				drawShadow(mmatrix,sundir * f->myMotionState->m_graphicsWorldTrans.getBasis(),f->body.bodyparts[0]->shape, f->body.bodyparts[0], aabbMin, aabbMax);
// 
// 			}
// 		}
// 
// 		glDepthMask(GL_TRUE);
// 		glFrontFace(GL_CCW);
// 		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
// 		glDepthFunc(GL_LEQUAL);
// 		glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
// 		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
// 		glDisable(GL_LIGHTING);
// 
// // 		glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
// // 		for( unsigned int i=0; i < critters.size(); i++)
// // 			critters[i]->draw(false, true);
// // 
// 		glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
// 		for( unsigned int i=0; i < entities.size(); i++)
// 		{
// 			if ( entities[i]->type == FOOD )
// 				entities[i]->draw(false, true);
// 		}
// 
// 		glDisable(GL_STENCIL_TEST);
// 	}

	
	
	#ifdef HAVE_OPENCL
// 	nbody.draw();
	#endif

// 	m_dynamicsWorld->debugDrawWorld();
}

void WorldB::drawfloor()
{
// 	for( auto it(entities.begin()); it != entities.end(); ++it)
// 		if ( (*it)->type == WALL )
// 			(*it)->draw();

// 	glColor4f( m_wallColor.r(), m_wallColor.g(), m_wallColor.b(), m_wallColor.a() );
// 	for( unsigned int i(0); i < entities.size(); ++i)
// 	{
// 		Entity* e(entities[i]);
// 		if ( e->type == WALL )
// 			e->draw_no_color();
// 	}

	glColor4f( m_wallColor.r(), m_wallColor.g(), m_wallColor.b(), m_wallColor.a() );
	for( unsigned int i(0); i < entities.size(); ++i)
	{
		Entity* e(entities[i]);
		if ( e->type == WALL )
			e->draw(false, true);
	}
}

void WorldB::drawfood()
{
	bool switchedColor(false);
	const auto& e_size(entities.size());
	for( unsigned int i(0); i < e_size; ++i)
	{
		Entity* e = entities[i];
		if ( e->type == FOOD )
		{
			if ( !switchedColor )
			{
				e->draw(true, true);
				switchedColor=true;
			}
			else
				e->draw(false, false);
		}
	}
}

void WorldB::drawShadow(btScalar* m,const btVector3& extrusion,const btCollisionShape* shape, Bodypart* bp,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
	glPushMatrix(); 
	glMultMatrixf(m);
	if(shape->getShapeType() == UNIFORM_SCALING_SHAPE_PROXYTYPE)
	{
		const btUniformScalingShape* scalingShape = static_cast<const btUniformScalingShape*>(shape);
		const btConvexShape* convexShape = scalingShape->getChildShape();
		float	scalingFactor = (float)scalingShape->getUniformScalingFactor();
		btScalar tmpScaling[4][4]={ {scalingFactor,0,0,0}, {0,scalingFactor,0,0}, {0,0,scalingFactor,0}, {0,0,0,1} };
		drawShadow((btScalar*)tmpScaling,extrusion,convexShape,bp,worldBoundsMin,worldBoundsMax);
		glPopMatrix();
		return;
	}
	else if(shape->getShapeType()==COMPOUND_SHAPE_PROXYTYPE)
	{
		const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(shape);
		for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
		{
			btTransform childTrans = compoundShape->getChildTransform(i);
			const btCollisionShape* colShape = compoundShape->getChildShape(i);
			btScalar childMat[16];
			childTrans.getOpenGLMatrix(childMat);
			drawShadow(childMat,extrusion*childTrans.getBasis(),colShape,bp,worldBoundsMin,worldBoundsMax);
		}
	}
// 	else
// 	{
// 	//	bool useWireframeFallback = true;
// 		if (shape->isConvex())
// 		{
// 			ShapeCache* sc = bp->cache((btConvexShape*)shape);
// 			btShapeHull* hull =&sc->m_shapehull;
// 			glBegin(GL_QUADS);
// 			for(int i=0;i<sc->m_edges.size();++i)
// 			{			
// 				const btScalar		d=btDot(sc->m_edges[i].n[0],extrusion);
// 				if((d*btDot(sc->m_edges[i].n[1],extrusion))<0)
// 				{
// 					const int		q=	d<0?1:0;
// 					const btVector3&	a=	hull->getVertexPointer()[sc->m_edges[i].v[q]];
// 					const btVector3&	b=	hull->getVertexPointer()[sc->m_edges[i].v[1-q]];
// 					glVertex3f(a[0],a[1],a[2]);
// 					glVertex3f(b[0],b[1],b[2]);
// 					glVertex3f(b[0]+extrusion[0],b[1]+extrusion[1],b[2]+extrusion[2]);
// 					glVertex3f(a[0]+extrusion[0],a[1]+extrusion[1],a[2]+extrusion[2]);
// 				}
// 			}
// 			glEnd();
// 		}
// 
// 	}
	glPopMatrix();

}

void WorldB::drawWithinCritterSight(CritterB *c)
{
	if ( c->body.mouths.size() > 0 )
	{
		const btVector3& cposi = c->body.mouths[0]->myMotionState->m_graphicsWorldTrans.getOrigin();

		// draw everything in it's sight
		const float sightrange(0.1f * *critter_sightrange);

		drawfloor();

		bool switchedColor(false);

// 		for( auto it(entities.begin()); it != entities.end(); ++it)
		const auto& e_size(entities.size());
		for( unsigned int i=0; i < e_size; ++i)
		{
			Entity* e = entities[i];
			if ( e->type == FOOD )
			{
				Food* f = static_cast<Food*>(e);
				if ( cposi.distance( f->myMotionState->m_graphicsWorldTrans.getOrigin() ) < sightrange )
				{
					if ( !switchedColor )
					{
						switchedColor=true;
						e->draw(true, true);
					}
					else
						e->draw(false, false);
				}
			}
		}

// 		for( auto c_it(critters.begin()); c_it != critters.end(); ++c_it)
		const auto& c_size(critters.size());
		for( unsigned int j=0; j < c_size; ++j)
		{
			const CritterB* f = critters[j];

// 			switchedColor = false;
			
			for( auto it(f->body.bodyparts.begin()); it != f->body.bodyparts.end(); ++it)
// 			for( unsigned int b=0; b < f->body.bodyparts.size(); ++b)
			{
				const Bodypart* b = (*it);
				
				if ( cposi.distance( b->myMotionState->m_graphicsWorldTrans.getOrigin() ) < sightrange )
				{
// // 					if ( !switchedColor )
// 					{
// // 						switchedColor = true;
// 						glColor4f( f->genotype->bodyArch->color.r(), f->genotype->bodyArch->color.g(), f->genotype->bodyArch->color.b(), f->genotype->bodyArch->color.a() );
// 					}

					
					if ( f->genotype == c->genotype )
					{
						glColor4f( f->genotype->bodyArch->color.r(), f->genotype->bodyArch->color.g(), f->genotype->bodyArch->color.b(), 0.1f );
					}
					else
					{
						glColor4f( f->genotype->bodyArch->color.r(), f->genotype->bodyArch->color.g(), f->genotype->bodyArch->color.b(), 0.5f );
					}


					b->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(drawposition);
					glPushMatrix(); 
	// 					glMultMatrixf(drawposition);
	// 					glMultMatrixd(drawposition); // FIXME WATCHIT OK WENT TO DOUBLE HERE 
						Displaylists::Instance()->glMultiMatrix(drawposition);

						const btVector3& halfExtent = static_cast<const btBoxShape*>(b->shape)->getHalfExtentsWithMargin();
						glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);

						Displaylists::Instance()->call(0);

					glPopMatrix();
				}
			}
		}

		// MOUTHS
		switchedColor = false;
// 		for( auto c_it(critters.begin()); c_it != critters.end(); ++c_it)
		for( unsigned int j=0; j < c_size; ++j)
		{
			const CritterB* f = critters[j];

// 			if ( c->critterID != f->critterID )
			{
				for( auto m_it(f->body.mouths.begin()); m_it != f->body.mouths.end(); ++m_it)
// 				for( unsigned int b=0; b < f->body.mouths.size(); ++b)
				{
					const auto* m = (*m_it);
					if ( cposi.distance( m->ghostObject->getWorldTransform().getOrigin() ) < sightrange )
					{
						if ( !switchedColor )
						{
							glColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
							switchedColor = true;
						}
						
						m->ghostObject->getWorldTransform().getOpenGLMatrix(drawposition);
						
						glPushMatrix(); 
// 						glMultMatrixf(drawposition);
// 						glMultMatrixd(drawposition); // FIXME WATCHIT OK WENT TO DOUBLE HERE 
						
						Displaylists::Instance()->glMultiMatrix(drawposition);

								const btVector3& halfExtent = static_cast<const btBoxShape*>(m->shape)->getHalfExtentsWithMargin();
								glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);

								Displaylists::Instance()->call(1);

						glPopMatrix();
					}
				}
			}
		}
		
	}
}

// same as before, but with exponential optimisation FIXME EXCEPT IT AIN'T WORKING
// void WorldB::drawWithinCritterSight(unsigned int cid)
// {
// 	CritterB *c = critters[cid];
// 
// 	if ( c->body.mouths.size() > 0 )
// 	{
// 		btVector3 cposi = c->body.mouths[0]->myMotionState->m_graphicsWorldTrans.getOrigin();
// 
// 		// draw everything in it's sight
// 		const float sightrange = (float)*critter_sightrange/10;
// 
// 		drawfloor();
// 
// 		for( auto it(entities.begin()); it != entities.end(); ++it)
// 		{
// 			if ( (*it)->type == FOOD )
// 			{
// 				Food* f = static_cast<Food*>(*it);
// 				if ( cposi.distance( f->myMotionState->m_graphicsWorldTrans.getOrigin() ) < sightrange )
// 					f->draw();
// 			}
// 		}
// // 		cout << "prerecorded " << c->crittersWithinRange.size() <<  endl;
// 
// 		// first process prechecked crittersWithinRange vector
// 		for( unsigned int p=0; p < c->crittersWithinRange.size(); p++)
// 		{
// 			CritterB *f = critters[ c->crittersWithinRange[p] ];
// 
// 			glColor4f( f->genotype->bodyArch->color.r, f->genotype->bodyArch->color.g, f->genotype->bodyArch->color.b, f->genotype->bodyArch->color.a );
// 			for( auto it(f->body.bodyparts.begin()); it != f->body.bodyparts.end(); ++it)
// 			{
// 				(*it)->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(drawposition);
// 				Displaylists::Instance()->glMultiMatrix(drawposition);
// 				btVector3 halfExtent = static_cast<const btBoxShape*>((*it)->shape)->getHalfExtentsWithMargin();
// 				glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);
// 				Displaylists::Instance()->call(0);
// 			}
// 
// 			glColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
// 
// 			for( auto m_it(f->body.mouths.begin()); m_it != f->body.mouths.end(); ++m_it)
// 			{
// 				(*m_it)->ghostObject->getWorldTransform().getOpenGLMatrix(drawposition);
// 				Displaylists::Instance()->glMultiMatrix(drawposition);
// 				btVector3 halfExtent = static_cast<const btBoxShape*>((*m_it)->shape)->getHalfExtentsWithMargin();
// 				glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);
// 				Displaylists::Instance()->call(1);
// 			}
// 		}
// 		// clear crittersWithinRange vector
// 		c->crittersWithinRange.clear();
// 
// // 		cout << "not recorded " << endl;
// 		// now start from current critter to last, record new checks for later critters
// 		for( unsigned int j=cid; j < critters.size(); j++)
// 		{
// // 			cout << " checking distance of " << j << endl;
// 			CritterB *f = critters[j];
// 			
// 			// if the first head is within range, draw critters bodyparts and if not same critter, draw head.
// 			if ( c->critterID == f->critterID || cposi.distance( f->body.mouths[0]->ghostObject->getWorldTransform().getOrigin() ) < sightrange )
// 			{
// 				//draw bodies if within range
// 				glColor4f( f->genotype->bodyArch->color.r, f->genotype->bodyArch->color.g, f->genotype->bodyArch->color.b, f->genotype->bodyArch->color.a );
// 				for( auto it(f->body.bodyparts.begin()); it != f->body.bodyparts.end(); ++it)
// 				{
// // 					if ( cposi.distance( (*it)->myMotionState->m_graphicsWorldTrans.getOrigin() ) < sightrange )
// // 					{
// 					(*it)->myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(drawposition);
// 					Displaylists::Instance()->glMultiMatrix(drawposition);
// 					btVector3 halfExtent = static_cast<const btBoxShape*>((*it)->shape)->getHalfExtentsWithMargin();
// 					glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);
// 					Displaylists::Instance()->call(0);
// // 					}
// 				}
// 				//draw heads if within range, and not itself
// // 				if ( c->critterID != f->critterID )
// // 				{
// 					// record for future distance checks
// // 					f->crittersWithinRange.push_back(cid);
// 					c->crittersWithinRange.push_back(j);
// 					
// 					glColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
// 					for( auto m_it(f->body.mouths.begin()); m_it != f->body.mouths.end(); ++m_it)
// 					{
// 						(*m_it)->ghostObject->getWorldTransform().getOpenGLMatrix(drawposition);
// 						Displaylists::Instance()->glMultiMatrix(drawposition);
// 						btVector3 halfExtent = static_cast<const btBoxShape*>((*m_it)->shape)->getHalfExtentsWithMargin();
// 						glScaled(halfExtent[0], halfExtent[1], halfExtent[2]);
// 						Displaylists::Instance()->call(1);
// 					}
// 			}
// 		}
// 	}
// 	
// 	
// 	
// 	
// }

void WorldB::loadAllCritters()
{
	vector<string> files;
	dirH.listContentsFull(dirlayout->loaddir, files);

	for ( unsigned int i = 0; i < files.size(); ++i )
	{
		auto& f(files[i]);
		if ( parseH.endMatches(".cr", f) || parseH.endMatches(".cr.bz2", f) )
		{
			BeFile befileCritter;
			if ( m_filesystem->load( befileCritter, f ) )
			{
				
				stringstream buf;
				buf << "loading " << f;
				m_logBuffer->add(buf.str());
	// 			cout << buf.str() << endl;

				std::string content( befileCritter.getContent().str() );
				
				btTransform t;
				t.setIdentity();
				t.setOrigin(findPosition());
				CritterB *c = new CritterB(content, m_dynamicsWorld, t, retina);

				if ( !c->loadError)
				{
					critters.push_back( c );

					c->critterID = currentCritterID++;
					c->calcFramePos(critters.size()-1);

					// start energy
					m_freeEnergy -= c->energyLevel;
				}
				else
					delete c;
			}
		}
		
	}

	stringstream buf;
	buf << "Loaded critters from " << dirlayout->loaddir;
	m_logBuffer->add(buf.str());
	cout << buf.str() << endl;
}

void WorldB::loadAllLastSavedCritters() // FIXME overlap with previous function
{
	cout << "loading" << endl;
	vector<string> files;
	
	string filen = dirlayout->progdir;
	filen.append("/lastsaved");
	
	if ( boost::filesystem::exists(filen) )
	{
		BeFile befile;
		if ( m_filesystem->load( befile, filen ) )
		{
			std::string lastsaveddir;
// 			std::string content;
			std::string line;
			while ( befile.getLine(line) )
			{
				lastsaveddir.append(line);
				lastsaveddir.append("\n");
			}

	// 		fileH.open( filen, lastsaveddir ); 
			
			lastsaveddir = lastsaveddir.substr(0, lastsaveddir.length() - 1);
			
		// 	lastsaveddir.append("/");
			cout << lastsaveddir << endl;

			dirH.listContentsFull(lastsaveddir, files);

			cout << "files: " << files.size() << endl;
			for ( unsigned int i = 0; i < files.size(); ++i )
			{
				if ( parseH.endMatches( ".cr", files[i] ) || parseH.endMatches(".cr.bz2", files[i]) )
				{
					stringstream buf;
					buf << "loading " << files[i];
					m_logBuffer->add(buf.str());

					BeFile befileCritter;
					if ( m_filesystem->load( befileCritter, files[i] ) )
					{
						std::string content( befileCritter.getContent().str() );
						
						btTransform t;
						t.setIdentity();
						t.setOrigin(findPosition());
						CritterB *c = new CritterB(content, m_dynamicsWorld, t, retina);

						if ( !c->loadError)
						{
							critters.push_back( c );

							c->critterID = currentCritterID++;
							c->calcFramePos(critters.size()-1);

							// start energy
							m_freeEnergy -= c->energyLevel;
						}
						else
							delete c;
					}
				}
			}
			stringstream buf;
			buf << "Loaded critters from " << lastsaveddir;
			m_logBuffer->add(buf.str());
			cout << buf.str() << endl;
		}
	}
}

void WorldB::saveAllCritters()
{
	// determine save directory
	const string subprofiledir(dirlayout->savedir + "/default");
	
	stringstream buf;
	buf << subprofiledir << "/" << time(0);
	string subsavedir = buf.str();

	// make dirs
	if ( !dirH.exists(subprofiledir) )	dirH.make(subprofiledir);
	if ( !dirH.exists(subsavedir) )		dirH.make(subsavedir);

	for ( unsigned int i = 0; i < critters.size(); ++i )
	{
		// determine filename
		stringstream filename;
		filename << subsavedir << "/" << "critter" << i << ".cr";
	
		// save critters
// 		m_filesystem->save(filename.str(), critters[i]->genotype->saveGenotype());
		m_filesystem->save_bz2(filename.str(), critters[i]->genotype->saveGenotype());
// 		fileH.save(filename.str(), critters[i]->genotype->saveGenotype());
	}

	// save lastsaved file
	stringstream lastsaved;
	lastsaved << dirlayout->progdir << "/" << "lastsaved";
	
	m_filesystem->save( lastsaved.str(), subsavedir );
	cout << "saved " << lastsaved.str() << " with " << subsavedir << endl;

 	//cout << endl << "Saved critters to " << subsavedir << endl << endl;
	stringstream buf2;
	buf2 << "Saved critters to " << subsavedir;
	m_logBuffer->add(buf2.str());
	cout << buf2.str() << endl;

}

void WorldB::resetCamera()
{
	int biggest = *worldsizeX;
	if ( *worldsizeY > biggest )
		biggest = 1.4f**worldsizeY;

	m_sceneNodeCamera.setOrigin( btVector3( 0.5f**worldsizeX, 1.1f*biggest, 0.5f**worldsizeY) );
// 	camera.position.setRotation(btQuaternion(btVector3(1, 0, 0), btScalar(90)));
	m_sceneNodeCamera.pitch( -SIMD_HALF_PI ); // 1.5707f  (float)*energy/10


// 	camera.position = btVector3( -0.5f**worldsizeX, -1.3f*biggest, -0.5f**worldsizeY);
// 	camera.rotation = Vector3f( 90.0f,  0.0f, 0.0f);
}

// selected critter actions
int WorldB::findSelectedCritterID()
{
	if ( critterselection->selectedCritter == 0 )
		return -1;
	
	for ( unsigned int i=0; i < critters.size(); ++i )
		if ( critters[i]->critterID == critterselection->selectedCritter->critterID )
			return i;
	return -1;
}

int WorldB::findCritterID(unsigned int cid)
{
	for ( unsigned int i=0; i < critters.size(); ++i )
		if ( critters[i]->critterID == cid )
			return i;
	return -1;
}

void WorldB::removeSelectedCritter()
{
	int f(findSelectedCritterID());
	if ( f > -1 )
		removeCritter(f);
}

void WorldB::removeAllSelectedCritters()
{
	while ( critterselection->clist.size() > 0 )
		removeCritter( findCritterID(critterselection->clist[0]->critterID) );
}

void WorldB::duplicateCritter(int cid, bool brainmutant, bool bodymutant)
{
	if ( cid > -1 )
	{
		btTransform np = critters[cid]->body.bodyparts[0]->myMotionState->m_graphicsWorldTrans;

		// position offset
		childPositionOffset(&np);

		CritterB *nc = new CritterB(*critters[cid], currentCritterID++, np, brainmutant, bodymutant);

		// duplicate energy levels
		nc->energyLevel = *critter_startenergy;
		m_freeEnergy -= nc->energyLevel;

		critters.push_back( nc );
		nc->calcFramePos(critters.size()-1);
	}
}

void WorldB::duplicateSelectedCritter()
{
	duplicateCritter( findSelectedCritterID(), false, false );
}
void WorldB::spawnBrainMutantSelectedCritter()
{
	duplicateCritter( findSelectedCritterID(), true, false );
}
void WorldB::spawnBodyMutantSelectedCritter()
{
	duplicateCritter( findSelectedCritterID(), false, true );
}
void WorldB::spawnBrainBodyMutantSelectedCritter()
{
	duplicateCritter( findSelectedCritterID(), true, true );
}

void WorldB::feedSelectedCritter()
{
	int f(findSelectedCritterID());
	if ( f > -1 )
	{
		CritterB* c = critters[f];
		if ( c->energyLevel < *critter_startenergy )
		{
			float max_currentDiff = (float)*critter_startenergy - c->energyLevel;
			c->energyLevel += max_currentDiff;
			m_freeEnergy -= max_currentDiff;
		}
	}
}

void WorldB::resetageSelectedCritter()
{
	int f(findSelectedCritterID());
	if ( f > -1 )
		critters[f]->totalFrames = 0;
}

void WorldB::duplicateAllSelectedCritters()
{
	for ( unsigned int i=0; i < critterselection->clist.size(); i ++ )
		duplicateCritter( findCritterID(critterselection->clist[i]->critterID), false, false );
}
void WorldB::spawnBrainMutantAllSelectedCritters()
{
	for ( unsigned int i=0; i < critterselection->clist.size(); i ++ )
		duplicateCritter( findCritterID(critterselection->clist[i]->critterID), true, false );
}
void WorldB::spawnBodyMutantAllSelectedCritters()
{
	for ( unsigned int i=0; i < critterselection->clist.size(); i ++ )
		duplicateCritter( findCritterID(critterselection->clist[i]->critterID), false, true );
}
void WorldB::spawnBrainBodyMutantAllSelectedCritters()
{
	for ( unsigned int i=0; i < critterselection->clist.size(); i ++ )
		duplicateCritter( findCritterID(critterselection->clist[i]->critterID), true, true );
}

void WorldB::activateFood() const
{
	for( auto it(entities.begin()); it != entities.end(); ++it )
	{	
		if ( (*it)->type == FOOD )
		{
			Food* f = static_cast<Food*>( (*it) );
			for( auto bit(f->body.bodyparts.begin()); bit != f->body.bodyparts.end(); ++bit )
				(*bit)->body->activate();
		}
	}
}

void WorldB::makeFloor()
{
	makeDefaultFloor();
	activateFood();
}

void WorldB::makeDefaultFloor()
{
	m_wallColor.setR(1.0f*0.28f);
	m_wallColor.setG(1.0f*0.18f);
	m_wallColor.setB(1.0f*0.08f);
	m_wallColor.setA(0.0f);
// 	m_wallColor.setR(0.34f);
// 	m_wallColor.setG(0.25f);
// 	m_wallColor.setB(0.11f);
// 	m_wallColor.normalize();

// 	for ( unsigned int i=0; i < walls.size(); ++i )
// 		delete walls[i];
// 	walls.clear();
	for ( int i=0; i < (int)entities.size(); ++i )
	{
		if ( entities[i]->type == WALL )
		{
			delete entities[i];
			entities.erase(entities.begin()+i);
			i--;
		}
	}

	// Wall Constants
		const float WallWidth = 2.0f;
		const float WallHalfWidth = WallWidth/2.0f;
		const float WallHeight = 5.0f;
		const float WallHalfHeight = WallHeight/2.0f;

	// Ground Floor
		
		btTransform t;
		t.setIdentity();
		t.setOrigin(btVector3( *worldsizeX/2.0f, -WallHalfWidth, *worldsizeY/2.0f ));
		
		Wall* w = new Wall( *worldsizeX, WallWidth, *worldsizeY, t, m_dynamicsWorld );
		w->color.setR(m_wallColor.r());
		w->color.setG(m_wallColor.g());
		w->color.setB(m_wallColor.b());
		entities.push_back(w);
	
	if ( settings->getCVar("worldwalls") )
	{
		// Left Wall
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3( 0.0f-WallHalfWidth, WallHalfHeight-WallWidth, *worldsizeY/2.0f ));

			w = new Wall( WallWidth, WallHeight, *worldsizeY, t, m_dynamicsWorld );
			w->color.setR(m_wallColor.r());
			w->color.setG(m_wallColor.g());
			w->color.setB(m_wallColor.b());
			entities.push_back(w);
		}
		// Right Wall
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3( *worldsizeX+WallHalfWidth, WallHalfHeight-WallWidth, *worldsizeY/2.0f ));

			w = new Wall( WallWidth, WallHeight, *worldsizeY, t, m_dynamicsWorld );
			w->color.setR(m_wallColor.r());
			w->color.setG(m_wallColor.g());
			w->color.setB(m_wallColor.b());
			entities.push_back(w);
		}
		// Top Wall
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3( *worldsizeX/2.0f, WallHalfHeight-WallWidth, 0.0f-WallHalfWidth ));

			w = new Wall( *worldsizeX+(WallWidth*2), WallHeight, WallWidth, t, m_dynamicsWorld );
			w->color.setR(m_wallColor.r());
			w->color.setG(m_wallColor.g());
			w->color.setB(m_wallColor.b());
			entities.push_back(w);
		}
		// Bottom Wall
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3( *worldsizeX/2.0f, WallHalfHeight-WallWidth, *worldsizeY+WallHalfWidth ));

			w = new Wall( *worldsizeX+(WallWidth*2), WallHeight, WallWidth, t, m_dynamicsWorld );
			w->color.setR(m_wallColor.r());
			w->color.setG(m_wallColor.g());
			w->color.setB(m_wallColor.b());
			entities.push_back(w);
		}
	}
}

unsigned int WorldB::findEntityIndex( unsigned int number, entityType et )
{
  
	unsigned int found = 0;
	
	for ( unsigned int currentIndex = 0; currentIndex < entities.size(); currentIndex++ )
	{
		if ( entities[currentIndex]->type == et )
		{
			found ++;
			if ( found == number )
				return currentIndex;
		}
	}

	cout << "findEntityIndex: number too high" << endl;
	exit(1);

}

void WorldB::togglePause()
{
	pause = !pause;
}

// void WorldB::toggleSleeper()
// {
// 	sleeper.swap();
// }

void WorldB::toggleMouselook()
{
	mouselook = !mouselook;
	if ( mouselook )
	{
#ifdef _WIN32
		SDL_WarpMouse(0,0);
#endif
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(SDL_DISABLE);
		// clear remaining poll events
		{ SDL_Event e; while (SDL_PollEvent(&e)) {} };
		
		// release picked objects
		mousepicker->detach();
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}
}

WorldB::~WorldB()
{
// 	for ( unsigned int i=0; i < food.size(); ++i )		delete food[i];
	for ( unsigned int i=0; i < entities.size(); ++i )	delete entities[i];
	for ( unsigned int i=0; i < critters.size(); ++i )	delete critters[i];
// 	for ( unsigned int i=0; i < walls.size(); ++i )		delete walls[i];

	free(retina);
	
	//Delete resources
	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffersEXT(1, &depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fb);	
	
	
	delete raycast;
	delete mousepicker;
	
	delete m_dynamicsWorld;

	delete m_collisionConfiguration;
	delete m_dispatcher;
	delete m_ghostpaircallback;
	delete m_broadphase;
	delete m_solver;

// 	omp_destroy_lock(&my_lock1);
// 	omp_destroy_lock(&my_lock2);
}
