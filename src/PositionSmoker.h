#pragma once

#include "cinder\Perlin.h"

#include "Smoker.h"
#include "PingPongFBO.h"

class PositionSmoker : public Smoker
{
public:
	PositionSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float volume, float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField);

private:
	Perlin mPerlin;

	gl::GlslProgRef
		mForcesProg,
		mDropProg;

	vec2 mSmokePosition,
		mSmokeVelocity;
};