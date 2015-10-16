#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Fluid.h"
#include "Smoker.h"
#include "PositionSmoker.h"
#include "BottomSmoker.h"

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
	PingPongFBO mSmokeField;

	vector<shared_ptr<Smoker>> mSmokers;
	int mCurrentSmoker;

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
	mSmokers.reserve(2);
	mSmokers.push_back(shared_ptr<BottomSmoker>(new BottomSmoker(fluidResolution, smokeResolution)));
	mSmokers.push_back(shared_ptr<PositionSmoker>(new PositionSmoker(fluidResolution, smokeResolution)));
	mCurrentSmoker = 0;

	gl::GlslProg::Format renderFormat;
	renderFormat.vertex(app::loadAsset("passthru.vert"));
	renderFormat.fragment(app::loadAsset("Smokers/smoke_draw.frag"));
	mRenderProg = gl::GlslProg::create(renderFormat);
	mRenderProg->uniform("i_resolution", smokeResolution);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA32F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mSmokeField = PingPongFBO(fmt, smokeResolution, 4);
}

void SlingshotSmokeApp::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	mAudioSource.update();

	// Update the smoker so that it has the correct forces shader and ping pong fbo
	mSmokers[mCurrentSmoker]->update(dt, &mFluid, &mAudioSource, &mSmokeField);
}

void SlingshotSmokeApp::draw()
{
	gl::clear(Color(0, 0, 0));

	gl::ScopedTextureBind tex(mSmokeField.getTexture(), 2);
	mRenderProg->uniform("tex", 2);

	gl::ScopedViewport vp(ivec2(0), mSmokeField.getTexture()->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(mSmokeField.getTexture()->getSize());
	gl::ScopedGlslProg glsl(mRenderProg);

	gl::drawSolidRect(mSmokeField.getTexture()->getBounds());

	gl::popMatrices();
}

void SlingshotSmokeApp::keyDown(KeyEvent event) {
	switch (event.getChar()) {
	case 'q':
		quit();
		break;
	case ' ':
		mCurrentSmoker = (mCurrentSmoker == mSmokers.size() - 1) ? 0 : mCurrentSmoker + 1;
		break;
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