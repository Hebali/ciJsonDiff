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
    string tDiff = ciJsonDiff::diff( fs::path("/Users/pjh/Desktop/TESTjsonA.json"), fs::path("/Users/pjh/Desktop/TESTjsonB.json") );
    cout << tDiff << endl;
    
    //http://www.bantherewind.com/cijson
    
    /*
    // Create JSON
    Json::Value mData;
    
    // Add data of a few types
    ciJsonDiff::append(mData, "myString", "test");
    ciJsonDiff::append(mData, "myInt", 1);
    ciJsonDiff::append(mData, "myBool", true);

    // THIS IS VALID:
    Json::Value mDataA;
    ciJsonDiff::append( mDataA, "_NEW_", 4.1 );
    ciJsonDiff::append( mDataA, "_OLD_", 7.3 );
    ciJsonDiff::append( mData, "mSomething", mDataA );
    
    vector<int> mInts;
    mInts.push_back( 1 );
    mInts.push_back( 2 );
    mInts.push_back( 3 );
    ciJsonDiff::append(mData, "myIntArray", mInts );
    
    
    string tOut = ciJsonDiff::serialize( mData );
    cout << tOut << endl;
    
    // Add an array of objects (append value to itself three times)
    vector<Json::Value> mValues;
    mValues.push_back(mData);
    mValues.push_back(mData);
    mValues.push_back(mData);
    ciJsonDiff::append(mData, "myObjects", mValues);
    */
}

void ciJsonDiffApp::mouseDown( MouseEvent event ) {
}

void ciJsonDiffApp::update() {
}

void ciJsonDiffApp::draw() {
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP_BASIC( ciJsonDiffApp, RendererGl )
