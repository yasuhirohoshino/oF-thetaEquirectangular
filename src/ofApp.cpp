#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    loadShader();
    
    srcFbo.allocate(resolution.x, resolution.y, GL_RGB);
    dstFbo.allocate(resolution.x, resolution.y, GL_RGB);
    
    sphereMesh = ofSpherePrimitive(3000, 64).getMesh();
    for(int i=0;i<sphereMesh.getNumTexCoords();i++){
        sphereMesh.setTexCoord(i, (ofVec2f(1.0) - sphereMesh.getTexCoord(i)) * ofVec2f(resolution.x, resolution.y));
    }
    for(int i=0;i<sphereMesh.getNumNormals();i++){
        sphereMesh.setNormal(i, sphereMesh.getNormal(i) * ofVec3f(-1));
    }
    
    if (bmdInput.setup(0)) {
        bmdInput.setDrawMode(ofxDeckLinkAPI::Input::DRAWMODE_PROGRESSIVE);
        bmdInput.start(bmdModeHD1080p30);
    }
    
    camera[0].radius[0] = 425;
    camera[0].centerPos[0] = ofVec2f(478, 478);
    camera[0].rotate[0] = 0.000;
    
    camera[0].radius[1] = 425;
    camera[0].centerPos[1] = ofVec2f(478, 478);
    camera[0].rotate[1] = 0.000;
    
    camera[1].radius[0] = 425;
    camera[1].centerPos[0] = ofVec2f(1438, 478);
    camera[1].rotate[0] = 0.000;
    
    camera[1].radius[1] = 425;
    camera[1].centerPos[1] = ofVec2f(1438, 478);
    camera[1].rotate[1] = 0.000;
    
    loadJSON();
    
    gui.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    ofDisableDepthTest();
    
    bmdInput.update();
    
    srcFbo.begin();
    bmdInput.draw(0, 0);
//    testImage.draw(0, 0);
    srcFbo.end();
    
    dstFbo.begin();
    ofClear(0);
    shader.begin();
    shader.setUniformTexture("source", srcFbo.getTexture(), 0);
    shader.setUniform2f("resolution", resolution);
    shader.setUniform1f("blendingRatio", blendingRatio);
    shader.setUniform1f("translate", translate);
    for(int i=0; i<2; i++){
        string index = ofToString(i);
        shader.setUniform1fv("cam[" + index + "].radius[0]", &camera[i].radius[0], 2);
        shader.setUniform2fv("cam[" + index + "].centerPos[0]", &camera[i].centerPos[0].x, 2);
        shader.setUniform1fv("cam[" + index + "].rotate[0]", &camera[i].rotate[0], 2);
    }
    srcFbo.draw(0, 0, resolution.x, resolution.y);
    shader.end();
    dstFbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch (appMode) {
        case 0:
            srcFbo.draw(0, 0);
            break;
            
        case 1:
            dstFbo.draw(0,0);
            break;
            
        case 2:
            ofEnableDepthTest();
            easyCam.begin();
            dstFbo.getTexture().bind();
            sphereMesh.draw();
            dstFbo.getTexture().unbind();
            easyCam.end();
            ofDisableDepthTest();
            break;
            
        default:
            break;
    }
    
    drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui(){
    gui.begin();
    {
        if(ImGui::Button("reload shader")){
            loadShader();
        }
        ImGui::SameLine();
        if(ImGui::Button("reload json")){
            loadJSON();
        }
        ImGui::Spacing();
        
        ImGui::Text("parameters");
        ImGui::DragFloat("blendingRatio", &blendingRatio, 0.01, 0.0, 1.0);
        ImGui::DragFloat("translate", &translate, 1);
        ImGui::Spacing();
        
        for(int i=0;i<2;i++){
            string name = "camera " + ofToString(i);
            ImGui::PushID(name.c_str());
            ImGui::Text("%s", name.c_str());
            ImGui::DragFloat("radius 1", &camera[i].radius[0], 0.1);
            ImGui::DragFloat2("center pos 1", &camera[i].centerPos[0].x, 0.1);
            ImGui::DragFloat("rotate 1", &camera[i].rotate[0], 0.0001);
            ImGui::DragFloat("radius 2", &camera[i].radius[1], 0.1);
            ImGui::DragFloat2("center pos 2", &camera[i].centerPos[1].x, 0.1);
            ImGui::DragFloat("rotate 2", &camera[i].rotate[1], 0.0001);
            ImGui::PopID();
            ImGui::Spacing();
        }
        
        ImGui::Text("mode");
        if(ImGui::RadioButton("Dual-fish eye", &appMode, 0)){
            easyCam.disableMouseInput();
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Equirectangular", &appMode, 1)){
            easyCam.disableMouseInput();
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Sphere", &appMode, 2)){
            easyCam.enableMouseInput();
        }
        if(appMode == 2){
            if(ImGui::Checkbox("easycam control", &enableCamControl)){
                if(enableCamControl){
                    easyCam.enableMouseInput();
                }else{
                    easyCam.disableMouseInput();
                }
            }
        }
        ImGui::Spacing();
        
        if(ImGui::Button("save json")){
            saveJSON();
        }
    }
    gui.end();
}

//--------------------------------------------------------------
void ofApp::loadShader(){
    if(ofIsGLProgrammableRenderer()){
        shader.load("shaders/gl3/equirectanguler.vert", "shaders/gl3/equirectanguler.frag");
    }else{
        shader.load("", "shaders/gl2/equirectanguler.frag");
    }
}

//--------------------------------------------------------------
void ofApp::loadJSON(){
    bool isLoaded = json.openLocal("thetaParams.json");
    if(isLoaded){
        blendingRatio = json["root"]["blendingRatio"].asFloat();
        translate = json["root"]["translate"].asFloat();
        for(int i=0;i<2;i++){
            for(int j=0;j<2;j++){
                camera[i].radius[j] = json["root"]["cam"][i]["radius"][j].asFloat();
                camera[i].centerPos[j].x = json["root"]["cam"][i]["centerPos"][j]["x"].asFloat();
                camera[i].centerPos[j].y = json["root"]["cam"][i]["centerPos"][j]["y"].asFloat();
                camera[i].rotate[j] = json["root"]["cam"][i]["rotate"][j].asFloat();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::saveJSON(){
    json["root"]["blendingRatio"] = blendingRatio;
    json["root"]["translate"] = translate;
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            json["root"]["cam"][i]["radius"][j] = camera[i].radius[j];
            json["root"]["cam"][i]["centerPos"][j]["x"] = camera[i].centerPos[j].x;
            json["root"]["cam"][i]["centerPos"][j]["y"] = camera[i].centerPos[j].y;
            json["root"]["cam"][i]["rotate"][j] = camera[i].rotate[j];
        }
    }
    json.save("thetaParams.json");
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
