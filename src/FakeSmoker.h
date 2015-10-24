#pragma once

#include "Smoker.h"

class FakeSmoker : public Smoker
{
public:
	FakeSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float volume, float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField);
	virtual void light(vec2 smokePosition, params::InterfaceGlRef params);

private:
	gl::GlslProgRef mForcesProg,
		mDropProg;
};
