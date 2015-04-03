#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // initial parameters
    f = 0.0195f;
    k = 0.066f;
    diffU = 0.25f;
    diffV = 0.04f;
    step = 9;
    bPause = false;
    
    destPath = "export/";
    bModalOpened = true;
    
    // gui
    gui = new ofxUISuperCanvas("Control Panel");
    
    gui->addSpacer();
    gui->addFPS();
    guiInfo = gui->addLabel("info", "1024 x 1024, zoom: 1/1");
    guiFPS  = gui->addNumberDialer("FPS", 1, 60, 24, 0);
    guiTexw = gui->addNumberDialer("texture width", 128, 2048, 1024, 0);
    guiTexh = gui->addNumberDialer("texture height", 128, 2048, 1024, 0);
    guiZoom = gui->addNumberDialer("zoom out (1/n)", 1, 8, 1, 0);
    
    gui->addLabelButton("apply", false);
    
    gui->addSpacer();
    gui->addLabel("Parameters");
    gui->addSlider("K", 0.005, 0.095, &k);
    gui->addSlider("F", 0.01, 0.028, &f);
    gui->addSlider("diff U", 0.001, 0.4f, &diffU);
    gui->addSlider("diff V", 0.001, 0.05f, &diffV);
    gui->addNumberDialer("step", 1, 30, &step, 0);

    gui->addSpacer();
    gui->addLabel("Commands");
    gui->addLabelButton("clear", false);
    //gui->addLabelButton("set destination", false);
    //gui->addLabelButton("load initcond", false);
    
    gui->autoSizeToFitWidgets();
    
    gui->loadSettings("gui.xml");
    
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
    
    // set window
    int fps = (int)guiFPS->getValue();
    int texw = (int)guiTexw->getValue();
    int texh = (int)guiTexh->getValue();
    int zoom = (int)guiZoom->getValue();
    
    gs.allocate(texw, texh, GL_RGB32F);
    ofSetWindowShape(texw / zoom, texh / zoom);
    scale = 1.0f / (float)zoom;
    gen = 0;
    
    ss << texw << " x " << texh << ", zoom: 1/" << zoom;
    guiInfo->setLabel(ss.str());
    
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    if (this == NULL) {
        return;
    }
    
    string name = e.widget->getName();
    
    cout << name << endl;
    
    if ( name == "apply" && !ofGetMousePressed() ) {
        
        int fps = (int)guiFPS->getValue();
        int texw = (int)guiTexw->getValue();
        int texh = (int)guiTexh->getValue();
        int zoom = (int)guiZoom->getValue();
        
        ofSetFrameRate( fps );
        gs.allocate(texw, texh, GL_RGB32F);
        ofSetWindowShape(texw / zoom, texh / zoom);
        scale = 1.0f / (float)zoom;
        gen = 0;
        
        ss.str("");
        ss << texw << " x " << texh << ", zoom: 1/" << zoom;
        guiInfo->setLabel(ss.str());
        
    } else if ( name == "clear" && !ofGetMousePressed() ) {
        
        int texw = (int)guiTexw->getValue();
        int texh = (int)guiTexh->getValue();
        
        gs.allocate(texw, texh, GL_RGB32F);
        gen = 0;
    
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
    
    ofPoint p;
    p.set(ofGetWindowWidth() - 400, ofGetWindowHeight() - 60);

    ss.str("");
    ss << "generation: " << gen;
    ofDrawBitmapString(ss.str(), p);
    
    p.y += 16;
    ss.str("");
    ss << frameNum << " frames saved";
    if (bRec) {
        ss << " (REC)";
    }
    ofDrawBitmapString(ss.str(), p);
    
    p.y += 16;
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
    
    tex.readToPixels(pixels);
    
    ss.str("");
    ss << destPath << "gray-scott_" << setfill('0') << setw(8) << frameNum++ << ".tiff";
    
    ofSaveImage(pixels, ss.str());
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
