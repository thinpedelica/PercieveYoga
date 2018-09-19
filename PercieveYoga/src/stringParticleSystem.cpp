#include "stringParticleSystem.h"

StringParticleSystem::StringParticleSystem() :
    x_skip_(4),
    y_skip_(4),
    size_(2),
    color_(ofFloatColor(0.9, 0.9, 0.2, 0.1)) {
    font_.loadFont("fonts/georgiab.ttf", 80, true, true);
}

void StringParticleSystem::setup(const std::string& message,
                                 const ofVec2f& start_pos) {
    message_ = message;

    createPixelsMessage();
    addParticles(start_pos);
}

void StringParticleSystem::update(const float delta) {
    for (Particle& p : particles_) {
        p.update(delta);
    }
}

void StringParticleSystem::draw() {
    ofSetColor(color_);
    for (Particle& p : particles_) {
        p.draw();
    }
}

void StringParticleSystem::addParticles(const ofVec2f& start_pos) {
    particles_.clear();

    float end_x = string_pixels_.getWidth();
    float end_y = string_pixels_.getHeight();
    for (int x = 0; x < end_x; x += x_skip_) {
        for (int y = 0; y < end_y; y += y_skip_) {
            ofColor pixel_color = string_pixels_.getColor(x, y);
            if (pixel_color == ofColor(0)) {
                // this pixel is on message
                particles_.push_back(Particle(start_pos.x,
                                              start_pos.y,
                                              ofGetWidth()  * 0.5f + x - end_x * 0.5f,
                                              ofGetHeight() * 0.9f + y - end_y * 0.5f,
                                              size_,
                                              ofRandom(0.5, 3)));
            }
        }
    }
}

void StringParticleSystem::createPixelsMessage() {
    fbo_.clear();
    fbo_.allocate(ofGetWidth(), ofGetHeight());

    string_pixels_.clear();

    fbo_.begin();
    ofBackground(255);
    ofSetColor(0);
    font_.drawString(message_,
                     ofGetWidth() * 0.5f - font_.stringWidth(message_) * 0.5f,
                     ofGetHeight() * 0.5f);

    fbo_.end();
    fbo_.readToPixels(string_pixels_);
}

void StringParticleSystem::reset(const int x, const int y) {
    for (Particle& p : particles_) {
        p.reset(x, y);
    }
}

void StringParticleSystem::setColor(const ofColor& color) {
    color_ = color;
}

