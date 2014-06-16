#include "be_scene_node.h"

BeSceneNode::BeSceneNode()
{
	m_transform.setIdentity();
}

void BeSceneNode::translateLocal(const btVector3& vector)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin( vector );
	m_transform *= transform;
}
