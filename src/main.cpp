#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1200, OF_WINDOW);			// <-------- setup the GL context       // 900 , 700
	//ofSetupOpenGL(1920, 1200, OF_FULLSCREEN);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
