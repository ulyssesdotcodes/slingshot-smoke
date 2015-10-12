#include "cinder\app\App.h"

#include "CenterSmoker.h"

using namespace ci;

CenterSmoker::CenterSmoker(vec2 fluidResolution, vec2 smokeResolution) : Smoker(fluidResolution, smokeResolution)
{
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/apply_force.frag"));
	mForcesProg = gl::GlslProg::create(updateFormat);
	mForcesProg->uniform("i_resolution", fluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/center_drop.frag"));
	mDropProg = gl::GlslProg::create(updateFormat);
	mDropProg->uniform("i_resolution", smokeResolution);
}

void CenterSmoker::update(float dt) {

	mForcesProg->uniform("i_dt", dt);
	mDropProg->uniform("i_dt", dt);
	mDropProg->uniform("i_time", (float)app::getElapsedSeconds());
}

gl::GlslProgRef CenterSmoker::getForcesProg()
{
	return mForcesProg;
}

gl::GlslProgRef CenterSmoker::getDropProg()
{
	return mDropProg;
}
