#pragma once

#include <vector>
#include <string>

#include "ofMain.h"
#include "MyKinect2.h"
#include "PredictYogaPose.h"
#include "YogaStateManager.h"
#include "ParticleSystem.h"
#include "StringParticleSystem.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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

    private:
        static constexpr size_t kWindowWidth  = 1920;
        static constexpr size_t kWindowHeight = 1080;

        void updateParticlesCenter(const std::vector<ofVec2f>& joints);
        void updateStringParticle();
        void drawYoggy();
        void drawYoggyInZone();

        void initializePoseNameList();

        MyKinect2            kinect_;
        PredictYogaPose      predict_yoga_pose_;
        YogaStateManager     yoga_state_manager_;
        ParticleSystem       particle_;
        StringParticleSystem string_ps_;

        ofImage studio_image_;
        ofTrueTypeFont font_;

        double pose_id_;
        double proba_;
        std::vector<std::string> pose_name_list_;
};
