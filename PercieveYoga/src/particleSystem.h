#pragma once
#include "ofMain.h"
#include <ofxGpuParticles.h>

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void setup(const int width, const int height);
    void update();
    void draw();

    void onUpdateEvent(ofShader& shader);
    void onDrawEvent(ofShader& shader);

    ofVec2f getCenterPoint() { return center_pos_; }
    void setCenterPoint(const ofVec2f& point);
    void setRadius(const float radius);

private:
    static constexpr float kNormalRadius = 500.f;
    static constexpr float kDeepRadius   = 700.f;
    static constexpr float kZenRadius    = 900.f;

    ofxGpuParticles particles_;
    ofVec2f center_pos_;
    float radius_;
    int width_;
    int height_;
    ofFloatColor color_;
    ofEasyCam cam_;

};

