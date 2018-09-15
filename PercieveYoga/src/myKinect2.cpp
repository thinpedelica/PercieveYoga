#include "MyKinect2.h"

#include <algorithm>
#include <iterator>

MyKinect2::MyKinect2() : 
    kinect_(nullptr),
    multi_frame_reader_(nullptr),
    mapper_(nullptr),
    color_width_(0),
    color_height_(0),
    color_byte_per_pixel_(0),
    depth_width_(0),
    depth_height_(0) {
    color_buffer_.resize(0);
    depth_buffer_.resize(0);
    body_index_buffer_.resize(0);
    for (auto& body : bodies_) {
        body = nullptr;
    }
}

MyKinect2::~MyKinect2() {
    exit();
}

bool MyKinect2::setup() {
    // initialize device
    auto ret = GetDefaultKinectSensor(&kinect_);
    if (ret != S_OK) {
        ofLogError("MyKinect2") << "Failed to GetDefaultKinectSensor. ret : " << ret;
        return false;
    }
    ret = kinect_->Open();
    if (ret != S_OK) {
        ofLogError("MyKinect2") << "Failed to Open. ret : " << ret;
        return false;
    }
    // setup frame reader
    ret = kinect_->OpenMultiSourceFrameReader(FrameSourceTypes_Color |
                                              FrameSourceTypes_Depth |
                                              FrameSourceTypes_BodyIndex |
                                              FrameSourceTypes_Body,
                                              &multi_frame_reader_);
    if (ret != S_OK) {
        ofLogError("MyKinect2") << "Failed to OpenMultiSourceFrameReader. ret : " << ret;
        return false;
    }
    // setup color frame
    {
        IColorFrameSource* source;
        if (kinect_->get_ColorFrameSource(&source) != S_OK) {
            ofLogError("MyKinect2") << "Failed to get_ColorFrameSource.";
            return false;
        }
        IFrameDescription* frame_desc;
        source->CreateFrameDescription(ColorImageFormat_Bgra, &frame_desc);

        frame_desc->get_Width(&color_width_);
        frame_desc->get_Height(&color_height_);
        frame_desc->get_BytesPerPixel(&color_byte_per_pixel_);

        color_buffer_.resize(color_width_ * color_height_ * color_byte_per_pixel_);

        frame_desc->Release();
        frame_desc = nullptr;
    }
    // setup depth frame
    {
        IDepthFrameSource* source;
        if (kinect_->get_DepthFrameSource(&source) != S_OK) {
            ofLogError("MyKinect2") << "Failed to get_DepthFrameSource.";
            return false;
        }
        IFrameDescription* frame_desc;
        if (source->get_FrameDescription(&frame_desc) != S_OK) {
            ofLogError("MyKinect2") << "Failed to get_FrameDescription.";
            return false;
        }

        frame_desc->get_Width(&depth_width_);
        frame_desc->get_Height(&depth_height_);

        depth_buffer_.resize(depth_width_ * depth_height_);

        frame_desc->Release();
        frame_desc = nullptr;
    }
    // setup body index frame
    {
        body_index_buffer_.resize(depth_width_ * depth_height_);
    }
    // setup body frame
    {
        for (auto& body : bodies_) {
            body = nullptr;
        }
    }
    // setup mapper
    kinect_->get_CoordinateMapper(&mapper_);

    return true;
}

bool MyKinect2::update() {
    IMultiSourceFrame* frame = NULL;
    auto ret = multi_frame_reader_->AcquireLatestFrame(&frame);
    if (ret != S_OK) {
        // just not ready
        return true;
    }
    if (updateColorImageData(frame) != true) {
        return false;
    }
    if (updateDepthImageData(frame) != true) {
        return false;
    }
    if (updateBodyIndex(frame) != true) {
        return false;
    }
    if (updateBody(frame) != true) {
        return false;
    }

    return true;
}

bool MyKinect2::getColorImageData(ofTexture& tex, const bool segmentation) {
    if (segmentation) {
        std::vector<DepthSpacePoint> depth_space(color_width_ * color_height_);
        mapper_->MapColorFrameToDepthSpace(depth_buffer_.size(), depth_buffer_.data(),
                                           depth_space.size(),   depth_space.data());
        size_t color_buffer_size = color_width_ * color_height_;
        // TODO segmentation by shader
        for (size_t index = 0; index < color_buffer_size; ++index) {
            int depth_x = static_cast<int>(depth_space.at(index).X);
            int depth_y = static_cast<int>(depth_space.at(index).Y);
            // out of range of depth space -> segmentation
            if ((depth_x < 0) ||
                (depth_y < 0) ||
                (depth_x >= depth_width_) ||
                (depth_y >= depth_height_)) {
                color_buffer_[index * 4 + 3] = 0;  // alpha
                continue;
            }

            int depth_index = (depth_width_ * depth_y) + depth_x;
            int body_index  = body_index_buffer_.at(depth_index);
            // not body -> segmentation
            if (body_index == 255) {
                color_buffer_[index * 4 + 3] = 0;  // alpha
            }
        }
    }

    tex.loadData(static_cast<const unsigned char*>(color_buffer_.data()),
                 color_width_,
                 color_height_,
                 GL_RGBA);
    return true;
}

void MyKinect2::getColorImageSize(int& width, int& height) {
    width  = color_width_;
    height = color_height_;
}

bool MyKinect2::getBodyJointPositionRGB(std::vector<ofVec2f>& joints) {
    joints.clear();

    // return just one tracking body
    for (const auto& body : bodies_) {
        if (body == nullptr) {
            continue;
        }
        BOOLEAN is_tracked = false;
        body->get_IsTracked(&is_tracked);
        if (!is_tracked) {
            continue;
        }
        
        joints.resize(JointType_Count);
        size_t joint_count = 0;
        Joint camera_space_joints[JointType_Count];
        body->GetJoints(JointType_Count, camera_space_joints);
        for (const auto& camera_space_joint : camera_space_joints) {
            ColorSpacePoint color_space_point;
            mapper_->MapCameraPointToColorSpace(camera_space_joint.Position, &color_space_point);
            joints.at(joint_count).x = color_space_point.X;
            joints.at(joint_count).y = color_space_point.Y;
            ++joint_count;
        }
        break;  // just 1 body
    }
    return true;
}

bool MyKinect2::getDepthData(std::vector<USHORT>& depth, const bool segmentation) {
    // segmentation
    if (segmentation) {
        depth.resize(depth_buffer_.size());
        for (size_t index = 0; index < depth_buffer_.size(); ++index) {
            BYTE body_index = body_index_buffer_[index];
            if (body_index == kNoBodyValue) {
                // no body
                continue;
            }
            depth.at(index) = depth_buffer_.at(index);
        }
    } else {
        std::copy(depth_buffer_.begin(),
                  depth_buffer_.end(),
                  std::back_inserter(depth));
    }
    return true;
}

void MyKinect2::getDepthSize(int& width, int& height) {
    width  = depth_width_;
    height = depth_height_;
}

bool MyKinect2::getColorMappedBodyIndex(std::vector<BYTE>& body_index) {
    body_index.resize(color_width_ * color_height_);
    std::vector<DepthSpacePoint> depth_space(color_width_ * color_height_);
    mapper_->MapColorFrameToDepthSpace(depth_buffer_.size(),
                                       depth_buffer_.data(),
                                       depth_space.size(),
                                       depth_space.data());
    size_t color_buffer_size = color_width_ * color_height_;

    // TODO segmentation by shader
    for (size_t index = 0; index < color_buffer_size; ++index) {
        int depth_x = (int)depth_space.at(index).X;
        int depth_y = (int)depth_space.at(index).Y;
        if ((depth_x < 0) ||
            (depth_y < 0) ||
            (depth_x >= depth_width_) ||
            (depth_y >= depth_height_)) {
            body_index.at(index) = kNoBodyValue;
            // out of range of depth space
            continue;
        }

        int depth_index = (depth_width_ * depth_y) + depth_x;
        body_index.at(index) = body_index_buffer_.at(depth_index);
    }

    return true;
}

bool MyKinect2::updateColorImageData(IMultiSourceFrame* frame) {
    if (frame == NULL) {
        return false;
    }

    IColorFrame* color_frame;
    IColorFrameReference* frame_ref = NULL;
    frame->get_ColorFrameReference(&frame_ref);
    frame_ref->AcquireFrame(&color_frame);
    if (frame_ref) {
        frame_ref->Release();
    }
    if (color_frame) {
        auto ret = color_frame->CopyConvertedFrameDataToArray(color_buffer_.size(),
                                                              color_buffer_.data(),
                                                              ColorImageFormat_Rgba);
        color_frame->Release();
        if (ret != S_OK) {
            ofLogError("MyKinect2") << "Failed to CopyConvertedFrameDataToArray(). : " << ret;
            return false;
        }
    }

    return true;
}

bool MyKinect2::updateDepthImageData(IMultiSourceFrame* frame) {
    if (frame == NULL) {
        return false;
    }

    IDepthFrame* depth_frame;
    IDepthFrameReference* frame_ref = NULL;
    frame->get_DepthFrameReference(&frame_ref);
    frame_ref->AcquireFrame(&depth_frame);
    if (frame_ref) {
        frame_ref->Release();
    }
    if (depth_frame) {
        auto ret = depth_frame->CopyFrameDataToArray(depth_buffer_.size(),
                                                     depth_buffer_.data());
        depth_frame->Release();
        if (ret != S_OK) {
            ofLogError("MyKinect2") << "Failed to CopyFrameDataToArray(). : " << ret;
            return false;
        }
    }

    return true;
}

bool MyKinect2::updateBodyIndex(IMultiSourceFrame* frame) {
    if (frame == NULL) {
        return false;
    }

    IBodyIndexFrame* body_index_frame;
    IBodyIndexFrameReference* frame_ref = NULL;
    frame->get_BodyIndexFrameReference(&frame_ref);
    frame_ref->AcquireFrame(&body_index_frame);
    if (frame_ref) {
        frame_ref->Release();
    }
    if (body_index_frame) {
        auto ret = body_index_frame->CopyFrameDataToArray(body_index_buffer_.size(),
                                                          body_index_buffer_.data());
        body_index_frame->Release();
        if (ret != S_OK) {
            ofLogError("MyKinect2") << "Failed to CopyFrameDataToArray(). : " << ret;
            return false;
        }
    }

    return true;
}

bool MyKinect2::updateBody(IMultiSourceFrame* frame) {
    if (frame == NULL) {
        return false;
    }

    IBodyFrame* body_frame;
    IBodyFrameReference* frame_ref = NULL;
    frame->get_BodyFrameReference(&frame_ref);
    frame_ref->AcquireFrame(&body_frame);
    if (frame_ref) {
        frame_ref->Release();
    }
    if (body_frame) {
        auto ret = body_frame->GetAndRefreshBodyData(BODY_COUNT, &bodies_[0]);
        body_frame->Release();
        if (ret != S_OK) {
            ofLogError("MyKinect2") << "Failed to GetAndRefreshBodyData(). : " << ret;
            return false;
        }
    }

    return true;
}

bool MyKinect2::exit() {
    if (kinect_) {
        kinect_->Close();
        kinect_ = nullptr;
    }
    return true;
}
