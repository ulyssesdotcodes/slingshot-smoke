#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Fluid.h"
#include "Smoker.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SlingshotSmokeApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	float mLastTime;

	Fluid mFluid;
	Smoker* mCurrentSmoker;
	PingPongFBO mSmokeFBO;

	gl::GlslProgRef mRenderProg;

	void drop(gl::GlslProgRef prog, PingPongFBO* target);
};

////////////////////
// Cinder methods

void SlingshotSmokeApp::setup()
{
	mLastTime = 0;

	mFluid = Fluid();

	mRenderProg = gl::getStockShader(gl::ShaderDef().texture());
}

void SlingshotSmokeApp::mouseDown( MouseEvent event )
{
}

void SlingshotSmokeApp::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	// Update the smoker so that it has the correct forces shader and ping pong fbo
	mCurrentSmoker->update(dt);

	// Update the fluid with the smoker's forces shader
	mFluid.update(dt, mCurrentSmoker->getForcesProg());

	// Drop new smoke
	drop(mCurrentSmoker->getDropProg(), &mSmokeFBO);

	// Use the fluid to advect the smoke
	mFluid.advect(dt, &mSmokeFBO);
}

void SlingshotSmokeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	// Bind the smoke texture
	gl::ScopedTextureBind texScope(mSmokeFBO.getTexture());

	// Set the appropriate bounds
	gl::ScopedViewport vp(ivec2(0), mSmokeFBO.getBounds().getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(mSmokeFBO.getBounds().getSize());

	// Draw the smoke texture
	gl::ScopedGlslProg glsl(mRenderProg);
	gl::drawSolidRect(mSmokeFBO.getBounds());

	// Pop everything for good measure
	gl::popMatrices();
}


////////////////////
// Helper methods

// Drop new smoke onto the FBO
void SlingshotSmokeApp::drop(gl::GlslProgRef prog, PingPongFBO* target) {
	target->render(prog);
}

CINDER_APP( SlingshotSmokeApp, RendererGl )
