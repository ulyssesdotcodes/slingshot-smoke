#include "cinder\app\App.h"

#include "PositionSmoker.h"

using namespace ci;

PositionSmoker::PositionSmoker(vec2 fluidResolution, vec2 smokeResolution) : Smoker(fluidResolution, smokeResolution)
{
	mPerlin = Perlin();

	mSmokeVelocity = vec2(0.1, 0.1);

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Smokers/smoke_forces.frag"));
	mForcesProg = gl::GlslProg::create(updateFormat);
	mForcesProg->uniform("i_resolution", fluidResolution);

	updateFormat.fragment(app::loadAsset("Smokers/position_drop.frag"));
	mDropProg = gl::GlslProg::create(updateFormat);
	mDropProg->uniform("i_resolution", smokeResolution);
}

void PositionSmoker::update(float volumeMult, float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField) 
{
	float smoothedVolume = audioSource->getSmoothedVolume();
	float volume = audioSource->getVolume();

	vec2 noise = vec2(mPerlin.noise(mSmokePosition.x, app::getElapsedSeconds() * 0.8), mPerlin.noise(mSmokePosition.y, app::getElapsedSeconds() * 1.6));
	noise = glm::normalize(noise);

	mSmokeVelocity += noise * (vec2((1.0 / 0.7) * math<float>::max(0, 0.7 - smoothedVolume)));

	mSmokeVelocity = glm::normalize(mSmokeVelocity);

	mSmokePosition += mSmokeVelocity * dt * volume;

	// If the position is close to the edge, reverse the velocity
	if ((mSmokePosition.x > 0.8 && mSmokeVelocity.x > 0) || (mSmokePosition.x < 0.2 && mSmokeVelocity.x < 0)) {
		mSmokeVelocity.x = -mSmokeVelocity.x;
	}

	if ((mSmokePosition.y > 0.8 && mSmokeVelocity.y > 0) || (mSmokePosition.y < 0.2 && mSmokeVelocity.y < 0)) {
		mSmokeVelocity.y = -mSmokeVelocity.y;
	}

	// Set the uniforms
	mForcesProg->uniform("i_dt", dt);
	mForcesProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_dt", dt);
	mDropProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_volume", smoothedVolume * volumeMult);
	mDropProg->uniform("i_smokePosition", mSmokePosition);
	mDropProg->uniform("i_fullness", 1.0f);

	// Update the fluid with the smoker's forces shader
	fluid->update(dt, mForcesProg, smokeField->getTexture());

	// Drop new smoke
	drop(mDropProg, smokeField);

	// Use the fluid to advect the smoke
	fluid->advect(dt, smokeField);
}

void PositionSmoker::light(vec2 smokePosition, params::InterfaceGlRef params)
{
	Smoker::light(smokePosition, params);
}
