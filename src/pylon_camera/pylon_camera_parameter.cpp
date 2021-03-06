/******************************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright (C) 2016, Magazino GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Magazino GmbH nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <pylon_camera/pylon_camera_parameter.h>

namespace pylon_camera
{

PylonCameraParameter::PylonCameraParameter() :
        camera_frame_("pylon_camera"),
        device_user_id_(""),
        device_serial_number_(""),
        frame_rate_(5.0),
        camera_info_url_(""),
        image_encoding_("bayer_rg8"),
        binning_x_(1),
        binning_y_(1),
        binning_x_given_(false),
        binning_y_given_(false),
        image_height_(1200),
        image_width_(1600),
        // ##########################
        //  image intensity settings
        // ##########################
        exposure_(10000.0),
        exposure_given_(false),
        gain_(0.5),
        gain_given_(false),
        gamma_(1.0),
        gamma_given_(false),
        brightness_(100),
        brightness_given_(false),
        brightness_continuous_(false),
        exposure_auto_(true),
        gain_auto_(true),
        // #########################
        mtu_size_(3000),
        inter_pkg_delay_(1000),
        shutter_mode_(SM_DEFAULT),
        // #########################
        light_source_preset_(0),
        balance_white_auto_(0),
        balance_ratio_selector_(2),
        balance_ratio_(2.56),
        overlap_mode_on_(false)
{}

PylonCameraParameter::~PylonCameraParameter()
{}

void PylonCameraParameter::readFromRosParameterServer(const ros::NodeHandle& nh)
{
    nh.param<std::string>("camera_frame", camera_frame_, "pylon_camera");

    nh.param<std::string>("device_user_id", device_user_id_, "");

    nh.param<std::string>("device_serial_number", device_serial_number_, "");

    if ( nh.hasParam("frame_rate") )
    {
        nh.getParam("frame_rate", frame_rate_);
    }

    nh.param<std::string>("camera_info_url", camera_info_url_, "");
    if ( nh.hasParam("camera_info_url") )
    {
        nh.getParam("camera_info_url", camera_info_url_);
    }

    nh.param<std::string>("image_encoding", image_encoding_, "mono8");

    nh.param("image_height", image_height_, 1200);

    nh.param("image_width", image_width_, 1600);

    binning_x_given_ = nh.hasParam("binning_x");
    if ( binning_x_given_ )
    {
        int binning_x;
        nh.getParam("binning_x", binning_x);
        std::cout << "binning x is given and has value " << binning_x << std::endl;
        if ( binning_x > 32 || binning_x < 0 )
        {
            ROS_WARN_STREAM("Desired horizontal binning_x factor not in valid "
                << "range! Binning x = " << binning_x << ". Will reset it to "
                << "default value (1)");
            binning_x_given_ = false;
        }
        else
        {
            binning_x_ = static_cast<size_t>(binning_x);
        }
    }
    binning_y_given_ = nh.hasParam("binning_y");
    if ( binning_y_given_ )
    {
        int binning_y;
        nh.getParam("binning_y", binning_y);
        std::cout << "binning y is given and has value " << binning_y << std::endl;
        if ( binning_y > 32 || binning_y < 0 )
        {
            ROS_WARN_STREAM("Desired vertical binning_y factor not in valid "
                << "range! Binning y = " << binning_y << ". Will reset it to "
                << "default value (1)");
            binning_y_given_ = false;
        }
        else
        {
            binning_y_ = static_cast<size_t>(binning_y);
        }
    }

    // ##########################
    //  image intensity settings
    // ##########################

    // > 0: Exposure time in microseconds
    exposure_given_ = nh.hasParam("exposure");
    if ( exposure_given_ )
    {
        nh.getParam("exposure", exposure_);
        std::cout << "exposure is given and has value " << exposure_ << std::endl;
    }

    gain_given_ = nh.hasParam("gain");
    if ( gain_given_ )
    {
        nh.getParam("gain", gain_);
        std::cout << "gain is given and has value " << gain_ << std::endl;
    }

    gamma_given_ = nh.hasParam("gamma");
    if ( gamma_given_ )
    {
        nh.getParam("gamma", gamma_);
        std::cout << "gamma is given and has value " << gamma_ << std::endl;
    }

    brightness_given_ = nh.hasParam("brightness");
    if ( brightness_given_ )
    {
        nh.getParam("brightness", brightness_);
        std::cout << "brightness is given and has value " << brightness_
            << std::endl;
        if ( gain_given_ && exposure_given_ )
        {
            ROS_WARN_STREAM("Gain ('gain') and Exposure Time ('exposure') "
                << "are given as startup ros-parameter and hence assumed to be "
                << "fix! The desired brightness (" << brightness_ << ") can't "
                << "be reached! Will ignore the brightness by only "
                << "setting gain and exposure . . .");
            brightness_given_ = false;
        }
        else
        {
            if ( nh.hasParam("brightness_continuous") )
            {
                nh.getParam("brightness_continuous", brightness_continuous_);
                std::cout << "brightness is continuous" << std::endl;
            }
            if ( nh.hasParam("exposure_auto") )
            {
                nh.getParam("exposure_auto", exposure_auto_);
                std::cout << "exposure is set to auto" << std::endl;
            }
            if ( nh.hasParam("gain_auto") )
            {
                nh.getParam("gain_auto", gain_auto_);
                std::cout << "gain is set to auto" << std::endl;
            }
        }
    }
    // ##########################

    if ( nh.hasParam("gige/mtu_size") )
    {
        nh.getParam("gige/mtu_size", mtu_size_);
    }

    if ( nh.hasParam("gige/inter_pkg_delay") )
    {
        nh.getParam("gige/inter_pkg_delay", inter_pkg_delay_);
    }

    std::string shutter_param_string;
    nh.param<std::string>("shutter_mode", shutter_param_string, "");
    if ( shutter_param_string == "rolling" )
    {
        shutter_mode_ = SM_ROLLING;
    }
    else if ( shutter_param_string == "global" )
    {
        shutter_mode_ = SM_GLOBAL;
    }
    else if ( shutter_param_string == "global_reset" )
    {
        shutter_mode_ = SM_GLOBAL_RESET_RELEASE;
    }
    else
    {
        shutter_mode_ = SM_DEFAULT;
    }

    // ##########################

    if ( nh.hasParam("usb/light_source_preset") )
    {
        nh.getParam("usb/light_source_preset", light_source_preset_);
    }

    if ( nh.hasParam("usb/balance_white_auto") )
    {
        nh.getParam("usb/balance_white_auto", balance_white_auto_);
    }

    if ( nh.hasParam("usb/balance_ratio_selector") )
    {
        nh.getParam("usb/balance_ratio_selector", balance_ratio_selector_);
    }

    if ( nh.hasParam("usb/balance_ratio") )
    {
        nh.getParam("usb/balance_ratio", balance_ratio_);
    }

    if ( nh.hasParam("usb/overlap_mode_on") )
    {
        nh.getParam("usb/overlap_mode_on", overlap_mode_on_);
    }

    validateParameterSet(nh);
    return;
}

void PylonCameraParameter::validateParameterSet(const ros::NodeHandle& nh)
{
    if ( !device_user_id_.empty() )
    {
        ROS_INFO_STREAM("Trying to open the following camera: "
            << device_user_id_.c_str());
    }
    else
    {
        ROS_INFO_STREAM("No Device User ID set -> Will open the camera device "
                << "found first");
    }

    if ( frame_rate_ < 0 && frame_rate_ != -1 )
    {
        ROS_WARN_STREAM("Unexpected frame rate (" << frame_rate_ << "). Will "
                << "reset it to default value which is 5 Hz");
        frame_rate_ = 5.0;
        nh.setParam("frame_rate", frame_rate_);
    }

    if ( exposure_given_ && ( exposure_ <= 0.0 || exposure_ > 1e7 ) )
    {
        ROS_WARN_STREAM("Desired exposure measured in microseconds not in "
                << "valid range! Exposure time = " << exposure_ << ". Will "
                << "reset it to default value!");
        exposure_given_ = false;
    }

    if ( gain_given_ && ( gain_ < 0.0 || gain_ > 1.0 ) )
    {
        ROS_WARN_STREAM("Desired gain (in percent) not in allowed range! "
                << "Gain = " << gain_ << ". Will reset it to default value!");
        gain_given_ = false;
    }

    if ( brightness_given_ && ( brightness_ < 0.0 || brightness_ > 255 ) )
    {
        ROS_WARN_STREAM("Desired brightness not in allowed range [0 - 255]! "
               << "Brightness = " << brightness_ << ". Will reset it to "
               << "default value!");
        brightness_given_ = false;
    }

    return;
}

const std::string& PylonCameraParameter::deviceUserID() const
{
    return device_user_id_;
}

const std::string& PylonCameraParameter::deviceSerialNumber() const
{
    return device_serial_number_;
}

std::string PylonCameraParameter::shutterModeString() const
{
    if ( shutter_mode_ == SM_ROLLING )
    {
        return "rolling";
    }
    else if ( shutter_mode_ == SM_GLOBAL )
    {
        return "global";
    }
    else if ( shutter_mode_ == SM_GLOBAL_RESET_RELEASE )
    {
        return "global_reset";
    }
    else
    {
        return "default_shutter_mode";
    }
}

std::string PylonCameraParameter::imageEncoding() const
{
    return image_encoding_;
}

const std::string& PylonCameraParameter::cameraFrame() const
{
    return camera_frame_;
}

const double& PylonCameraParameter::frameRate() const
{
    return frame_rate_;
}

void PylonCameraParameter::setFrameRate(const ros::NodeHandle& nh,
                                        const double& frame_rate)
{
    frame_rate_ = frame_rate;
    nh.setParam("frame_rate", frame_rate_);
}

const std::string& PylonCameraParameter::cameraInfoURL() const
{
    return camera_info_url_;
}

void PylonCameraParameter::setCameraInfoURL(const ros::NodeHandle& nh,
                                            const std::string& camera_info_url)
{
    camera_info_url_ = camera_info_url;
    nh.setParam("camera_info_url", camera_info_url_);
}

}  // namespace pylon_camera
