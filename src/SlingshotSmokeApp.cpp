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

	AudioSource mAudioSource;

	Fluid mFluid;
	Smoker* mCurrentSmoker;

	gl::GlslProgRef mRenderProg;

};

////////////////////
// Cinder methods

void SlingshotSmokeApp::setup()
{
	mLastTime = 0;

	mAudioSource = AudioSource();
	mAudioSource.setup();

	vec2 fluidResolution = vec2(512);
	vec2 smokeResolution = app::getWindowSize();

	mFluid = Fluid(fluidResolution);
	mCurrentSmoker = new CenterSmoker(fluidResolution, smokeResolution);

	gl::GlslProg::Format renderFormat;
	renderFormat.vertex(app::loadAsset("passthru.vert"));
	renderFormat.fragment(app::loadAsset("Smokers/smoke_draw.frag"));
	mRenderProg = gl::GlslProg::create(renderFormat);
	mRenderProg->uniform("i_resolution", smokeResolution);
}

void SlingshotSmokeApp::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	mAudioSource.update();

	// Update the smoker so that it has the correct forces shader and ping pong fbo
	mCurrentSmoker->update(dt, &mFluid, &mAudioSource);
}

void SlingshotSmokeApp::draw()
{
	gl::clear(Color(0, 0, 0));

	gl::ScopedTextureBind tex(mCurrentSmoker->getSmokeTexture(), 2);
	mRenderProg->uniform("tex", 2);

	gl::ScopedViewport vp(ivec2(0), mCurrentSmoker->getSmokeTexture()->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(mCurrentSmoker->getSmokeTexture()->getSize());
	gl::ScopedGlslProg glsl(mRenderProg);

	gl::drawSolidRect(mCurrentSmoker->getSmokeTexture()->getBounds());

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