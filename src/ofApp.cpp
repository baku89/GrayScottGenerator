#include "ofApp.h"


// init settings

#define DEFAULT_F		0.07f
#define DEFAULT_K		0.077f
#define DEFAULT_DIFF_U	0.25f
#define DEFAULT_DIFF_V	0.04f
#define DEFAULT_STEP	9
#define DEFAULT_TEXW	1024
#define DEFAULT_TEXH	1024
#define DEFAULT_ZOOM	1
#define DEFAULT_FPS		24
#define DEFAULT_FILENAME "gray-scott"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // initial parameters
	f = DEFAULT_F;
	k = DEFAULT_K;
	diffU = DEFAULT_DIFF_U;
	diffV = DEFAULT_DIFF_V;
	step = DEFAULT_STEP;
    bPause = false;
	bRec = false;
	savedNum = 0;
	
	bModalOpened = false;
	
	filename = DEFAULT_FILENAME;
	
    // gui
    gui = new ofxUISuperCanvas("Control Panel");
    
    gui->addSpacer();
    gui->addFPS();
    guiInfo = gui->addLabel("");
	guiGen  = gui->addLabel("");
	guiFilename = gui->addLabel("");
	
	gui->addSpacer();
	gui->addLabel("Comp Settings");
    guiFPS  = gui->addNumberDialer("fps", 1, 60, 24, 0);
    guiTexw = gui->addNumberDialer("texture width", 128, 2048, DEFAULT_TEXW, 0);
    guiTexh = gui->addNumberDialer("texture height", 128, 4096, DEFAULT_TEXH, 0);
    guiZoom = gui->addNumberDialer("zoom (1/n)", 1, 8, DEFAULT_ZOOM, 0);
	guiFloat= gui->addToggle("enable 32bit", false);
    gui->addLabelButton("apply", false);
    
    gui->addSpacer();
    gui->addLabel("Parameters");
    gui->addSlider("K", 0.005, 0.1, &k);
    gui->addSlider("F", 0.01, 0.1, &f);
    gui->addSlider("diff U", 0.001, 0.4f, &diffU);
    gui->addSlider("diff V", 0.001, 0.05f, &diffV);
    gui->addNumberDialer("step", 1, 30, &step, 0);

    gui->addSpacer();
    gui->addLabel("Commands");
    gui->addLabelButton("clear", false);
	gui->addLabelButton("change filename", false);
    //gui->addLabelButton("set destination", false);
    //gui->addLabelButton("load initcond", false);
    
    gui->autoSizeToFitWidgets();
    
    gui->loadSettings("gui.xml");
    
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
	
	updateCompSettings();
}

//--------------------------------------------------------------
void ofApp::updateCompSettings() {
	
	// set window
	int texw = (int)guiTexw->getValue();
	int texh = (int)guiTexh->getValue();
	int zoom = (int)guiZoom->getValue();
	
	gs.allocate(texw, texh, GL_RGB32F);
	pixels.allocate(texw, texh, 3);
	floatPixels.allocate(texw, texh, 3);
	
	ofSetWindowShape(texw / zoom, texh / zoom);
	scale = 1.0f / (float)zoom;
	gen = 0;
	
	
	bFloating = guiFloat->getValue();
	
	ss << texw << " x " << texh << ", zoom: 1/" << zoom;
	guiInfo->setLabel(ss.str());
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    if (this == NULL) {
        return;
    }
    
    string name = e.widget->getName();
    
    if ( name == "apply" && !ofGetMousePressed() ) {
        
		updateCompSettings();
		
	} else if ( name == "fps" ) {
	
		ofSetFrameRate( (int)guiFPS->getValue() );
		
	} else if ( name == "clear" && !ofGetMousePressed() ) {
        
		updateCompSettings();
    
	} else if ( name == "change filename" && !ofGetMousePressed() && !bModalOpened ) {
		
		bModalOpened = true;
		string str = ofSystemTextBoxDialog("enter filename");
		bModalOpened = false;
		
		if (str != "") {
			filename = str;
			savedNum = 0;
		}
		
	}/* else if ( name == "set destination" && !ofGetMousePressed() && !bModalOpened ) {
        
        bModalOpened = true;
        ofFileDialogResult result = ofSystemSaveDialog("", "");
        bModalOpened = false;
        
        cout << result.getPath() << endl << result.getName();
        
    }*//* else if ( name == "load initcond" && !ofGetMousePressed() ) {
        
        ofFileDialogResult result = ofSystemLoadDialog();
        
        ofImage initCond;
        initCond.loadImage( result.getPath() );
        
        if ( !initCond.bAllocated() ) {
            ofSystemAlertDialog("This file type is not supported");
            return;
        }
        
        int texw = (int)guiTexw->getValue();
        int texh = (int)guiTexh->getValue();
        
        gs.allocate(texw, texh, GL_RGB32F);
        
        gs.begin();
        ofSetColor(0xff, 0, 0xff);
        initCond.draw(0, 0, texw, texh);
        gs.end();
    }*/
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    gs.setF(f);
    gs.setK(k);
    gs.setDiffU(diffU);
    gs.setDiffV(diffV);
    
    if (!bPause) {
        for (int i = 0; i < (int)step; i++) {
            gs.update();
            gen++;
        }
    }
    
    if (bRec) {
        saveFrame();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    gs.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	
	if (bCaptured) {
		ofSetColor(255);
		ofNoFill();
		ofSetLineWidth(10);
		ofRect(ofGetWindowRect());
	}
	bCaptured = false;
	
	// ofxUI
    ss.str("");
    ss << "Generation: " << gen;
	guiGen->setLabel(ss.str());
	
	ss.str("");
	ss << "name: " << filename << "-" << savedNum;
    if (bRec) {
        ss << " (R)";
    }
	guiFilename->setLabel(ss.str());
	
	// HUD
	ofPoint p;
	p.set(ofGetWindowWidth() - 400, ofGetWindowHeight() - 40);
    ofDrawBitmapString("[space]: toggle play/pause, [r]: toggle rec", p);
    p.y += 16;
    ofDrawBitmapString("[s]: save current frame", p);
}

//--------------------------------------------------------------
void ofApp::exit() {
    
    gui->saveSettings("gui.xml");
}

//--------------------------------------------------------------

void ofApp::saveFrame() {
    // save one frame
    ofTexture &tex = gs.getTextureReference();
 
    ss.str("");
    ss << filename << "/" << filename << setfill('0') << setw(8) << savedNum++ << ".tiff";
	
	if (bFloating) {
		tex.readToPixels(floatPixels);
		ofSaveImage(floatPixels, ss.str());
	} else {
		tex.readToPixels(pixels);
		ofSaveImage(pixels, ss.str());
	}
	
	bCaptured = true;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case ' ':
            bPause = !bPause;
            break;
        case 'r':
            bRec = !bRec;
            break;
        case 's':
            saveFrame();
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    drawGrayScott(x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    drawGrayScott(x, y);
}

//--------------------------------------------------------------
void ofApp::drawGrayScott(int x, int y){
    
    
    ofxUIRectangle *guiRect = gui->getRect();
    
    if (guiRect->inside(x, y)) {
        return;
    }
    
    gs.begin();
    ofSetColor(ofNoise( ofGetElapsedTimef() ) * 0xff);
	ofFill();
	ofSetLineWidth(0);
    ofCircle(x / scale, y / scale, 3);
    gs.end();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
