#include "YogaStateManager.h"

YogaStateManager::YogaStateManager() :
    current_state_(kNotReady),
    is_pose_keeping_(false),
    is_good_pose_(false),
    pose_goodness_(0),
    last_pose_id_(0.0) {
    // none
}

YogaStateManager::~YogaStateManager() {
    // none
}

void YogaStateManager::update(const YogaPoseID pose_id, const double proba) {
    switch (current_state_) {
        case kNotReady:
            doGetReady(pose_id, proba);
            break;
        case kPracticing:
            doPractice(pose_id, proba);
            break;
        case kInZone:
            doYogaDeep(pose_id, proba);
            break;
        default:
            ofLogError("YogaStateManager") << "Invalid state.. ret : " << current_state_;
            break;
    }

    updatePoseGoodness();
    last_pose_id_ = pose_id;
}

bool YogaStateManager::isPracticing() {
    if (current_state_ == kPracticing) {
        return true;
    } else {
        return false;
    }
}

bool YogaStateManager::isInZone() {
    if (current_state_ == kInZone) {
        return true;
    } else {
        return false;
    }
}

bool YogaStateManager::isGoodPose() {
    return is_good_pose_;
}

uint32_t YogaStateManager::getPoseGoodness() {
    return pose_goodness_;
}

void YogaStateManager::doGetReady(const YogaPoseID pose_id, const double proba) {
    if ((pose_id == kPoseReady) && (proba > 0.9)) {
        if (is_pose_keeping_) {
            if (getPoseKeepTimeMS() > kEnoughKeepTimeMS) {
                current_state_ = kPracticing;
                is_pose_keeping_ = false;
            }
        } else {
            is_pose_keeping_ = true;
            pose_keep_start_time_ = std::chrono::system_clock::now();
        }
    } else {
        is_pose_keeping_ = false;
    }
}

void YogaStateManager::doPractice(const YogaPoseID pose_id, const double proba) {
    // TODO functionalize condition checks
    if ((pose_id > kPoseReady) &&
        (last_pose_id_ == pose_id) &&
        (proba > 0.8)) {
        if (is_pose_keeping_) {
            if (getPoseKeepTimeMS() > kEnoughKeepTimeMS) {
                is_good_pose_ = true;
            }
        } else {
            is_pose_keeping_ = true;
            pose_keep_start_time_ = std::chrono::system_clock::now();
        }
    } else if ((pose_id == kPoseReady) &&
               (last_pose_id_ == kPoseReady) &&
               (proba > 0.9)) {
        if (is_pose_keeping_) {
            if (getPoseKeepTimeMS() > kEnoughKeepTimeMS) {
                current_state_   = kInZone;
                is_pose_keeping_ = false;
            }
        } else {
            is_pose_keeping_ = true;
            pose_keep_start_time_ = std::chrono::system_clock::now();
        }
    } else {
        is_pose_keeping_   = false;
        is_good_pose_ = false;
    }
}

void YogaStateManager::doYogaDeep(const YogaPoseID pose_id, const double proba) {
    if ((pose_id == kPoseReady) && (proba > 0.9)) {
        if (is_pose_keeping_) {
            if (getPoseKeepTimeMS() > kEnoughKeepTimeMS) {
                current_state_   = kPracticing;
                is_pose_keeping_ = false;
            }
        } else {
            is_pose_keeping_ = true;
            pose_keep_start_time_ = std::chrono::system_clock::now();
        }
    } else {
        is_pose_keeping_ = false;
    }
}

double YogaStateManager::getPoseKeepTimeMS() {
    std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
    double keep_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - pose_keep_start_time_).count();

    return keep_time;
}

void YogaStateManager::updatePoseGoodness() {
    if (is_good_pose_) {
        if (pose_goodness_ != kPoseGoodnessMax) {
            pose_goodness_ += kPoseGoodnessDelta;
        }
    } else {
        if (pose_goodness_ != 0) {
            pose_goodness_ -= kPoseGoodnessDelta;
        }
    }
}