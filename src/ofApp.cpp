#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    //--------GRAINCLOUD-----------------------

    
    grainVoices = cloud.getVoicesNum();
    
    // sampleData is an instance of pdsp::SampleBuffer
    sampleData.setVerbose(true);
    sampleData.load(  "/Users/f00b455/Music/samples/collection/ambient/drone/453552__kyles__rumble-low-water-gushing-movement.wav" ); // many debugger (included gdb and xcode) change the base path
                                                         // when you run your app in debug mode
                                                         // so the best is to use an absolute path
    cloud.setWindowType(pdsp::Triangular); // select the shape of the grain envelope
            // available windows: Rectangular, Triangular, Hann, Hamming, Blackman, BlackmanHarris, SineWindow, Tukey, Welch
    cloud.setSample(&sampleData); // give to the pdsp::GrainCloud the pointer to the sample

    posX >> cloud.in_position();
    jitY >> cloud.in_position_jitter();
    length >> cloud.in_length();
    density   >> cloud.in_density();
    distanceJit  >> cloud.in_distance_jitter();
    pitchJit >> cloud.in_pitch_jitter();
    pitch >> cloud.in_pitch();
    cloud.in_direction();
    cloud.getInputsList();


    ampControl.enableSmoothing(50.0f);
    ampControl.set(0.0f);

    amps.resize(2);
    ampControl >> amps[0].in_mod();
    ampControl >> amps[1].in_mod();

    cloud.ch(0) >> amps[0] * dB(12.0f) >> engine.audio_out(0);
    cloud.ch(1) >> amps[1] * dB(12.0f) >> engine.audio_out(1);
    
    cout<<"finished patching\n";
    
    
    //ui values-------------------------------
    uiWidth  = ofGetWidth()-40;
    uiHeigth = ofGetHeight()-40;
    uiX      = 20;
    uiY      = 20;
    uiMaxX = uiX + uiWidth;
    uiMaxY = uiY + uiHeigth;
    drawGrains = false;
 
    //graphic setup---------------------------
    ofSetVerticalSync(true);
    ofDisableAntiAliasing();
    ofBackground(0);
    ofSetColor(ofColor(0, 255, 255));
    ofNoFill();
    ofSetLineWidth(1.0f);
    ofSetFrameRate(24);

    // create an ofFbo with the plotted waveform
    waveformGraphics.setWaveform(sampleData, 0, ofColor(0, 100, 100, 255), uiWidth, uiHeigth);
    
    
    //-----------------init and start audio-------------
    engine.listDevices();
    engine.setDeviceID(2); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup( 48000, 512, 3 );
    
    
    
    // -- imgui
    
    gui.setup();
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().MouseDrawCursor = false;

}

//--------------------------------------------------------------
void ofApp::update(){
    pitch       >> cloud.in_pitch();
    density     >> cloud.in_density();
    length      >> cloud.in_length();
    posX        >> cloud.in_position();
    jitY        >> cloud.in_position_jitter();
    distanceJit >> cloud.in_distance_jitter();
    pitchJit    >> cloud.in_pitch_jitter();
}

//--------------------------------------------------------------
void ofApp::draw(){

    gui.begin();
    
    ImGui::Text("f00gr GrainSampler");
    ImGui::SliderFloat("dens", &density, 0.0f, 1.0f);
    ImGui::SliderFloat("pitch", &pitch, -24.0f, 24.0f);
    ImGui::SliderFloat("length", &length, 10.0f, 5000.0f);
    ImGui::SliderFloat("pos", &posX, 0.0f, 1.0f);
    ImGui::SliderFloat("jit", &jitY, 0.0f, 1.0f);
    ImGui::SliderFloat("distJit", &distanceJit, 0.0f, 100.0f);
    ImGui::SliderFloat("pitchJit", &pitchJit, -24.0f, 24.0f);
    
    
    gui.end();
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofDrawRectangle(uiX, uiY, uiWidth, uiHeigth);
    
    waveformGraphics.draw(uiX, uiY);
    ofDrawBitmapString (sampleData.filePath, uiX+5, uiY+15);
    
    if(drawGrains){
        //draw position cross
        ofDrawLine(controlX, uiY, controlX, uiMaxY);
        ofDrawLine(uiX, controlY, uiMaxX, controlY);
        //draw grains
        ofSetRectMode(OF_RECTMODE_CENTER);
        int grainsY = uiY + uiHeigth/2;
        
        ofLog(OF_LOG_NOTICE, "voices: %i" , grainVoices);
        
        for (int i=0; i<grainVoices; ++i){
            float xpos = uiX + (uiWidth * cloud.meter_position(i));
            float dimensionX = cloud.meter_env(i)*10;
            float dimensionY = cloud.meter_env(i)*50;
            ofDrawRectangle(xpos, grainsY, dimensionX, dimensionY);
        }
    }

    ofDrawBitmapString ( "click to control: x=position y=jitter  |  press L to load a sample", uiX+5, uiHeigth+15);

    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if( key=='l' || key=='L' ) loadRoutine();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::controlOn(int x, int y){
    //ofMap(x, uiX, uiMaxX, 0.0f, 1.0f, true) >> posX;
    //ofMap(y, uiY, uiMaxY, 1.0f, 0.0f) >> jitY;
    
    if(x > uiX && x<uiMaxX && y>uiY && y<uiMaxY){
        ampControl.set(1.0f);
        drawGrains = true;
        controlX = x;
        controlY = y;
    }
}

void ofApp::mouseDragged(int x, int y, int button){
    controlOn(x, y);
}

void ofApp::mousePressed(int x, int y, int button){
    controlOn(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    //ampControl.set(0.0f);
    //drawGrains = false;
}

//--------------------------------------------------------------
void ofApp::loadRoutine() {
    
    ampControl.set( 0.0f );
    drawGrains = false;
    
    //Open the Open File Dialog
    ofFileDialogResult openFileResult= ofSystemLoadDialog("select an audio sample");
    
    //Check if the user opened a file
    if (openFileResult.bSuccess){
        
        string path = openFileResult.getPath();
        
        sampleData.load ( path );
        
        waveformGraphics.setWaveform(sampleData, 0, ofColor(0, 100, 100, 255), uiWidth, uiHeigth);
    
        ofLogVerbose("file loaded");
        
    }else {
        ofLogVerbose("User hit cancel");
    }

}
        
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

void ofApp::selectAudioOutDevice(int deviceID) {
    if (deviceID >= devices.size()) {
        ofLog() << "Critical error: No sound card found.";
        return;
    }
    engine.setDeviceID(deviceID);
}

void ofApp::selectAudioOutDevice(string deviceName) {
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].name == deviceName) {
            selectAudioOutDevice(i);
        }
    }
}
