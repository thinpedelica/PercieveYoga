#pragma once

#include <chrono>
#include "ofMain.h"

class YogaStateManager {
public:
    using YogaPoseID = uint32_t;

    enum YogaState {
        kNotReady = 0,
        kPracticing,
        kInZone
    };

    YogaStateManager();
    ~YogaStateManager();

    void update(const YogaPoseID pose_id, const double proba);
    bool isPracticing();
    bool isInZone();
    bool isGoodPose();
    uint32_t getPoseGoodness();
    
private:
    static constexpr YogaPoseID kPoseNotReady = 0;
    static constexpr YogaPoseID kPoseReady    = 1;

    static constexpr double kEnoughKeepTimeMS = 3000.0;
    static constexpr uint32_t kPoseGoodnessMax = 100;
    static constexpr uint32_t kPoseGoodnessDelta = 1;

    void doGetReady(const YogaPoseID pose_id, const double proba);
    void doPractice(const YogaPoseID pose_id, const double proba);
    void doYogaDeep(const YogaPoseID pose_id, const double proba);

    double getPoseKeepTimeMS();
    void updatePoseGoodness();

    uint32_t current_state_;
    bool is_pose_keeping_;
    bool is_good_pose_;
    uint32_t pose_goodness_;
    YogaPoseID last_pose_id_;
    std::chrono::system_clock::time_point pose_keep_start_time_;
};

