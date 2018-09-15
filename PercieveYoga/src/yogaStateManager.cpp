#include "YogaStateManager.h"

YogaStateManager::YogaStateManager() :
    current_state_(kNotReady),
    is_pose_keeping_(false),
    is_good_yoga_pose_(false),
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
    if (current_state_ == kInZone) {
        // always good pose if in zone :)
        return true;
    }
    return is_good_yoga_pose_;
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
    if ((pose_id > kPoseReady) &&
        (last_pose_id_ == pose_id) &&
        (proba > 0.8)) {
        if (is_pose_keeping_) {
            if (getPoseKeepTimeMS() > kEnoughKeepTimeMS) {
                is_good_yoga_pose_ = true;
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
        is_good_yoga_pose_ = false;
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