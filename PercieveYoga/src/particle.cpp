#include "particle.h"

Particle::Particle(const float start_x, const float start_y,
                   const float end_x,   const float end_y,
                   const float size,
                   const float duration,
                   const ofColor& color) : 
    start_(start_x, start_y),
    end_(end_x, end_y),
    size_(size),
    duration_(duration),
    progress_(0),
    color_(color) {

    float angle = ofRandom(TWO_PI);
    anchor_ = ofVec2f(cos(angle) * ofRandom(100, 800) + ofGetWidth() * 0.5,
                      sin(angle) * ofRandom(100, 800) + ofGetHeight() * 0.5);
}

void Particle::reset(const float start_x, const float start_y) {
    start_ = ofVec2f(start_x, start_y);
    progress_ = 0;
}

void Particle::update(const float delta) {
    if (progress_ >= 1) {
        progress_ = 1;
    } else {
        progress_ += delta / duration_;
        if (progress_ > 1) {
            progress_ = 1;
        }
    }

    float fi = interpolate(progress_);
    float x = ofLerp(ofLerp(start_.x, anchor_.x, fi), ofLerp(anchor_.x, end_.x, fi), fi);
    float y = ofLerp(ofLerp(start_.y, anchor_.y, fi), ofLerp(anchor_.y, end_.y, fi), fi);
    current_.set(x, y);
}

void Particle::draw() {
    ofSetColor(color_);
    ofDrawCircle(current_.x, current_.y, size_);
}

float Particle::interpolate(float progress) {
    return 1 - pow(1 - progress, 2);
}

