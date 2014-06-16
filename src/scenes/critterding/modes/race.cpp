#include "race.h"
#ifdef HAVE_OPENMP
	#include <omp.h>
#endif

WorldRace::WorldRace(  boost::shared_ptr<BeGraphicsSystem> system, boost::shared_ptr<Textverbosemessage> textverbosemessage )
 : WorldB( system, textverbosemessage )
{
}

void WorldRace::init()
{
	testcounter = 1;

	cerr << endl << "Initializing run " << testcounter << " ... " << endl;

	// insert Floor
		makeFloor();

	// autoload critters
	if ( settings->getCVar("autoload") )
		loadAllCritters();
	if ( settings->getCVar("autoloadlastsaved") )
		loadAllLastSavedCritters();

	const unsigned int mincritters(settings->getCVar("mincritters"));
	
	// insert first batch of critters
		for ( unsigned int i=critters.size(); i < mincritters; ++i  )
			insRandomCritter( i );

	// insert food
		for ( unsigned int i=0; i < mincritters; ++i  )
			insFood( i );

		framecounter = 0;
		haveWinner = false;

	cerr<< "Running" << " ... " << endl;
}

void WorldRace::process(const BeTimer& timer)
{
	if ( !pause )
	{
		autosaveCritters(timer);

		// do a bullet step
			m_dynamicsWorld->stepSimulation(0.016667f, 0, 0.016667f);
	// 		m_dynamicsWorld->stepSimulation(0.016667f);
	// 		m_dynamicsWorld->stepSimulation(Timer::Instance()->bullet_ms / 1000.f);

		// render critter vision, optimized for this sim
			renderVision();
		// Read pixels into retina
			grabVision();

		unsigned int lmax = critters.size();
		unsigned int i;

		for( i=0; i < lmax; ++i)
			checkCollisions( critters[i] );

	// process
#ifdef HAVE_OPENMP
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
			CritterB *c = critters[i];
			
// 			// process
// 				c->process();

			// process Output Neurons
				if ( c->eat && c->touchingFood )
				{
					Food* f = static_cast<Food*>(c->touchedEntity);
					float eaten = *critter_maxenergy / 100.0f;
					if ( c->energyLevel + eaten > *critter_maxenergy )
						eaten -= (c->energyLevel + eaten) - *critter_maxenergy;
					if ( f->energyLevel - eaten < 0 )
						eaten = f->energyLevel;

					c->energyLevel += eaten;
					f->energyLevel -= eaten;
					
					// if a food unit has no more energy left, we have a winner, the race is over
					if ( f->energyLevel  == 0.0f )
						haveWinner = true;
				}
		}

		framecounter++;

		if ( (haveWinner || framecounter >= (unsigned int)settings->getCVar("critter_maxlifetime")) )
		{
			if ( haveWinner )
				cerr << "we have a WINNER after " << framecounter << " frames" << endl;

			cerr << "Evaluating..." << endl;

				// measure their distances from their respective food targets
					for ( unsigned int i=0; i < critters.size(); i++  )
					{
						// fitness function 1: distance to food cube
							btDefaultMotionState* cmyMotionState = (btDefaultMotionState*)critters[i]->body.mouths[0]->body->getMotionState();
							btVector3 cposi = cmyMotionState->m_graphicsWorldTrans.getOrigin();

// 							btDefaultMotionState* fmyMotionState = (btDefaultMotionState*)food[i]->body.bodyparts[0]->body->getMotionState();
// 							btVector3 fposi = fmyMotionState->m_graphicsWorldTrans.getOrigin();

							// find food cube number i
							unsigned int foodN = findEntityIndex( i+1, FOOD );
							
							Food* f = static_cast<Food*>( entities[foodN] );
							btDefaultMotionState* fmyMotionState = (btDefaultMotionState*)f->body.bodyparts[0]->body->getMotionState();
							btVector3 fposi = fmyMotionState->m_graphicsWorldTrans.getOrigin();

							critters[i]->fitness_index =  1.0f /(cposi.distance(fposi) + 0.0000001); 
						
						// fitness function 2: energy of food consumed
							critters[i]->fitness_index += ( (float)settings->getCVar("food_maxenergy") /(f->energyLevel + 0.0000001));

					}

				// initialize sort indices
					vector<int> indices ( critters.size(), 0 );
					for ( unsigned int i = 0; i < critters.size(); i++ )
						indices[i] = i;
		
				// sort results
					for ( int i = critters.size(); i>0; i--  )
						for ( int j = 0; j < i-1; j++  )
							if ( critters[indices[j]]->fitness_index < critters[indices[j+1]]->fitness_index )
							{
								unsigned keepI	= indices[j];
								indices[j]	= indices[j+1];
								indices[j+1]	= keepI;
							}

				// display results
					for ( unsigned int i=0; i < critters.size(); i++  )
						cerr << "c " << indices[i] << " : " << critters[indices[i]]->fitness_index << endl;

			cerr << endl << "Initializing run " << ++testcounter << " ... " << endl;

				// backup the 50% best critters
					vector<CritterB*> best;
					unsigned int bestNum = critters.size()/2;
					if ( critters.size() == 1 )
						bestNum = 1;
					
					btTransform t;
					t.setIdentity();

					for ( unsigned int i=0; i < bestNum; i++  )
						best.push_back( new CritterB(*critters[indices[i]], critters[indices[i]]->critterID, t, false, false) );
					
				// remove critters and food
					for ( unsigned int i=0; i < critters.size(); i++ )
					{
						stringstream buf;
						buf << setw(4) << critters[i]->critterID << " old";
						m_panel_textverbosemessage->addDeath(buf);

						if ( critters[i]->isPicked )
							mousepicker->detach();
	// FIXME on windows, we segfault here 1/10 after the first run
						critterselection->unregisterCritterID(critters[i]->critterID);
						critterselection->deselectCritter(critters[i]->critterID);
						delete critters[i];
	// FIXME
					}
					critters.clear();

// 					for ( unsigned int i=0; i < food.size(); i++ )
// 					{
// 						if ( food[i]->isPicked )
// 							mousepicker->detach();
// 						delete food[i];
// 					}
// 					food.clear();
					for ( unsigned int i=0; i < entities.size(); i++ )
					{
						if ( entities[i]->type == FOOD )
						{
							Food* f = static_cast<Food*>( entities[i] );
							if ( f->isPicked )
								mousepicker->detach();
							delete f;
							entities.erase(entities.begin()+i);
							i--;
						}
					}
// 					food.clear();

				// clear floor and remake it
					makeFloor();

					const unsigned int mincritters(settings->getCVar("mincritters"));
				// reinsert the best critters
					for ( unsigned int i=0; i < best.size() && i < mincritters; i++  )
						insMutatedCritter( *best[i], critters.size(), best[i]->critterID, false, false );

				// insert the mutants
					unsigned int count = 0;
					while ( critters.size() < mincritters )
					{
						if ( best.size() > 0 )
						{
							bool brainmutant = false;
							bool bodymutant = false;
							if ( randgen->Instance()->get(1,1000) <= settings->getCVar("brain_mutationrate") )
								brainmutant = true;

							if ( randgen->Instance()->get(1,1000) <= settings->getCVar("body_mutationrate") )
								bodymutant = true;

							insMutatedCritter( *best[count], critters.size(), currentCritterID++, brainmutant, bodymutant );

							CritterB* c = best[count];
							CritterB* nc = critters[critters.size()-1];
							stringstream buf;
							buf << setw(4) << c->critterID << " : " << setw(4) << nc->critterID;
							buf << " ad: " << setw(4) << nc->genotype->adamdist;
							buf << " n: " << setw(4) << nc->brain.totalNeurons << " s: " << setw(5) << nc->brain.totalSynapses;

							count++;
							if ( count == best.size() && count > 0 )
								count = 0;

							if ( brainmutant || bodymutant )
							{
								buf << " ";
								if ( brainmutant ) buf << "brain";
								if ( brainmutant && bodymutant ) buf << "+";
								if ( bodymutant ) buf << "body";
								buf << " mutant";
							}

							m_panel_textverbosemessage->addBirth(buf);
						}
						else
							insRandomCritter( critters.size() );
					}

				// remove best again
					for ( unsigned int i=0; i < best.size(); i++  )
						delete best[i];

				// reinsert respective food units
					for ( unsigned int i=0; i < mincritters; i++  )
						insFood( i );

				framecounter = 0;
				haveWinner = false;

				cerr << "Running... " << endl;
		}
		getGeneralStats();
	}
}

void WorldRace::makeFloor()
{
// 	for ( unsigned int i=0; i < walls.size(); i++ )	
// 		delete walls[i];
// 	walls.clear();
	for ( int i=0; i < (int)entities.size(); i++ )
	{
		if ( entities[i]->type == WALL )
		{
			delete entities[i];
			entities.erase(entities.begin()+i);
			i--;
		}
	}

	const unsigned int mincritters(settings->getCVar("mincritters"));
	critterspacing = (float)settings->getCVar("worldsizeX") / mincritters;

	makeDefaultFloor();

	// seperator walls
		float WallWidth = 2.0f;
		float WallHalfWidth = WallWidth/2.0f;
		float WallHeight = 5.0f;
		float WallHalfHeight = WallHeight/2.0f;

		for ( unsigned int i=1; i < mincritters; i++  )
		{
			btVector3 position = btVector3 ( 0.0f-WallHalfWidth + (critterspacing*i), WallHalfHeight-WallWidth, settings->getCVar("worldsizeY")/2.0f );
			
			btTransform t;
			t.setIdentity();
			t.setOrigin(position);

			Wall* w = new Wall( WallWidth, WallHeight, settings->getCVar("worldsizeY"), t, m_dynamicsWorld );
			w->color.setR(0.34f);
			w->color.setG(0.25f);
			w->color.setB(0.11f);
			entities.push_back(w);
		}
		
	activateFood();
}

void WorldRace::insRandomCritter(int nr)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3( (critterspacing/2)+(critterspacing*nr), 3.0f, settings->getCVar("worldsizeY")-8.0f ));
				
	CritterB *c = new CritterB(m_dynamicsWorld, currentCritterID++, t, retina);
	c->energyLevel = settings->getCVar("critter_maxenergy") / 2;
	critters.push_back( c );
	c->calcFramePos(critters.size()-1);
}

void WorldRace::insMutatedCritter(CritterB& other, int nr, unsigned int id, bool mutateBrain, bool mutateBody)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3( (critterspacing/2)+(critterspacing*nr), 3.0f, settings->getCVar("worldsizeY")-8.0f ));
	
	CritterB *nc = new CritterB(other, id, t, mutateBrain, mutateBody);
	nc->energyLevel = settings->getCVar("critter_maxenergy") / 2;
	critters.push_back( nc );
	nc->calcFramePos(critters.size()-1);
}

void WorldRace::insFood(int nr)
{
	Food *f = new Food;
	f->energyLevel = settings->getCVar("food_maxenergy");
	f->createBody( m_dynamicsWorld, btVector3( (critterspacing/2)+(critterspacing*nr), 3.0f, 8.0f ) );
	entities.push_back( f );
}

void WorldRace::insertCritter()
{
	cerr << "inserting critters is disabled during race" << endl;
}

void WorldRace::loadAllCritters()
{
	if ( critters.size() > 0 )
	{
		stringstream buf;
		buf << "use --autoload 1 at commandline to autoload critters into a race";
// 		Logbuffer::Instance()->add(buf);
		cerr << "use --autoload 1 at commandline to autoload critters into a race" << endl;
	}
	else
	{
		vector<string> files;
		dirH.listContentsFull(dirlayout->loaddir, files);

		const unsigned int mincritters(settings->getCVar("mincritters"));
		unsigned int inserted = 0;
		for ( unsigned int i = 0; i < files.size() && inserted < mincritters; i++ )
		{
			auto& f( files[i] );
			
			if ( parseH.endMatches( ".cr", f ) || parseH.endMatches(".cr.bz2", f) )
			{
				BeFile befileCritter;
				if ( m_filesystem->load( befileCritter, f ) )
				{
					std::string content( befileCritter.getContent().str() );
					
					critterspacing = (float)settings->getCVar("worldsizeX") / settings->getCVar("mincritters");
					
					btTransform t;
					t.setIdentity();
					t.setOrigin(btVector3( (critterspacing/2)+(critterspacing*critters.size()), 1.0f, settings->getCVar("worldsizeY")-(settings->getCVar("worldsizeY")/4) ));
					
					CritterB *c = new CritterB(content, m_dynamicsWorld, t, retina);

					unsigned int error = 0;
					if ( c->genotype->bodyArch->retinasize != (unsigned int)*critter_retinasize )
						error = 1;

					if ( !error)
					{
						critters.push_back( c );

						c->critterID = currentCritterID++;
						c->calcFramePos(critters.size()-1);
						c->energyLevel = settings->getCVar("critter_maxenergy") / 2;
						inserted++;
					}
					else
					{
						if ( error == 1 )
						{
							cerr << "ERROR: critter retinasize (" << c->genotype->bodyArch->retinasize << ") doesn't fit world retinasize (" << *critter_retinasize << ")" << endl;
						}
						delete c;
					}
				}
			}
		}
		cerr << endl << "Loaded critters from " << dirlayout->loaddir << endl << endl;
	}
}

WorldRace::~WorldRace()
{
}
