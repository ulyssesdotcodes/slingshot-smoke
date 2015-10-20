#pragma once

#include "cinder\gl\gl.h"
#include "cinder\params\Params.h"

#include "Fluid.h"
#include "AudioSource.h"

using namespace ci;

// A container for all the things needed to create and animate a smoking object. It must update the program's
// uniforms correctly.
class Smoker
{
public:
	Smoker(vec2 fluidResolution, vec2 smokeResolution);

	// Override these!
	virtual void update(float volume, float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField) = 0;
	virtual void light(vec2 smokePosition, params::InterfaceGlRef params);
	vec2 getPosition();

protected:
	vec2 mFluidResolution, mSmokeResolution;

	vec2 mSmokePosition;

	void drop(gl::GlslProgRef prog, PingPongFBO* target);
};