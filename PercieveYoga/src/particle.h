#pragma once

#include "ofMain.h"

class Particle {
public:
    Particle(const float start_x, const float start_y,
             const float end_x,   const float end_y,
             const float size, 
             const float duration,
             const ofColor& color);
    void reset(const float start_x, const float start_y);

    void update(const float delta);
    void draw();

private:
    float interpolate(float f);

    ofVec2f start_;
    ofVec2f anchor_;
    ofVec2f end_;
    ofVec2f current_;
    
    float size_;
    float duration_;
    float progress_;
    ofColor color_;
};