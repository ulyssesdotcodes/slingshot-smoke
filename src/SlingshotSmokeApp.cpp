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
	PingPongFBO mSmokeFBO;

	gl::GlslProgRef mRenderProg;

	void drop(gl::GlslProgRef prog, PingPongFBO* target);
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

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA16F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mSmokeFBO = PingPongFBO(fmt, getWindowSize(), 4);
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

void SlingshotSmokeApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}


////////////////////
// Helper methods

// Drop new smoke onto the FBO
void SlingshotSmokeApp::drop(gl::GlslProgRef prog, PingPongFBO* target) {
	gl::ScopedTextureBind scopeSmokeDrop(mSmokeFBO.getTexture(), 0);
	prog->uniform("tex_prev", 0);
	target->render(prog);
}

CINDER_APP(SlingshotSmokeApp, RendererGl(), [&](App::Settings *settings) {
	FullScreenOptions options;
	vector<DisplayRef> displays = Display::getDisplays();
	settings->setFullScreen(true, options);	
	settings->setFrameRate(60.0f);
});