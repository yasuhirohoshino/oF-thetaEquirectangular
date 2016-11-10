#pragma once

#include "ofMain.h"
#include "ofxDeckLink.h"
#include "ofxImGui.h"
#include "ofxJSON.h"

struct CameraParams{
    float radius[2];
    ofVec2f centerPos[2];
    float rotate[2];
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void loadShader();
        void drawGui();
        void loadJSON();
        void saveJSON();

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
		
    ofxDeckLinkAPI::Input bmdInput;
    ofxImGui gui;
    ofxJSONElement json;
    
    ofShader shader;
    ofFbo srcFbo, dstFbo;
    ofEasyCam easyCam;
    bool enableCamControl = true;
    ofVboMesh sphereMesh;
    
    int appMode = 1;
    int guiMode = 0;
    
    CameraParams camera[2];
    float blendingRatio = 0.99;
    float translate = 0.0;
    const ofVec2f resolution = ofVec2f(1920, 960);
};
