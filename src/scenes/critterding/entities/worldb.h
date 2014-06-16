#ifndef WORLDB_H
#define WORLDB_H

// #include "BulletMultiThreaded/PosixThreadSupport.h"
// #include "BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h"
// #include "BulletMultiThreaded/SpuGatheringCollisionDispatcher.h"
// #include "BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuMinkowskiPenetrationDepthSolver.h"
// #include "BulletMultiThreaded/btParallelConstraintSolver.h"
#include "btBulletDynamicsCommon.h"
#include <iomanip>

#include <GL/glew.h>

#ifdef HAVE_OPENCL
// 	#include "clnbody.h"
// 	#include "../../cl/clcontext.h"
#endif

#include "filesystem/be_dir.h"
#include "common/be_parser.h"
#include "utils/settings.h"
#include "common/be_mouse_picker.h"
#include "statsbuffer.h"
#include "gui/logbuffermessage.h"
// #include "utils/logbuffer.h"
#include "dirlayout.h"
#include "common/be_rand.h"
#include "common/be_frame_limiter.h"

#include "../gui/textverbosemessage.h"

#include "critterselection.h"
#include "food.h"
#include "wall.h"
#include "critterb.h"
#include "graphics/be_camera.h"
#include "graphics/be_graphics_system.h"

#include "common/be_physics_debug_renderer.h"

using namespace std;

class WorldB
{
	class	btThreadSupportInterface*		m_threadSupportCollision;
	class	btThreadSupportInterface*		m_threadSupportSolver;

	public:

		BePhysicsDebugRenderer	debugDrawer;
		WorldB( boost::shared_ptr<BeGraphicsSystem> system, boost::shared_ptr<Textverbosemessage> textverbosemessage );

		virtual			~WorldB();
		virtual void		init();
		virtual void		process(const BeTimer& timer);
		virtual btVector3	findPosition();
		virtual			void makeFloor();
		void			activateFood() const;
		bool 			pause;
		bool			mouselook;

		float			insertHight;
		void			getGeneralStats();
		void			killHalforDouble();
		void			expireFood();
		void			expireCritters();
		void			autoinsertFood();
		void			autosaveCritters(const BeTimer& timer);
		void			autoexchangeCritters(const BeTimer& timer);
		void			autoExchangeSaveCritter();
		void			autoinsertCritters();
		void			eat( CritterB* c );
		void			procreate( CritterB* c );
		void			makeDefaultFloor();
		
		Settings*		settings;
		Critterselection	*critterselection;
		Statsbuffer*		statsBuffer;
		BeRand*			randgen;
		BeMousePicker*		mousepicker;
		Dirlayout*		dirlayout;
// 		BeCamera		camera;
		BeCameraPerspective	m_camera;
		BeSceneNode		m_sceneNodeCamera;
		void resetCamera();
		unsigned int		findEntityIndex( unsigned int number, entityType et );

		btCollisionDispatcher*	m_dispatcher;
		btBroadphaseInterface*	m_broadphase;
		btGhostPairCallback*	m_ghostpaircallback;
		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btDynamicsWorld*	m_dynamicsWorld;
		btConstraintSolver*	m_solver;
		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

		vector<CritterB*>	critters;
		vector<Entity*>		entities;
// 		vector<Food*>		food_units;

		unsigned long		currentCritterID;

		virtual void		drawWithGrid();
		virtual void		drawWithoutFaces();
 		void			drawShadow(btScalar* m,const btVector3& extrusion,const btCollisionShape* shape, Bodypart* bp,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);
		
		void			drawWithinCritterSight(CritterB *c);
// 		void			drawWithinCritterSight(unsigned int cid);

		virtual void		drawfloor();
		virtual void		childPositionOffset(btTransform* v);
		virtual void		drawfood();

		void			startfoodamount(unsigned int amount);

		void			insertRandomFood(int amount, float energy);

		virtual void		insertCritter();
		void			saveAllCritters();
		virtual void		loadAllCritters();
		virtual void		loadAllLastSavedCritters();
		BeFrameLimiter		framelimiter;

		void			removeSelectedCritter();
		void			removeAllSelectedCritters();
		void			duplicateCritter(int cid, bool brainmutant, bool bodymutant);

		void			duplicateSelectedCritter();
		void			spawnBrainMutantSelectedCritter();
		void			spawnBodyMutantSelectedCritter();
		void			spawnBrainBodyMutantSelectedCritter();
		void			feedSelectedCritter();
		void			resetageSelectedCritter();

		void			duplicateAllSelectedCritters();
		void			spawnBrainMutantAllSelectedCritters();
		void			spawnBodyMutantAllSelectedCritters();
		void			spawnBrainBodyMutantAllSelectedCritters();

		void			killHalfOfCritters();
		void			togglePause();
// 		void			toggleSleeper();
		void			toggleMouselook();

		void			pickBody();
		void			unpickBody();
		void			selectBody();
		void			deselectBody();
		void			castMouseRay();
		void			movePickedBodyTo();
		void			movePickedBodyFrom();
		float			autosaveCounter;
		float			autoexchangeCounter;

		// vision
		unsigned char		*retina;
		unsigned int		items;

		const int	retinasperrow;
// 		const int*	retinasperrow;
		const int*	critter_raycastvision;
		const int*	critter_retinasize;
		const int*	critter_maxenergy;
		const int*	worldsizeX;
		const int*	worldsizeY;
		const int*	headless;
		const int*	threads;
		int			m_threads_last;

		void			checkCollisions( CritterB* c );

		void grabVision();

		void renderVision();

		void			calcMouseDirection();
		void			moveInMouseDirection(bool towards);

		void			calcFreeEnergy();
		float			m_freeEnergy;

		Dir			dirH;
		BeParser		parseH;

		bool			mouseRayHit;
		Entity*			mouseRayHitEntity;
		
		void setLogbuffer( boost::shared_ptr<Logbuffer> logBuffer ) { m_logBuffer = logBuffer; }

		// pointers to parents (evolution) mousepos
		int mousex;
		int mousey;
		int relx;
		int rely;
		
		// threading
// 		omp_lock_t my_lock1;

		boost::shared_ptr<BeGraphicsSystem> m_graphicsSystem; 
		BeFilesystem* m_filesystem;
	protected:
		boost::shared_ptr<Textverbosemessage> m_panel_textverbosemessage;

	private:
		
		Raycast*		raycast;

#ifdef HAVE_OPENCL
// 		CLNBody nbody;
#endif

		static void CollisionNearOverride(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo);
		castResult		mouseRay;
		btVector3		mouseRayTo;
		
		
		unsigned int		insertCritterCounter;
		unsigned int		foodIntervalCounter;
		unsigned int		critterIntervalCounter;


		// methods
		inline void		removeCritter(unsigned int cid);
// 		inline void		createDirs();

		// Settings pointers.. performance
		const int*	critter_maxlifetime;
		const int*	critter_autosaveinterval;
		const int*	critter_autoexchangeinterval;
		const int*	critter_sightrange;
		const int*	critter_enableomnivores;

		const int*	brain_mutationrate;
		const int*	body_mutationrate;

		const int*	population_limit_energy;
		const int*	population_limit_energy_percent;

		const int*	population_eliminate_portion;
		const int*	population_eliminate_portion_percent;
		const int*	population_eliminate_portion_decrenergy;
		const int*	population_eliminate_portion_incrworldsizeX;
		const int*	population_eliminate_portion_incrworldsizeY;
		const int*	population_eliminate_portion_decrmaxlifetimepct;

		const int*	population_eliminate_portion_brainmutationratechange;
		const int*	population_eliminate_portion_brainmutationratemin;
		const int*	population_eliminate_portion_brainmutationratemax;
		const int*	population_eliminate_portion_bodymutationratechange;
		const int*	population_eliminate_portion_bodymutationratemin;
		const int*	population_eliminate_portion_bodymutationratemax;

		const int*	population_double;
		
		const int*	food_maxlifetime;
		const int*	food_maxenergy;
		const int*	energy;
		const int*	mincritters;
		const int*	insertcritterevery;

		const int*	critter_startenergy;
		
// 		bool			m_extinctionmode;
// 		unsigned int	m_extinctionmode_until;
		
// 		const unsigned int m_starttime;
		std::string	m_starttime_s;
// 		std::string	m_hostname;
		


		// vision opts
		unsigned int picwidth;
		btScalar drawposition[16];
		
		btManifoldArray   manifoldArray;

		int			findSelectedCritterID();
		int			findCritterID(unsigned int cid);

		GLuint color_tex;
		GLuint fb;
		GLuint depth_rb;
		boost::shared_ptr<Logbuffer> m_logBuffer;
		
		BeColor m_wallColor;
		
};

#endif
