#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.setGLVersion(4, 3);
    settings.windowMode = OF_WINDOW;
    ofCreateWindow(settings);
	ofRunApp(new ofApp());

}
