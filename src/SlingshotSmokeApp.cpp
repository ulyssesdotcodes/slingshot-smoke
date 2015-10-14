#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Fluid.h"
#include "Smoker.h"
#include "CenterSmoker.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SlingshotSmokeApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

private:
	float mLastTime;

	Fluid mFluid;
	Smoker* mCurrentSmoker;

	gl::GlslProgRef mRenderProg;

};

////////////////////
// Cinder methods

void SlingshotSmokeApp::setup()
{
	mLastTime = 0;

	vec2 fluidResolution = vec2(512);

	mFluid = Fluid(fluidResolution);
	mCurrentSmoker = new CenterSmoker(fluidResolution, app::getWindowSize());

	mRenderProg = gl::getStockShader(gl::ShaderDef().texture());
}

void SlingshotSmokeApp::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	// Update the smoker so that it has the correct forces shader and ping pong fbo
	mCurrentSmoker->update(dt, &mFluid);
}

void SlingshotSmokeApp::draw()
{
	vec2 size = mCurrentSmoker->getSmokeTexture()->getSize();

	gl::clear( Color( 0, 0, 0 ) ); 

	// Set the appropriate bounds
	gl::ScopedViewport vp(ivec2(0), size);
	gl::pushMatrices();
	gl::setMatricesWindow(size);

	gl::ScopedGlslProg glsl(mRenderProg);

	// Draw the smoke texture
	{
		gl::ScopedTextureBind texScope(mCurrentSmoker->getSmokeTexture());
		gl::drawSolidRect(mCurrentSmoker->getSmokeTexture()->getBounds());
	}

	// Pop everything for good measure
	gl::popMatrices();
}

void SlingshotSmokeApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}


////////////////////
// Helper methods


CINDER_APP(SlingshotSmokeApp, RendererGl(), [&](App::Settings *settings) {
	FullScreenOptions options;
	vector<DisplayRef> displays = Display::getDisplays();
	settings->setFullScreen(true, options);	
	settings->setFrameRate(60.0f);
});