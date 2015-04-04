#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "GrayScott32.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void guiEvent(ofxUIEventArgs &e);
    
    void drawGrayScott(int x, int y);
	
	void updateCompSettings();
    void saveFrame();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // var
    stringstream ss;
    
    ofxUISuperCanvas    *gui;
    ofxUILabel          *guiInfo, *guiGen, *guiFilename, *guiRec;
    ofxUINumberDialer   *guiFPS, *guiTexw, *guiTexh, *guiZoom;
	ofxUIToggle			*guiFloat;
    
    // screen settings
    float scale;
    
    GrayScott32 gs;
    
	ofPixels pixels;
	ofFloatPixels floatPixels;
	
    float f, k, diffU, diffV;
    float step;
    
	int gen, savedNum;
	
    bool bPause, bRec;
    bool bModalOpened;
	bool bCaptured;
	bool bFloating;
    
	string filename;
};
