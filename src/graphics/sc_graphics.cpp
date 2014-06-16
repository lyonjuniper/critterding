#include "sc_graphics.h"

#include <GL/glew.h>

ScGraphics::ScGraphics( boost::shared_ptr<BeGraphicsSystem> graphicsSystem, BeFilesystem& filesystem ) : m_graphicsSystem(graphicsSystem)
{
	//Initialise default GL state

	//Hint for everything to be nicest
	m_graphicsSystem->hint(GL_FOG_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	m_graphicsSystem->hint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

	//Smooth shading
	m_graphicsSystem->shadeModel(GL_SMOOTH);

	//Enable back face culling
	//m_graphicsSystem.enable(GL_CULL_FACE);
	//m_graphicsSystem.cullFace(GL_BACK);

	//Enable depth test
	m_graphicsSystem->enable(GL_DEPTH_TEST);

	//Alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable (GL_BLEND); 

	glEnable ( GL_ALPHA_TEST );
	glAlphaFunc ( GL_GREATER, 0.5f ) ;

	//Set default light model settings
	m_graphicsSystem->lightModel(GL_LIGHT_MODEL_AMBIENT, Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
	m_graphicsSystem->lightModel(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	m_graphicsSystem->lightModel(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	m_graphicsSystem->lightModel(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	//Set one directional light by default
	BeDirectionalLight light;
	light.setDirection(Vector3f(1,1,0));
	light.setSpecular(Vector4f(1,1,1,1));
	m_graphicsSystem->applyLight(GL_LIGHT0, &light);

	//Set the default material
	m_graphicsSystem->applyMaterial(GL_FRONT_AND_BACK, 0);

	//Disable lighting
	m_graphicsSystem->disable(GL_LIGHTING);

	//Set default matrices
	m_graphicsSystem->matrixLoadIdentity(GL_PROJECTION);
	m_graphicsSystem->matrixLoadIdentity(GL_MODELVIEW);

	m_effect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/default.vert", "shaders/default.frag"));

	m_luminanceEffect2 = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/luminance2x2.frag"));
	m_luminanceEffect3 = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/luminance2x2.frag"));
	m_brightEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/brightpass.frag"));
	m_gaussianBlurHorizonalEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/gaussianH.frag"));
	m_gaussianBlurVerticalEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/gaussianV.frag"));
	m_tonemapEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/tonemap.frag"));
	m_addRgbEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/simple.vert", "shaders/addrgb.frag"));

	const int width=Settings::Instance()->getCVar("window_width");
	const int height=Settings::Instance()->getCVar("window_height");

	m_luminanceEffect2->m_program->uniform("Width", width);
	m_luminanceEffect2->m_program->uniform("Height", height);
	
	m_luminanceEffect3->m_program->uniform("Width", width);
	m_luminanceEffect3->m_program->uniform("Height", height);

	m_brightEffect->m_program->uniform("Tex0", 0);
	m_brightEffect->m_program->uniform("Tex1", 1);

	float weights[9] = {0.0677841f, 0.0954044f, 0.121786f, 0.140999f, 0.148054f, 0.140999f, 0.121786f, 0.0954044f, 0.0677841f};
	float offsetsH[9], offsetsV[9];
	for (int i=0; i<9; i++) {
		offsetsH[i] = (i - 4.0f) / float(width);
		offsetsV[i] = (i - 4.0f) / float(height);
	}

	m_gaussianBlurHorizonalEffect->m_program->uniform("Weights", 9, weights);
	m_gaussianBlurHorizonalEffect->m_program->uniform("Offsets", 9, offsetsH);

	m_gaussianBlurVerticalEffect->m_program->uniform("Weights", 9, weights);
	m_gaussianBlurVerticalEffect->m_program->uniform("Offsets", 9, offsetsV);

	m_tonemapEffect->m_program->uniform("Tex0", 0);
	m_tonemapEffect->m_program->uniform("Tex1", 1);

	m_addRgbEffect->m_program->uniform("Tex0", 0);
	m_addRgbEffect->m_program->uniform("Tex1", 1);


	// Create luminance FBOs chain
	int chain_length = 1;
	int start_size = (height/2 < width/2) ? height/2 : width/2; // min(width/4, height/4)
	int size = 1;
	for (size = 1; size < start_size; size *= 2) {
		chain_length++;
	}
	size /= 2;
	chain_length--;

	int max3_size = size;

// 	printf("MAX3_SIZE: %d\n", size);

	m_sceneTexture = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width, height));
	m_sceneRenderbuffer = boost::shared_ptr<BeRenderbuffer>(new BeRenderbuffer(GL_DEPTH_COMPONENT, width, height));
	m_sceneFramebuffer = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());

	m_sceneFramebuffer->texture2D(GL_COLOR_ATTACHMENT0, m_sceneTexture->handle(), 0);
	m_sceneFramebuffer->renderbuffer(GL_DEPTH_ATTACHMENT, m_sceneRenderbuffer->handle());

	m_luminanceTexture2 = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width/2, height/2));
	m_luminanceFramebuffer2 = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
	m_luminanceFramebuffer2->texture2D(GL_COLOR_ATTACHMENT0, m_luminanceTexture2->handle(), 0);
	m_luminanceTexture2->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
	m_luminanceTexture2->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);

	for (int i = 0; i < chain_length; i++) {
		m_luminanceFramebuffer3[i] = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
		m_luminanceTexture3[i] = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, size, size));
		m_luminanceTexture3[i]->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		m_luminanceTexture3[i]->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
		m_luminanceFramebuffer3[i]->texture2D(GL_COLOR_ATTACHMENT0, m_luminanceTexture3[i]->handle(), 0);
		size /= 2;
	}

	m_brightTexture = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width/4, height/4));
	m_brightFramebuffer = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
	m_brightFramebuffer->texture2D(GL_COLOR_ATTACHMENT0, m_brightTexture->handle(), 0);

	m_tonemapTexture = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width, height));
	m_tonemapFramebuffer = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
	m_tonemapFramebuffer->texture2D(GL_COLOR_ATTACHMENT0, m_tonemapTexture->handle(), 0);

	for(int i=0;i<2;++i)
	{
		m_gaussianTexture[i]=boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width/4, height/4));
		m_gaussianFramebuffer[i] = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
		m_gaussianFramebuffer[i]->texture2D(GL_COLOR_ATTACHMENT0, m_gaussianTexture[i]->handle(), 0);
	}

	//DOF

	m_dofSceneEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/default_dof.vert", "shaders/default_dof.frag"));
	m_dofSkyboxEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/skybox_dof.vert", "shaders/skybox_dof.frag"));

	m_dofDownsampleEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/second.vert", "shaders/second.frag"));
	m_dofFilterXEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/third.vert", "shaders/third.frag"));
	m_dofFilterYEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/fourth.vert", "shaders/fourth.frag"));
	m_dofFinalEffect = boost::shared_ptr<BeGraphicsEffect>(new BeGraphicsEffect(filesystem, "shaders/fifth.vert", "shaders/fifth.frag"));

	m_dofSceneTexture = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width, height));
	m_dofSceneFramebuffer = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
	m_dofSceneFramebuffer->texture2D(GL_COLOR_ATTACHMENT0, m_dofSceneTexture->handle(), 0);

	for(int i=0;i<3;++i)
	{
		m_dofDownsampleTexture[i] = boost::shared_ptr<BeTexture2D>(new BeTexture2D(GL_RGBA16F, width, height));
		m_dofDownsampleFramebuffer[i] = boost::shared_ptr<BeFramebuffer>(new BeFramebuffer());
		m_dofDownsampleFramebuffer[i]->texture2D(GL_COLOR_ATTACHMENT0, m_dofDownsampleTexture[i]->handle(), 0);
		m_dofDownsampleTexture[i]->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		m_dofDownsampleTexture[i]->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
	}



	m_dofSceneEffect->m_program->uniform("focalDistance",2.0f);
	m_dofSceneEffect->m_program->uniform("focalRange",550.0f);

	m_dofSkyboxEffect->m_program->uniform("focalDistance",2.0f);
	m_dofSkyboxEffect->m_program->uniform("focalRange",550.0f);

	m_dofDownsampleEffect->m_program->uniform("Tex0", 0);
	m_dofFilterXEffect->m_program->uniform("Tex0", 0);
	m_dofFilterXEffect->m_program->uniform("Width", width);
	m_dofFilterYEffect->m_program->uniform("Tex0", 0);
	m_dofFilterYEffect->m_program->uniform("Height", height);
	m_dofFinalEffect->m_program->uniform("Tex0", 0);
	m_dofFinalEffect->m_program->uniform("Tex1", 1);

}

ScGraphics::~ScGraphics()
{

}
