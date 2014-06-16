#include "roundworld.h"
#ifdef HAVE_OPENMP
	#include <omp.h>
#endif

Roundworld::Roundworld(  boost::shared_ptr<BeGraphicsSystem> system, boost::shared_ptr<Textverbosemessage> textverbosemessage )
 : WorldB( system, textverbosemessage )
{
}

void Roundworld::init()
{
	//makeFloor();

	groundTransform.setIdentity();
	groundTransform.setOrigin( btVector3(0,0,0) );

	groundShape = new btSphereShape(btScalar( *worldsizeX ));
	
	fixedGround = new btCollisionObject();
	fixedGround->setUserPointer(this);
	fixedGround->setCollisionShape(groundShape);
	fixedGround->setWorldTransform(groundTransform);
	m_dynamicsWorld->addCollisionObject(fixedGround);
	
	if ( settings->getCVar("autoload") )
		loadAllCritters();
	if ( settings->getCVar("autoloadlastsaved") )
		loadAllLastSavedCritters();
}

void Roundworld::process(const BeTimer& timer)
{
	if ( !pause )
	{
		// recalc freeEnergy
			calcFreeEnergy();

		killHalforDouble();
		expireFood();
		autoinsertFood();
		expireCritters();
		autoexchangeCritters(timer);
		autosaveCritters(timer);
		autoinsertCritters();
		// adjust gravity vectors of all entities' rigid bodies
		unsigned int j, b;
// 		Food* f;
// 		CritterB* bod;
		btRigidBody* bo;
		
		for( j=0; j < entities.size(); j++)
		{	
			if ( entities[j]->type == FOOD )
			{
// 				f = food[j];
				Food* f = static_cast<Food*>( entities[j] );
				for( b=0; b < f->body.bodyparts.size(); b++)
				{
					bo = f->body.bodyparts[b]->body;
					bo->setGravity( -(bo->getCenterOfMassPosition().normalized()*10) );
				}
			}
		}
		for( j=0; j < critters.size(); j++)
		{
			CritterB* bod = critters[j];
			for( b=0; b < bod->body.bodyparts.size(); b++)
			{
				bo = bod->body.bodyparts[b]->body;
				bo->setGravity( -(bo->getCenterOfMassPosition().normalized()*10) );
			}
			
			// catch by Ethical
			for( b=0; b < bod->body.mouths.size(); b++)
			{
				bo = bod->body.mouths[b]->body;
				bo->setGravity( -(bo->getCenterOfMassPosition().normalized()*10) );
			}
		}
		
		if ( *critter_raycastvision == 0 )
		{
			renderVision();
			grabVision();
		}

		// do a bullet step
			m_dynamicsWorld->stepSimulation(0.016667f, 0, 0.016667f);
	// 		m_dynamicsWorld->stepSimulation(Timer::Instance()->bullet_ms / 1000.f);

		unsigned int i;
		const unsigned int lmax = (int)critters.size();
// 		CritterB *c;
		float freeEnergyc = 0.0f;

// 		// FIXME USE FROM WORLDB
// 		for( int i=0; i < lmax; i++)
// 		{
// 			c = critters[i];
// 			
// 				checkCollisions(  c );
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
// 				procreate(c);
// 		}

		
		
		for( i=0; i < lmax; ++i)
			checkCollisions( critters[i] );
		
#ifdef HAVE_OPENMP
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

void Roundworld::makeFloor()
{
	m_dynamicsWorld->removeCollisionObject(fixedGround);
	delete groundShape;
	delete fixedGround;

	groundShape = new btSphereShape(btScalar( *worldsizeX ));
	
	fixedGround = new btCollisionObject();
	fixedGround->setUserPointer(this);
	fixedGround->setCollisionShape(groundShape);
	fixedGround->setWorldTransform(groundTransform);
	m_dynamicsWorld->addCollisionObject(fixedGround);

	activateFood();
}


void Roundworld::drawWithGrid()
{
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glPushMatrix(); 
		glTranslatef(0,0,0);
		glColor4f( 0.07f, 0.045f, 0.02f, 0.0f );
		drawSphere(
			*worldsizeX,
			(12.56637* *worldsizeX / 16),
			(12.56637* *worldsizeX / 8)
		);
	glPopMatrix(); 
	for( unsigned int i=0; i < critters.size(); i++)
		critters[i]->draw(true);

	for( unsigned int i=0; i < entities.size(); i++)
		entities[i]->draw();

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);}

void Roundworld::childPositionOffset(btTransform* v)
{
// 	*v+= (v->normalized()*insertHight);
	
// 	t.setOrigin( o.getOrigin().normalized() * 3.0f );
	
	
	// offset by one up
	btTransform o(*v);
	btTransform t;
	t.setIdentity();
	t.setOrigin( o.getOrigin().normalized() * insertHight );
	
	*v = t * o;
	
	
}

btVector3 Roundworld::findPosition()
{
	return btVector3(	((float)randgen->Instance()->get( 0, 200 )-100.0f) / 100,
				((float)randgen->Instance()->get( 0, 200 )-100.0f) / 100,
				((float)randgen->Instance()->get( 0, 200 )-100.0f) / 100
	).normalized() * ( *worldsizeX + insertHight);
}

void Roundworld::drawfloor()
{
	glPushMatrix(); 
		glTranslatef(0,0,0);
		glColor4f( 0.07f, 0.045f, 0.02f, 0.0f );
		drawSphere(
			*worldsizeX,
			(12.56637* *worldsizeX / 30),
			(12.56637* *worldsizeX / 15)
		);
	glPopMatrix(); 
}

void Roundworld::drawSphere(btScalar radius, int lats, int longs) 
{
	int i, j;
	for(i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0  = radius*sin(lat0);
		btScalar zr0 =  radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
		}
		glEnd();
	}
}

Roundworld::~Roundworld()
{
	m_dynamicsWorld->removeCollisionObject(fixedGround);
	delete groundShape;
	delete fixedGround;
}
