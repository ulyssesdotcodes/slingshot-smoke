#pragma once

#include "Smoker.h"
#include "PingPongFBO.h"

class CenterSmoker : public Smoker
{
public:
	CenterSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float dt, Fluid* fluid, AudioSource* audioSource);
	gl::GlslProgRef getForcesProg();
	gl::TextureRef getSmokeTexture();

private:
	float mVolumeMult;

	gl::GlslProgRef
		mForcesProg,
		mDropProg;

	PingPongFBO mSmokeFields;

	void drop(gl::GlslProgRef prog, PingPongFBO* target);
};