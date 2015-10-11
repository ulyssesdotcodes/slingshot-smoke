#pragma once

#include "cinder\gl\gl.h"
#include "cinder\gl\BufferTexture.h"
#include "PingPongFBO.h"

using namespace ci;

class Fluid {
public:
	// Update the main velocity state
	void update(float dt, gl::GlslProgRef forces);

	// Advect a texture based on the current velocity
	void advect(float dt, PingPongFBO value);

	// Drop the new colors onto the texture provided
	void drop(float dt, gl::GlslProgRef dropShader, PingPongFBO valueTexture);
};