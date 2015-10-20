#pragma once

#include "Smoker.h"

class TransitionSmoker : public Smoker
{
public:
	TransitionSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float volume, float dt, Fluid* fluid, AudioSource* audioSource, PingPongFBO* smokeField);
	void light(vec2 smokePosition, params::InterfaceGlRef params);

private:
	vec2 mSmokeVelocity,
		mDestination;

	float mStartTime,
		mSmokeFullness;

	gl::GlslProgRef
		mForcesProg,
		mLineDropProg,
		mPositionDropProg;
};