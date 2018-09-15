#pragma once

#include <string>
#include <vector>
#include "ofMain.h"
#include "particle.h"

class StringParticleSystem {
public:
    StringParticleSystem();

    void setup(const std::string& message,
               const ofVec2f& start_pos);
    void update(const float delta);
    void draw();
    void reset(const int x, const int y);
private:
    void addParticles(const ofVec2f& start_pos);
    void createPixelsMessage();

    ofFbo fbo_;
    ofPixels string_pixels_;
    ofTrueTypeFont font_;

    std::string message_;
    int x_skip_;
    int y_skip_;
    size_t size_;
    ofColor color_;

    std::vector<Particle> particles_;
};