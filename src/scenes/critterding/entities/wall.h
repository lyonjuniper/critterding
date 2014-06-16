#ifndef WALL_H
#define WALL_H

#include "btBulletDynamicsCommon.h"
// #include "GL/gl.h"
#include "displaylists.h"
#include <iostream>

#include "entity.h"

using namespace std;

class Wall : public Entity
{
	public:
		Wall(const float X, const float Y, const float Z, const btTransform& transform, btDynamicsWorld* m_dynamicsWorld);
		~Wall();

		void			draw();
		void			draw( const bool do_color, const bool do_scale );
// 		void			create(float X, float Y, float Z, btVector3 position, btDynamicsWorld* m_dynamicsWorld);

// 		float			color[4];
	private:
		float			halfX;
		float			halfY;
		float			halfZ;
		btTransform		groundTransform;
		btDynamicsWorld* 	m_ownerWorld;
		btCollisionShape*	groundShape;
		btScalar		nposition[16];
		btCollisionObject*	fixedGround;
};

#endif
