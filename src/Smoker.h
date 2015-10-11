#pragma once

#include "cinder\gl\gl.h"

using namespace ci;

// A container for all the things needed to create and animate a smoking object. It must update the program's
// uniforms correctly.
class Smoker
{
public:
	virtual void update(float dt) = 0;
	virtual gl::GlslProgRef getForcesProg() = 0;
	virtual gl::GlslProgRef getDropProg() = 0;
};