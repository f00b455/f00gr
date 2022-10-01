#pragma once

#include "ofMain.h"
#include "ofxf00gr.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void controlOn(int x, int y);
    void loadRoutine();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    
    // pdsp modules
    pdsp::Engine            engine;
    pdsp::VAOscillator      osc;
    pdsp::LFO               lfo;
    std::vector<ofSoundDevice> devices;
    std::vector<string> deviceList;
    
    pdsp::SampleBufferPlotter  waveformGraphics;
   
    int                         grainVoices;
    pdsp::SampleBuffer          sampleData;
    pdsp::GrainCloud            cloud {100};
    
    float density = 1.0;
    float pitch = 0.0;
    float length = 1000.0f;
    float posX;
    float jitY;
    float distanceJit = 1.0f;
    float pitchJit = 0.0f;
    
    std::vector<pdsp::Amp>  amps;
    pdsp::ValueControl      ampControl;


    
    bool    drawGrains;

    int     uiWidth;
    int     uiHeigth;
    int     uiX;
    int     uiY;
    int     uiMaxX;
    int     uiMaxY;
    int     controlX;
    int     controlY;
    
    // gui
    ofxImGui::Gui gui;
    ImVec4 backgroundColor;
    
    
private:
    void selectAudioOutDevice(int deviceID);
    void selectAudioOutDevice(string deviceName);
};
