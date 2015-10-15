#include "cinder\app\App.h"

#include "CenterSmoker.h"

using namespace ci;

CenterSmoker::CenterSmoker(vec2 fluidResolution, vec2 smokeResolution) : Smoker(fluidResolution, smokeResolution)
{
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Smokers/center_force.frag"));
	mForcesProg = gl::GlslProg::create(updateFormat);
	mForcesProg->uniform("i_resolution", fluidResolution);

	updateFormat.fragment(app::loadAsset("Smokers/center_drop.frag"));
	mDropProg = gl::GlslProg::create(updateFormat);
	mDropProg->uniform("i_resolution", smokeResolution);

	updateFormat.fragment(app::loadAsset("Smokers/center_draw.frag"));
	mDrawProg = gl::GlslProg::create(updateFormat);
	mDrawProg->uniform("i_resolution", smokeResolution);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA32F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mSmokeFields = PingPongFBO(fmt, smokeResolution, 4);

	mSmokeFBO = gl::Fbo::create(smokeResolution.x, smokeResolution.y, fmt);
}

void CenterSmoker::update(float dt, Fluid* fluid, AudioSource* audioSource) 
{
	mForcesProg->uniform("i_dt", dt);
	mForcesProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_dt", dt);
	mDropProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_volume", audioSource->getVolume());

	// Drop new smoke
	drop(mDropProg, &mSmokeFields);

	// Use the fluid to advect the smoke
	fluid->advect(dt, &mSmokeFields);

	// Update the fluid with the smoker's forces shader
	fluid->update(dt, mForcesProg, mSmokeFields.getTexture());

	{
		gl::ScopedFramebuffer smoke(mSmokeFBO);
		gl::clear(Color(0, 0, 0));

		gl::ScopedTextureBind tex(mSmokeFields.getTexture(), 2);
		mDrawProg->uniform("tex", 2);

		gl::ScopedViewport vp(ivec2(0), mSmokeFBO->getSize());
		gl::pushMatrices();
		gl::setMatricesWindow(mSmokeFBO->getSize());
		gl::ScopedGlslProg glsl(mDrawProg);

		gl::drawSolidRect(mSmokeFBO->getBounds());

		gl::popMatrices();
	}
}

// Drop new smoke onto the FBO
void CenterSmoker::drop(gl::GlslProgRef prog, PingPongFBO* target) {
	gl::ScopedTextureBind scopeSmokeDrop(mSmokeFields.getTexture(), 0);
	prog->uniform("tex_prev", 0);
	target->render(prog);
}

gl::GlslProgRef CenterSmoker::getForcesProg()
{
	return mForcesProg;
}

gl::TextureRef CenterSmoker::getSmokeTexture()
{
	return mSmokeFBO->getColorTexture();
}
