#ifndef BE_SCENE_NODE_H_INCLUDED
#define BE_SCENE_NODE_H_INCLUDED

#include "LinearMath/btTransform.h"

class BeSceneNode
{
public:
	BeSceneNode();
	void setTransform(const btTransform& transform) { m_transform=transform; }
	const btTransform& getTransform() const { return m_transform; }
	void setOrigin(const btVector3& origin) { m_transform.setIdentity(); m_transform.setOrigin(origin); }
	const btVector3& getOrigin() const { return m_transform.getOrigin(); }
	void translate(const btVector3& vector) { m_transform.setOrigin(m_transform.getOrigin()+vector); }
	void translateLocal(const btVector3& vector);
	const btVector3& getRightVector() const { return m_transform.getBasis().getRow(0); }
	const btVector3& getUpVector() const { return m_transform.getBasis().getRow(1); }
	const btVector3& getForwardVector() const { return m_transform.getBasis().getRow(2); }
	void yaw(const float radians){ m_transform *= btTransform(btQuaternion(radians,0,0)); }
	void pitch(const float radians){ m_transform *= btTransform(btQuaternion(0,radians,0)); }
	void roll(const float radians){ m_transform *= btTransform(btQuaternion(0,0,radians)); }
private:
	btTransform m_transform;
};

#endif