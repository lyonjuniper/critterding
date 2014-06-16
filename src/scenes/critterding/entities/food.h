#ifndef FOOD_H_INCLUDED
#define FOOD_H_INCLUDED

#include "btBulletDynamicsCommon.h"
// #include "GL/gl.h"

#include "src/utils/settings.h"
#include "displaylists.h"
#include "entity.h"
#include "body.h"

// using namespace std;

class Food : public Entity
{
	public:
		Food();
		~Food();

		Body			body;
		void			createBody(btDynamicsWorld* m_dynamicsWorld, const btVector3& startOffset);

		void			draw();
		void			draw( const bool do_color, const bool do_scale );
// 		void			draw_no_color();

		float		position[16];

// 		float			color[3];

		unsigned int		lifetime;
		float			energyLevel;
		unsigned int		totalFrames;
		btDefaultMotionState* myMotionState;
		btVector3 halfExtent;
	private:
		Settings		*settings;

		btBoxShape* boxShape;

		const int*	food_size;
};

#endif
