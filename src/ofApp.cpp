#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
    devices = theta.listDevices();
    bool isDeviceConnected = false;
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].deviceName == "RICOH THETA S"){
            theta.setDeviceID(devices[i].id);
            isDeviceConnected = true;
        }
    }
    if(!isDeviceConnected){
        ofLog(OF_LOG_ERROR, "RICOH THETA S is not found.");
    }
    theta.initGrabber(1280, 720);
    
    shader.load("shaders/equirectanguler");
    
    fbo.allocate(1280, 640);
    
    sphere = ofSpherePrimitive(2000, 64).getMesh();
    for(int i=0;i<sphere.getNumTexCoords();i++){
        sphere.setTexCoord(i, ofVec2f(1.0) - sphere.getTexCoord(i));
    }
    for(int i=0;i<sphere.getNumNormals();i++){
        sphere.setNormal(i, sphere.getNormal(i) * ofVec3f(-1));
    }
    
    offset.set("uvOffset", ofVec4f(0,0.0,0,0.0), ofVec4f(-0.1), ofVec4f(0.1));
    radius.set("radius", 0.445, 0.0, 1.0);
    showSphere.set("showSphere", false);
    thetaParams.add(offset);
    thetaParams.add(radius);
    gui.setup(thetaParams);
    gui.add(showSphere);
    
    cam.setAutoDistance(false);
    cam.setDistance(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    theta.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(theta.isFrameNew()){
        
        fbo.begin();
        ofClear(0);
        shader.begin();
        shader.setUniformTexture("mainTex", theta.getTexture(), 0);
        shader.setUniforms(thetaParams);
        theta.draw(0, 0, 1280, 640);
        shader.end();
        fbo.end();
        
    }
    
    if(!showSphere){
        
        fbo.draw(0, 0, 1280, 640);
        
    }else{
        
        ofEnableDepthTest();
        cam.begin();
        fbo.getTexture().bind();
        sphere.draw();
        fbo.getTexture().unbind();
        cam.end();
        
    }
    
    ofDisableDepthTest();
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
