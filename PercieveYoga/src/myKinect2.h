#pragma once
#include "ofMain.h"
#include <vector>
#include <kinect.h>

class MyKinect2 {
public:
    static constexpr BYTE kNoBodyValue = 255;

    MyKinect2();
    ~MyKinect2();

    bool setup();
    bool update();
    bool getColorImageData(ofTexture&tex, const bool segmentation = false);
    void getColorImageSize(int& width, int& height);
    bool getBodyJointPositionRGB(std::vector<ofVec2f>& joints);
    bool getDepthData(std::vector<USHORT>& depth, const bool segmentation = false);
    void getDepthSize(int& width, int& height);
    bool getColorMappedBodyIndex(std::vector<BYTE>& body_index);

protected:
    bool updateColorImageData(IMultiSourceFrame* frame);
    bool updateDepthImageData(IMultiSourceFrame* frame);
    bool updateBodyIndex(IMultiSourceFrame* frame);
    bool updateBody(IMultiSourceFrame* frame);
    bool exit();

private:
    // for Kinect2 Basic
    IKinectSensor* kinect_;
    IMultiSourceFrameReader* multi_frame_reader_;
    ICoordinateMapper* mapper_;

    // for Color Image
    std::vector<BYTE> color_buffer_;
    int color_width_;
    int color_height_;
    unsigned int color_byte_per_pixel_;

    // for Depth Image
    std::vector<USHORT> depth_buffer_;
    int depth_width_;
    int depth_height_;

    // for Body Index
    std::vector<BYTE> body_index_buffer_;

    // for Body
    IBody* bodies_[BODY_COUNT];
};

