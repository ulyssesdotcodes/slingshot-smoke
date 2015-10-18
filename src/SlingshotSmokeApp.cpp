#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"

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
	void drawRender();
	void drawParams();

private:
	float mVolumeMult;

	float mLastTime;

	AudioSource mAudioSource;

	Fluid mFluid;
	PingPongFBO mSmokeField;

	params::InterfaceGlRef mParams;

	vector<shared_ptr<Smoker>> mSmokers;
	int mCurrentSmoker;

	gl::GlslProgRef mRenderProg;
};

////////////////////
// Cinder methods

void SlingshotSmokeApp::setup()
{
	mVolumeMult = 5.0;

	mLastTime = 0;

	getWindowIndex(0)->getSignalDraw().connect([=]() { drawRender(); });

	mAudioSource = AudioSource();
	mAudioSource.setup();

	vec2 fluidResolution = vec2(512);
	vec2 smokeResolution = app::getWindowSize();

	mFluid = Fluid(fluidResolution);
	mSmokers.reserve(2);
	mSmokers.push_back(shared_ptr<BottomSmoker>(new BottomSmoker(fluidResolution, smokeResolution)));
	mSmokers.push_back(shared_ptr<PositionSmoker>(new PositionSmoker(fluidResolution, smokeResolution)));
	mCurrentSmoker = 1;

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


	// Do params last so that all the FBOs are in the right context
	vec2 paramsSize = vec2(255, 512);
	auto format = Window::Format();
	format.setSize(paramsSize + vec2(40, 20));
	format.setPos(ivec2(100));
	WindowRef paramsWindow = createWindow(format);
	paramsWindow->getSignalDraw().connect([=]() { drawParams(); });
	mParams = params::InterfaceGl::create(paramsWindow, "Options", paramsSize);

	mParams->addParam("Volume", &mVolumeMult)
		.max(10.0)
		.min(0.0);
}

void SlingshotSmokeApp::update()
{
	getWindowIndex(0)->getRenderer()->makeCurrentContext();

	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	if (dt > 0.5) {
		dt = 0.5;
	}

	mAudioSource.update();

	// Update the smoker so that it has the correct forces shader and ping pong fbo
	mSmokers[mCurrentSmoker]->update(mVolumeMult, dt, &mFluid, &mAudioSource, &mSmokeField);
}

void SlingshotSmokeApp::drawRender()
{
	getWindowIndex(0)->getRenderer()->makeCurrentContext();

	vec2 smokeSize = mSmokeField.getTexture()->getSize();
	gl::clear(Color(0, 0, 0));

	gl::ScopedTextureBind tex(mSmokeField.getTexture(), 2);
	mRenderProg->uniform("tex", 2);

	gl::ScopedViewport vp(ivec2(0), smokeSize);
	gl::pushMatrices();
	gl::setMatricesWindow(smokeSize);
	gl::ScopedGlslProg glsl(mRenderProg);

	gl::drawSolidRect(Rectf(0, 0, smokeSize.x, smokeSize.y));

	gl::popMatrices();
}

void SlingshotSmokeApp::drawParams()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mParams->draw();
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
	settings->setFrameRate(60.0f);
	settings->setFullScreen(true, options);	
	if (displays.size() > 1) {
		settings->setDisplay(displays[1]);
	}
});