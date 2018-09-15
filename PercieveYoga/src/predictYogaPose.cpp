#include "PredictYogaPose.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <boost/foreach.hpp>
#include <kinect.h>

namespace py = boost::python;

PredictYogaPose::PredictYogaPose() {
    initializeJointPosList();
}

PredictYogaPose::~PredictYogaPose() {
    // none
}

bool PredictYogaPose::setup() {
    Py_Initialize();
    main_mod_ = py::import("__main__");
    auto main_ns  = main_mod_.attr("__dict__");

    try {
        std::string file_path = ofFilePath::getAbsolutePath("dnn/yoga_master_on_cpp.py", true);
        std::ifstream ifs(file_path.c_str());
        if (!ifs.is_open()) {
            ofLogError("PredictYogaPose") << "Failed to open dnn file. path : " << file_path;
            return false;
        }
        std::string script((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        py::exec(script.c_str(), main_ns);
    } catch (boost::python::error_already_set) {
        PyErr_Print();
        return false;
    }

    return true;
}

bool PredictYogaPose::predict(const std::vector<ofVec2f>& joints,
                              double& pose_id,
                              double& proba,
                              double& last_pose_matching) {
    if (joints.size() < kJointNum) {
        return false;
    }

    std::vector<ofVec2f> reordered_joints;
    convertJointsFromKinect2(joints, reordered_joints);

    try {
        boost::python::list skelton_pos_list;
        for (auto& joint : reordered_joints) {
            skelton_pos_list.append(joint.x);
            skelton_pos_list.append(joint.y);
        }
        auto result = main_mod_.attr("predeict_pose_from_cpp")(skelton_pos_list);
        using input_iterator = boost::python::stl_input_iterator<double>;
        input_iterator begin(result), end;

        std::vector<double> results(begin, end);
        if (results.size() != kPredictResultVals) {
            return false;
        }
        pose_id            = results.at(0);
        proba              = results.at(1);
        last_pose_matching = results.at(2);

    }
    catch (boost::python::error_already_set) {
        PyErr_Print();
        return false;
    }

    return true;
}

void PredictYogaPose::initializeJointPosList() {
    joint_pos_list_.resize(kJointNum);
    joint_pos_list_.at(0)  = JointType_Head;
    joint_pos_list_.at(1)  = JointType_SpineShoulder;
    joint_pos_list_.at(2)  = JointType_ShoulderLeft;
    joint_pos_list_.at(3)  = JointType_ElbowLeft;
    joint_pos_list_.at(4)  = JointType_WristLeft;
    joint_pos_list_.at(5)  = JointType_ShoulderRight;
    joint_pos_list_.at(6)  = JointType_ElbowRight;
    joint_pos_list_.at(7)  = JointType_WristRight;
    joint_pos_list_.at(8)  = JointType_HipLeft;
    joint_pos_list_.at(9)  = JointType_KneeLeft;
    joint_pos_list_.at(10) = JointType_AnkleLeft;
    joint_pos_list_.at(11) = JointType_HipRight;
    joint_pos_list_.at(12) = JointType_KneeRight;
    joint_pos_list_.at(13) = JointType_AnkleRight;
}

void PredictYogaPose::convertJointsFromKinect2(const std::vector<ofVec2f>& in_joints,
                                               std::vector<ofVec2f>& out_joints) {
    out_joints.resize(kJointNum);
    size_t index = 0;
    for (auto& out_joint : out_joints) {
        out_joint.x = in_joints.at(joint_pos_list_.at(index)).x;
        out_joint.y = in_joints.at(joint_pos_list_.at(index)).y;
        ++index;
    }
}
