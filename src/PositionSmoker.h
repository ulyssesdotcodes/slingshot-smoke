#pragma once

#include "Smoker.h"
#include "PingPongFBO.h"

class PositionSmoker : public Smoker
{
public:
	PositionSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField);

private:
	float mVolumeMult;

	gl::GlslProgRef
		mForcesProg,
		mDropProg;
};