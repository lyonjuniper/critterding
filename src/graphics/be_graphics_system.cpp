#include "be_graphics_system.h"

BeGraphicsSystem::BeGraphicsSystem()
{

}

void BeGraphicsSystem::reset2D(const int width, const int height)
{
	glViewport(0,0,width,height);
	disable(GL_DEPTH_TEST);
	disable(GL_BLEND);
	disable(GL_ALPHA_TEST);
	glDepthMask(GL_FALSE);
	matrixLoadIdentity(GL_PROJECTION);
	matrixOrtho(GL_PROJECTION, 0, width, height, 0,  0, 1);
	matrixLoadIdentity(GL_MODELVIEW);
	glColor4f(1,1,1,1);
	useProgram(0);
}

void BeGraphicsSystem::reset3D(const int width, const int height)
{
	glViewport(0,0,width,height);
	enable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	//enable(GL_ALPHA_TEST);
	//enable(GL_BLEND);
	glColor4f(1,1,1,1);
}

void BeGraphicsSystem::flush()
{

}

void BeGraphicsSystem::bindFramebuffer( BeFramebuffer* const framebuffer )
{
	if(framebuffer)
	{
		m_graphicsKernel.bindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle());
	}
	else
	{
		m_graphicsKernel.bindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void BeGraphicsSystem::bindVertexArray( BeVertexArray* const vertexArray )
{
	if(vertexArray)
	{
		m_graphicsKernel.bindVertexArray(vertexArray->handle());
	}
	else
	{
		m_graphicsKernel.bindVertexArray(0);
	}
}

void BeGraphicsSystem::bindTexture2D( BeTexture2D* const texture )
{
	if(texture)
	{
		m_graphicsKernel.bindTexture(GL_TEXTURE_2D, texture->handle());
	}
	else
	{
		m_graphicsKernel.bindTexture(GL_TEXTURE_2D, 0);
	}
}

void BeGraphicsSystem::bindMultiTexture2D(const GLint index, BeTexture2D* const texture )
{
	if(texture)
	{
		m_graphicsKernel.bindMultiTexture(index, GL_TEXTURE_2D, texture->handle());
	}
	else
	{
		m_graphicsKernel.bindMultiTexture(index, GL_TEXTURE_2D, 0);
	}
}


void BeGraphicsSystem::useProgram( BeProgram* const program )
{
	if(program)
	{
		m_graphicsKernel.useProgram(program->handle());
	}
	else
	{
		m_graphicsKernel.useProgram(0);
	}
}

void BeGraphicsSystem::drawMesh( const BeMesh* const mesh )
{
	assert(mesh);
}

void BeGraphicsSystem::matrixPush(const GLenum matrixMode)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.pushMatrix();
}

void BeGraphicsSystem::matrixPop(const GLenum matrixMode)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.popMatrix();
}

void BeGraphicsSystem::matrixLoad(const GLenum matrixMode, const float* const m)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.loadMatrix(m);
}

void BeGraphicsSystem::matrixLoadIdentity(const GLenum matrixMode)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.loadIdentity();
}

void BeGraphicsSystem::matrixOrtho(const GLenum matrixMode, const double l, const double r, const double b, const double t, const double n, const double f)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.ortho(l, r, b, t, n, f);
}

void BeGraphicsSystem::matrixFrustum(const GLenum matrixMode, const double l, const double r, const double b, const double t, const double n, const double f)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.frustum(l, r, b, t, n, f);
}


void BeGraphicsSystem::matrixPerspective( const GLenum matrixMode, const float fovy, const float aspect, const float zNear, const float zFar )
{
	glMatrixMode(matrixMode);
	const float frustumHalfHeight = tan( fovy * 180 / M_PI * M_PI / 360.0 ) * zNear;
	const float frustumHalfWidth = frustumHalfHeight * aspect;
	matrixFrustum(matrixMode, -frustumHalfWidth, frustumHalfWidth, -frustumHalfHeight, frustumHalfHeight, zNear, zFar);
}


void BeGraphicsSystem::matrixTranslate(const GLenum matrixMode, const float x, const float y, const float z)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.translate(x, y, z);
}

void BeGraphicsSystem::matrixRotate(const GLenum matrixMode, const float angle, const float x, const float y, const float z)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.rotate(angle, x, y, z);
}

void BeGraphicsSystem::matrixScale(const GLenum matrixMode, const float x, const float y, const float z)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.scale(x, y, z);
}

void BeGraphicsSystem::matrixMult(const GLenum matrixMode, const float* const m)
{
	m_graphicsKernel.matrixMode(matrixMode);
	m_graphicsKernel.multMatrix(m);
}

void BeGraphicsSystem::clear( const GLenum mask )
{
	m_graphicsKernel.clear(mask);
}

void BeGraphicsSystem::enable( const GLenum cap )
{
	m_graphicsKernel.enable(cap);
}

void BeGraphicsSystem::disable( const GLenum cap )
{
	m_graphicsKernel.disable(cap);
}

void BeGraphicsSystem::hint( const GLenum target, const GLenum hint )
{
	m_graphicsKernel.hint(target, hint);
}

void BeGraphicsSystem::shadeModel( const GLenum mode )
{
	m_graphicsKernel.shadeModel(mode);
}

void BeGraphicsSystem::cullFace( const GLenum mode )
{
	m_graphicsKernel.cullFace(mode);
}

void BeGraphicsSystem::lightModel( const GLenum pname, const Vector4f& params )
{
	m_graphicsKernel.lightModel(pname, params.getFloatPointer());
}

void BeGraphicsSystem::lightModel( const GLenum pname, const GLenum param )
{
	m_graphicsKernel.lightModel(pname, reinterpret_cast<const GLint*>(&param));
}

void BeGraphicsSystem::light( const GLenum lightId, const GLenum pname, const GLfloat* const params )
{
	m_graphicsKernel.light(lightId, pname, params);
}

void BeGraphicsSystem::material( const GLenum face, const GLenum pname, const Vector4f& params )
{
	m_graphicsKernel.material(face, pname, params.getFloatPointer());
}

void BeGraphicsSystem::material( const GLenum face, const GLenum pname, const btVector3& params )
{
    const float f[4]={params.x(), params.y(), params.z(),1};
    m_graphicsKernel.material(face, pname, f);
}

void BeGraphicsSystem::material( const GLenum face, const GLenum pname, const float param )
{
	m_graphicsKernel.material(face, pname, &param);
}

void BeGraphicsSystem::applyLight( const GLenum lightId, const BeLight* const l )
{
	if(l)
	{
		disable(GL_COLOR_MATERIAL);
		light(lightId, GL_AMBIENT, l->getAmbient().getFloatPointer());
		light(lightId, GL_DIFFUSE, l->getDiffuse().getFloatPointer());
		light(lightId, GL_SPECULAR, l->getSpecular().getFloatPointer());
		matrixPush(GL_MODELVIEW);
		matrixLoadIdentity(GL_MODELVIEW);
		light(lightId, GL_POSITION, l->m_position.getFloatPointer());
		matrixPop(GL_MODELVIEW);
		const float direction[3]={l->m_spotDirection.x, l->m_spotDirection.y, l->m_spotDirection.z};
		light(lightId, GL_SPOT_DIRECTION, direction);
		light(lightId, GL_SPOT_EXPONENT, &l->m_spotExponent);
		light(lightId, GL_SPOT_CUTOFF, &l->m_spotCutoff);
		const Vector3f attenuation=l->m_attenuation;
		light(lightId, GL_CONSTANT_ATTENUATION, &attenuation.x);
		light(lightId, GL_LINEAR_ATTENUATION, &attenuation.y);
		light(lightId, GL_QUADRATIC_ATTENUATION, &attenuation.z);
		enable(lightId);
	}
	else
	{
		disable(lightId);
	}
}

void BeGraphicsSystem::applyMaterial( const GLenum face, const BeMaterial* const m )
{
	if(m)
	{
		material(face, GL_AMBIENT, m->getAmbient());
		material(face, GL_DIFFUSE, m->getDiffuse());
		material(face, GL_SPECULAR, m->getSpecular());
		material(face, GL_EMISSION, m->getEmission());
		material(face, GL_SHININESS, m->getShininess());
	}
	else
	{
		BeMaterial mat;
		material(face, GL_AMBIENT, mat.getAmbient());
		material(face, GL_DIFFUSE, mat.getDiffuse());
		material(face, GL_SPECULAR, mat.getSpecular());
		material(face, GL_EMISSION, mat.getEmission());
		material(face, GL_SHININESS, mat.getShininess());
	}
}

void BeGraphicsSystem::bindBuffer( const GLenum target, BeBuffer* const buffer )
{
	if(buffer)
	{
		buffer->bind();
	}
	else
	{
		m_graphicsKernel.bindBuffer(target, 0);
	}
}

void BeGraphicsSystem::applyCamera( const BeCameraPerspective& camera )
{
	matrixLoadIdentity(GL_PROJECTION);
	matrixPerspective(GL_PROJECTION, camera.m_fovy, camera.m_aspect, camera.m_zNear, camera.m_zFar);
	const BeSceneNode* const sceneNode=camera.getSceneNode();
	if(sceneNode)
	{
		btScalar modelview[16];
		sceneNode->getTransform().inverse().getOpenGLMatrix(modelview);
		matrixLoad(GL_MODELVIEW, modelview);
	}
	else
	{
		matrixLoadIdentity(GL_MODELVIEW);
	}
}
