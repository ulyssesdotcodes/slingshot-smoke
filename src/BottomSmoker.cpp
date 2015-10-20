#include "BottomSmoker.h"

BottomSmoker::BottomSmoker(vec2 fluidResolution, vec2 smokeResolution) : Smoker(fluidResolution, smokeResolution)
{
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Smokers/smoke_forces.frag"));
	mForcesProg = gl::GlslProg::create(updateFormat);
	mForcesProg->uniform("i_resolution", fluidResolution);

	updateFormat.fragment(app::loadAsset("Smokers/line_drop.frag"));
	mDropProg = gl::GlslProg::create(updateFormat);
	mDropProg->uniform("i_resolution", smokeResolution);
}

void BottomSmoker::update(float volume, float dt, Fluid * fluid, AudioSource * audioSource, PingPongFBO* smokeField)
{
	mForcesProg->uniform("i_dt", dt);
	mForcesProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_dt", dt);
	mDropProg->uniform("i_time", (float) app::getElapsedSeconds());
	mDropProg->uniform("i_volume", audioSource->getSmoothedVolume() * volume);
	mDropProg->uniform("i_smokeLineY", audioSource->getHighestVolumePos() * 0.5f + 0.05f);
	mDropProg->uniform("i_fullness", 1.0f);

	// Drop new smoke
	drop(mDropProg, smokeField);

	// Use the fluid to advect the smoke
	fluid->advect(dt, smokeField);

	// Update the fluid with the smoker's forces shader
	fluid->update(dt, mForcesProg, smokeField->getTexture());
}

void BottomSmoker::light(vec2 smokePosition, params::InterfaceGlRef params)
{
	Smoker::light(smokePosition, params);
}
