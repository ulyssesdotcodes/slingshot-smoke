#include "Fluid.h"

const int VELOCITY_POINTER = 0;
const int PRESSURE_POINTER = 1;
const int ADVECT_POINTER = 2;

Fluid::Fluid() 
{
	mFluidResolution = vec2(512, 512);

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/advect_maccormack.frag"));
	mAdvectMaccormackShader = gl::GlslProg::create(updateFormat);
	mAdvectMaccormackShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/subtract_pressure.frag"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);
	mSubtractPressureShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/velocity_divergence.frag"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	mDivergenceShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/solve_pressure.frag"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);
	mPressureSolveShader->uniform("i_resolution", mFluidResolution);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA16F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mVelocityFBO = PingPongFBO(fmt, mFluidResolution, 4);

	texFmt.setInternalFormat(GL_RG16F);
	gl::Fbo::Format fmtRG;
	fmtRG.disableDepth()
		.setColorTextureFormat(texFmt);

	mPressureFBO = PingPongFBO(fmtRG, mFluidResolution, 2);
}


void Fluid::update(float dt, gl::GlslProgRef forces) 
{
	advect(dt, &mVelocityFBO);

	applyForces(forces);

	computeDivergence();
	solvePressure();
	subtractPressure();
}

void Fluid::advect(float dt, PingPongFBO* value) 
{
	{
		mAdvectShader->uniform("dt", dt);
		gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
		mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);
		gl::ScopedTextureBind scopeTarget(value->getTexture(), ADVECT_POINTER);
		mAdvectShader->uniform("tex_target", ADVECT_POINTER);

		value->render(mAdvectShader);
	}

	// Run time backwards for the second one
	{
		mAdvectShader->uniform("dt", -dt);
		gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
		mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);
		gl::ScopedTextureBind scopeTarget(value->getTexture(), ADVECT_POINTER);
		mAdvectShader->uniform("tex_target", ADVECT_POINTER);

		value->render(mAdvectShader);
	}

	{
		mAdvectMaccormackShader->uniform("dt", dt);
		gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
		mAdvectMaccormackShader->uniform("tex_velocity", VELOCITY_POINTER);

		vector<gl::TextureRef> textures = value->getTextures();
		gl::ScopedTextureBind scopedPhiN(textures.at(1), 3);
		mAdvectMaccormackShader->uniform("phi_n", 3);
		mAdvectMaccormackShader->uniform("tex_target", 3);
		gl::ScopedTextureBind scopedPhiN1Hat(textures.at(2), 4);
		mAdvectMaccormackShader->uniform("phi_n_1_hat", 4);
		gl::ScopedTextureBind scopedPhiNHat(textures.at(3), 5);
		mAdvectMaccormackShader->uniform("phi_n_hat", 5);

		value->render(mAdvectMaccormackShader);
	}

}

void Fluid::applyForces(gl::GlslProgRef forces)
{
	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	forces->uniform("tex_velocity", VELOCITY_POINTER);

	mVelocityFBO.render(forces);
}

void Fluid::computeDivergence() 
{
	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mDivergenceShader->uniform("tex_velocity", VELOCITY_POINTER);
	gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
	mDivergenceShader->uniform("tex_pressure", PRESSURE_POINTER);

	mPressureFBO.render(mDivergenceShader);
}

void Fluid::solvePressure()
{
	for (int i = 0; i < 40; ++i) {
		gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
		mPressureSolveShader->uniform("tex_pressure", PRESSURE_POINTER);
		mPressureFBO.render(mPressureSolveShader);
	}
}

void Fluid::subtractPressure()
{
	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mSubtractPressureShader->uniform("tex_velocity", VELOCITY_POINTER);
	gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
	mSubtractPressureShader->uniform("tex_pressure", PRESSURE_POINTER);

	mVelocityFBO.render(mSubtractPressureShader);
}