#include "Smoker.h"

Smoker::Smoker(vec2 fluidResolution, vec2 smokeResolution) {
	mFluidResolution = fluidResolution;
	mSmokeResolution = smokeResolution;
}

// Drop new smoke onto the FBO
void Smoker::drop(gl::GlslProgRef prog, PingPongFBO* target) {
	gl::ScopedTextureBind scopeSmokeDrop(target->getTexture(), 0);
	prog->uniform("tex_prev", 0);
	target->render(prog);
}