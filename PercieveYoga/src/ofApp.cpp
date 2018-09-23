#include "ofApp.h"
#include <kinect.h>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetCircleResolution(64);
    ofEnableSmoothing();
    ofSetWindowShape(kWindowWidth, kWindowHeight);
    ofSetWindowPosition(0, 0);

    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofBackground(0);

    // initialize kinect
    if (kinect_.setup()) {
        exit();
    }

    // initialize Predict Yoga Pose
    if (!predict_yoga_pose_.setup()) {
        exit();
    }

    // initialize GPU Particles
    particle_.setup(kWindowWidth, kWindowHeight);

    // initialize background image
    studio_image_.load("image/studio.jpg");
    studio_image_.resize(kWindowWidth, kWindowHeight);

    // initialize font
    font_.loadFont("fonts/arial.ttf", 32);

    // initialize Yoga State => None
    pose_id_ = 0.0;
    proba_   = 0.0;

    // initialize string particles
    initializePoseNameList();
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect_.update();

    std::vector<ofVec2f> joints;
    kinect_.getBodyJointPositionRGB(joints);
    if (joints.empty() == false) {
        double last_pose_matching = 0.0;  // not using now
        predict_yoga_pose_.predict(joints, pose_id_, proba_, last_pose_matching);

        yoga_state_manager_.update(pose_id_, proba_);
        updateParticlesCenter(joints);
        updatePoseName();
    }

    uint32_t goodness = yoga_state_manager_.getPoseGoodness();
    if (goodness > 0) {
        particle_.update();
        string_ps_.update(1.0f / 120);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (yoga_state_manager_.isInZone()) {
        drawYoggyInZone();
    } else {
        drawYoggy();
    }

    // for Debug
    //ofSetColor(255, 0, 0);
    //font_.drawString(std::to_string(pose_id_), 100, 100);
    //font_.drawString(std::to_string(proba_),   100, 200);
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

void ofApp::updateParticlesCenter(const std::vector<ofVec2f>& joints) {
    // select highest position(closer to 0) from right hand, left hand, breast(SpineShoulder)
    JointType highest_point = JointType_SpineShoulder;
    if (joints.at(JointType_WristRight).y < joints.at(JointType_WristLeft).y) {
        if (joints.at(JointType_WristRight).y < joints.at(JointType_SpineShoulder).y) {
            highest_point = JointType_WristRight;
        } else {
            highest_point = JointType_SpineShoulder;
        }
    } else {
        if (joints.at(JointType_WristLeft).y < joints.at(JointType_SpineShoulder).y) {
            highest_point = JointType_WristLeft;
        } else {
            highest_point = JointType_SpineShoulder;
        }
    }

    particle_.setCenterPoint(ofVec2f(joints.at(highest_point).x,
                                     joints.at(highest_point).y));
}

void ofApp::updatePoseName() {
    static bool is_good_pose = false;
    if (yoga_state_manager_.isGoodPose()) {
        if (is_good_pose == false) {
            is_good_pose = true;
            string pose_name  = pose_name_list_.at(static_cast<size_t>(pose_id_));
            ofVec2f start_pos = particle_.getCenterPoint();

            string_ps_.setup(pose_name, start_pos);
        }
    } else {
        is_good_pose = false;
    }
}

void ofApp::drawYoggy() {
    ofBackground(0);
    ofSetColor(255);
    studio_image_.draw(0, 0);

    // when do practice yoga, erase background to be able to see studio image
    ofTexture tex;
    kinect_.getColorImageData(tex, yoga_state_manager_.isPracticing());
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    tex.draw(0, 0);

    uint32_t goodness = yoga_state_manager_.getPoseGoodness();
    if (goodness > 0) {
        particle_.setColor(ofFloatColor(0.9, 0.9, 0.2, static_cast<float>(goodness) * 0.001));
        string_ps_.setColor(ofFloatColor(0.9, 0.9, 0.2, static_cast<float>(goodness) * 0.01));

        particle_.draw();
        string_ps_.draw();
    }
}

void ofApp::drawYoggyInZone() {
    ofBackgroundGradient(ofColor(0), ofColor(50), OF_GRADIENT_LINEAR);

    std::vector<BYTE> body_index;
    kinect_.getColorMappedBodyIndex(body_index);

    int height, width;
    kinect_.getColorImageSize(width, height);

    for (int h = 0; h < height; h += 8) {
        for (int w = 0; w < width; w += 8) {
            size_t index = width * h + w;
            BYTE body_index_data = body_index.at(index);
            if (body_index_data != MyKinect2::kNoBodyValue) {
                ofSetColor(ofColor(255 * ofNoise(w, h * 0.025 - ofGetFrameNum() * 0.05)));
                ofDrawCircle(w, h, 2);
            }
        }
    }
}

void ofApp::initializePoseNameList() {
    pose_name_list_.push_back("Samastitihi");
    pose_name_list_.push_back("Om");
    pose_name_list_.push_back("Utthita Trikonasana");
    pose_name_list_.push_back("Utthita Parsvakonasana");
    pose_name_list_.push_back("Utthita Trikonasana");
    pose_name_list_.push_back("Utthita Parsvakonasana");
    pose_name_list_.push_back("Parivritta Trikonasana");
    pose_name_list_.push_back("Parivritta Trikonasana");
    pose_name_list_.push_back("Utthita Hasta Padangushthasana");
    pose_name_list_.push_back("Utthita Hasta Padangushthasana");
}

