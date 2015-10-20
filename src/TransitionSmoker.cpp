#include "TransitionSmoker.h"

TransitionSmoker::TransitionSmoker(vec2 fluidResolution, vec2 smokeResolution) : 
	Smoker(fluidResolution, smokeResolution)
{
	mSmokeFullness = 0.0;
	mDestination = vec2(0.5, 0.05);

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Smokers/smoke_forces.frag"));
	mForcesProg = gl::GlslProg::create(updateFormat);
	mForcesProg->uniform("i_resolution", fluidResolution);

	updateFormat.fragment(app::loadAsset("Smokers/position_drop.frag"));
	mPositionDropProg = gl::GlslProg::create(updateFormat);
	mPositionDropProg->uniform("i_resolution", smokeResolution);

	updateFormat.fragment(app::loadAsset("Smokers/line_drop.frag"));
	mLineDropProg = gl::GlslProg::create(updateFormat);
	mLineDropProg->uniform("i_resolution", smokeResolution);
}

void TransitionSmoker::light(vec2 smokePosition, params::InterfaceGlRef params)
{
	Smoker::light(smokePosition, params);
	mSmokeVelocity = vec2(mDestination - mSmokePosition) * vec2(1.5);
}

void TransitionSmoker::update(float volumeMult, float dt, Fluid * fluid, AudioSource * audioSource, PingPongFBO * smokeField)
{
	float volume = audioSource->getSmoothedVolume();

	vec2 dist = mSmokePosition - mDestination;
	if(glm::dot(dist, dist) > 0.001) {
		mSmokePosition += mSmokeVelocity * dt * volume;
	}
	else if(mSmokeFullness < 1.0) {
		mSmokeFullness += 0.4 * dt * volume;
	}

	// Set the uniforms
	mForcesProg->uniform("i_dt", dt);
	mForcesProg->uniform("i_time", (float) app::getElapsedSeconds());

	mPositionDropProg->uniform("i_dt", dt);
	mPositionDropProg->uniform("i_time", (float) app::getElapsedSeconds());
	mPositionDropProg->uniform("i_volume", volume * volumeMult);
	mPositionDropProg->uniform("i_smokePosition", mSmokePosition);
	mPositionDropProg->uniform("i_fullness", 1.0f - mSmokeFullness);

	mLineDropProg->uniform("i_dt", dt);
	mLineDropProg->uniform("i_time", (float) app::getElapsedSeconds());
	mLineDropProg->uniform("i_volume", volume * volumeMult);
	mLineDropProg->uniform("i_smokeLineY", 0.05f);
	mLineDropProg->uniform("i_fullness", mSmokeFullness);

	// Drop new smoke
	drop(mPositionDropProg, smokeField);

	// Drop new smoke
	drop(mLineDropProg, smokeField);

	// Use the fluid to advect the smoke
	fluid->advect(dt, smokeField);

	// Update the fluid with the smoker's forces shader
	fluid->update(dt, mForcesProg, smokeField->getTexture());
}