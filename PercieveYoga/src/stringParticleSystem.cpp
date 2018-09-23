#include "stringParticleSystem.h"

StringParticleSystem::StringParticleSystem() :
    x_skip_(2),
    y_skip_(2),
    size_(3.0),
    color_(ofFloatColor(0.9, 0.9, 0.2, 0.1)),
    start_pos_(0.f, 0.f),
    delta_(0.f),
    progress_(0.f) {
    font_.loadFont("fonts/georgiab.ttf", 80, true, true);

    particles_.init(kResolutionX, kResolutionY);
    particles_.loadShaders("shader/update_string", "shader/draw_string");

    ofAddListener(particles_.updateEvent, this, &StringParticleSystem::onUpdateEvent);
    ofAddListener(particles_.drawEvent,   this, &StringParticleSystem::onDrawEvent);
}

void StringParticleSystem::setup(const std::string& message,
                                 const ofVec2f& start_pos) {
    message_ = message;

    createPixelsMessage();
    initializeParticles(start_pos);
}

void StringParticleSystem::update(const float delta) {
    delta_ = delta;
    particles_.update();
}

void StringParticleSystem::draw() {
    ofSetColor(color_);
    glPointSize(size_);
    particles_.draw();
}

void StringParticleSystem::onUpdateEvent(ofShader& shader) {
    shader.setUniform2fv("start_pos",   start_pos_.getPtr());
    shader.setUniform1f("delta",        delta_);
}

void StringParticleSystem::onDrawEvent(ofShader& shader) {
    shader.setUniform4f("color", color_);
}

void StringParticleSystem::initializeParticles(const ofVec2f& start_pos) {
    start_pos_ = start_pos;
    progress_  = 0.f;
    float* initial_val1 = new float[kResolutionX * kResolutionY * 4];
    float* initial_val2 = new float[kResolutionX * kResolutionY * 4];

    float end_x = string_pixels_.getWidth();
    float end_y = string_pixels_.getHeight();
    uint32_t index = 0;
    for (int x = 0; x < end_x; x += x_skip_) {
        for (int y = 0; y < end_y; y += y_skip_) {
            ofColor pixel_color = string_pixels_.getColor(x, y);
            if (pixel_color == ofColor(0)) {
                // this pixel is on message
                float angle = ofRandom(TWO_PI);
                ofVec2f anchor = ofVec2f(cos(angle) * ofRandom(100, 800) + ofGetWidth()  * 0.5,
                                         sin(angle) * ofRandom(100, 800) + ofGetHeight() * 0.5);
                initial_val1[index * 4 + 0] = ofGetWidth()  * 0.5f + x - end_x * 0.5f;   // end_pos_x
                initial_val1[index * 4 + 1] = ofGetHeight() * 0.9f + y - end_y * 0.5f;   // end_pos_y
                initial_val1[index * 4 + 2] = anchor.x;                                  // anchor.x
                initial_val1[index * 4 + 3] = anchor.y;                                  // anchor.y

                initial_val2[index * 4 + 0] = 0.f;                                       // current pos x
                initial_val2[index * 4 + 1] = 0.f;                                       // current pos y
                initial_val2[index * 4 + 2] = 0.f;                                       // progress
                initial_val2[index * 4 + 3] = ofRandom(0.5, 3);                          // duration
                ++index;
            }
        }
    }

    uint32_t index_max = kResolutionX * kResolutionY;
    for (; index < index_max; ++index) {
        initial_val2[index * 4 + 2] = -1.f;   // if progress is under zero, its not used
    }

    particles_.loadDataTexture(ofxGpuParticles::POSITION, initial_val1);
    particles_.loadDataTexture(ofxGpuParticles::VELOCITY, initial_val2);
    delete[] initial_val1;
    delete[] initial_val2;
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

void StringParticleSystem::setColor(const ofColor& color) {
    color_ = color;
}

