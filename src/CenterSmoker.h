#pragma once

#include "Smoker.h"

class CenterSmoker : public Smoker
{
public:
	CenterSmoker(vec2 fluidResolution, vec2 smokeResolution);
	void update(float dt);
	gl::GlslProgRef getForcesProg();
	gl::GlslProgRef getDropProg();

private:
	gl::GlslProgRef 
		mForcesProg,
		mDropProg;
};