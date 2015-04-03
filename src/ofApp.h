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
    ofxUILabel          *guiInfo, *guiRec;
    ofxUINumberDialer   *guiFPS, *guiTexw, *guiTexh, *guiZoom;
    
    // screen settings
    float scale;
    
    GrayScott32 gs;
    
    ofPixels pixels;
    
    float f, k, diffU, diffV;
    float step;
    
    int gen, frameNum;
    
    bool bPause, bRec;
    bool bModalOpened;
    
    string destPath;
};
