#include "Smoker.h"

Smoker::Smoker(vec2 fluidResolution, vec2 smokeResolution) {
	mFluidResolution = fluidResolution;
	mSmokeResolution = smokeResolution;
	mSmokePosition = vec2(0.5, 0.5);
}

void Smoker::light(vec2 smokePosition, params::InterfaceGlRef params)
{
	mSmokePosition = smokePosition;
}

vec2 Smoker::getPosition()
{
	return mSmokePosition;
}

// Drop new smoke onto the FBO
void Smoker::drop(gl::GlslProgRef prog, PingPongFBO* target) {
	gl::ScopedTextureBind scopeSmokeDrop(target->getTexture(), 0);
	prog->uniform("tex_prev", 0);
	target->render(prog);
}