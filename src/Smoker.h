#pragma once

#include "cinder\gl\gl.h"
#include "Fluid.h"

using namespace ci;

// A container for all the things needed to create and animate a smoking object. It must update the program's
// uniforms correctly.
class Smoker
{
public:
	Smoker(vec2 fluidResolution, vec2 smokeResolution);

	// Override these!
	virtual void update(float dt, Fluid* fluid) = 0;
	virtual gl::GlslProgRef getForcesProg() = 0;
	virtual gl::TextureRef getSmokeTexture() = 0;

protected:
	vec2 mFluidResolution, mSmokeResolution;
};