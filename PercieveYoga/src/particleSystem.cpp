#include "ParticleSystem.h"


ParticleSystem::ParticleSystem() {
    // none
}

ParticleSystem::~ParticleSystem() {
    // none
}

void ParticleSystem::setup(const int width, const int height) {
    width_        = width;
    height_       = height;
    size_         = 1.0f;
    center_pos_.x = width_  * .5f;
    center_pos_.y = height_ * .5f;
    radius_       = kNormalRadius;
    color_.set(0.9, 0.9, 0.2, 0.1);

    unsigned w = 1000;
    unsigned h = 1000;
    particles_.init(w, h);
    particles_.loadShaders("shader/update", "shader/draw");

    float* initial_values = new float[w * h * 4];
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = w * y + x;
            initial_values[idx * 4 + 0] = (200.f * y / (float)h - 200.f) * sin(360.f * x / (float)w);
            initial_values[idx * 4 + 1] = (200.f * y / (float)h - 200.f) * cos(360.f * x / (float)w);
            initial_values[idx * 4 + 2] = 0.f; // particle z
            initial_values[idx * 4 + 3] = 0.f; // dummy
        }
    }
    particles_.loadDataTexture(ofxGpuParticles::POSITION, initial_values);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = w * y + x;
            initial_values[idx * 4 + 0] = ofRandom(100.f, -100.f);
            initial_values[idx * 4 + 1] = ofRandom(100.f, -100.f);
            initial_values[idx * 4 + 2] = 0.f; // particle z
            initial_values[idx * 4 + 3] = 0.f; // dummy
        }
    }
    particles_.loadDataTexture(ofxGpuParticles::VELOCITY, initial_values);
    delete[] initial_values;

    // listen for update event to set additonal update uniforms
    ofAddListener(particles_.updateEvent, this, &ParticleSystem::onUpdateEvent);
    ofAddListener(particles_.drawEvent,   this, &ParticleSystem::onDrawEvent);
}

void ParticleSystem::update() {
    particles_.update();
}

void ParticleSystem::draw() {
    cam_.begin();
    glPointSize(size_);
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    particles_.draw();
    ofDisableBlendMode();
    cam_.end();
}

void ParticleSystem::onUpdateEvent(ofShader& shader) {
    ofVec3f target_pos(center_pos_.x - width_ * 0.5f,
                       height_ * 0.5f - center_pos_.y);
    shader.setUniform3fv("target_pos",   target_pos.getPtr());
    shader.setUniform1f("elapsed",       ofGetLastFrameTime());
    shader.setUniform1f("radiusSquared", radius_ * radius_);
    shader.setUniform1f("resolution_x",  width_);
    shader.setUniform1f("resolution_y",  height_);
}

void ParticleSystem::onDrawEvent(ofShader& shader) {
    shader.setUniform4f("color", color_);
}

void ParticleSystem::setCenterPoint(const ofVec2f& point) {
    center_pos_.x = point.x;
    center_pos_.y = point.y;
}

void ParticleSystem::setRadius(const float radius) {
    radius_ = radius;
}

void ParticleSystem::setColor(const ofColor& color) {
    color_ = color;
}

