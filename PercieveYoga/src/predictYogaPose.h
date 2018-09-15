#pragma once
#include <vector>
#include <Python.h>
#include <boost/python.hpp>
#include "ofMain.h"

class PredictYogaPose {
public:
    PredictYogaPose();
    ~PredictYogaPose();

    bool setup();
    bool predict(const std::vector<ofVec2f>& joints,
                 double& pose_id,
                 double& proba,
                 double& last_pose_matching);

private:
    static constexpr size_t kJointNum = 14;
    static constexpr size_t kPredictResultVals = 3;

    void initializeJointPosList();
    void convertJointsFromKinect2(const std::vector<ofVec2f>& in_joints,
                                   std::vector<ofVec2f>& out_joints);

    boost::python::api::object main_mod_;
    std::vector<uint32_t> joint_pos_list_;
};

