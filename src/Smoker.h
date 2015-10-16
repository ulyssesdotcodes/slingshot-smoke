#pragma once

#include "cinder\gl\gl.h"
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
	virtual void update(float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField) = 0;

protected:
	vec2 mFluidResolution, mSmokeResolution;

	void drop(gl::GlslProgRef prog, PingPongFBO* target);
};