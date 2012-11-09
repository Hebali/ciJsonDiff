#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

#include "ciJsonDiff.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciJsonDiffApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ciJsonDiffApp::setup() {
    string tDiff = ciJsonDiff::diff( fs::path( getResourcePath() / "testA.json" ), fs::path( fs::path( getResourcePath() / "testB.json" ) ) );
    cout << tDiff << endl;
}

void ciJsonDiffApp::mouseDown( MouseEvent event ) {
}

void ciJsonDiffApp::update() {
}

void ciJsonDiffApp::draw() {
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP_BASIC( ciJsonDiffApp, RendererGl )
