#pragma once

#include "ofMain.h"
#include <string>
#include <ofxGpuParticles.h>

class StringParticleSystem {
public:
    StringParticleSystem();

    void setup(const std::string& message,
               const ofVec2f& start_pos);
    void update(const float delta);
    void draw();

    void onUpdateEvent(ofShader& shader);
    void onDrawEvent(ofShader& shader);

    void setColor(const ofColor& color);

private:
    void initializeParticles(const ofVec2f& start_pos);
    void createPixelsMessage();

    static constexpr int kResolutionX = 200;
    static constexpr int kResolutionY = 200;

    ofxGpuParticles particles_;
    ofEasyCam cam_;

    ofFbo fbo_;
    ofPixels string_pixels_;
    ofTrueTypeFont font_;

    std::string message_;
    int x_skip_;
    int y_skip_;
    float size_;
    ofColor color_;

    ofVec2f start_pos_;
    float delta_;
    float progress_;

};