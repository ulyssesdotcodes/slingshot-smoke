#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SlingshotSmokeApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SlingshotSmokeApp::setup()
{
}

void SlingshotSmokeApp::mouseDown( MouseEvent event )
{
}

void SlingshotSmokeApp::update()
{
}

void SlingshotSmokeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( SlingshotSmokeApp, RendererGl )
