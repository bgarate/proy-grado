//
// Created by bruno on 24/09/17.
//

#include <src/hal/WhiteBalanceMode.h>
#include "ConfigKeys.h"

DEFINE_CONFIG_KEY(Drone, Name, std::string)
DEFINE_CONFIG_KEY(Drone, Id, int)
DEFINE_CONFIG_KEY(Drone, FOV, double)
DEFINE_CONFIG_KEY(Drone, FrameSize, cv::Size)
DEFINE_CONFIG_KEY(Drone, CameraTilt, double)
DEFINE_CONFIG_KEY(Drone, CameraMatrix, cv::Mat)
DEFINE_CONFIG_KEY(Drone, DistortionCoefficients, cv::Mat)

DEFINE_CONFIG_KEY_CALCULATED(Drone, FrameSizeRatio, double, [](
Config* config) -> double {
    cv::Size size = config->Get<cv::Size>(ConfigKeys::Drone::FrameSize);
    return size.width/(double)size.height;
})

DEFINE_CONFIG_KEY_CALCULATED(Drone, VerticalFOV, double, [](
Config* config) -> double {
    double aspect = config->Get<double>(ConfigKeys::Drone::FrameSizeRatio);
    double fov = config->Get<double>(ConfigKeys::Drone::FOV);
return fov/aspect;
})

DEFINE_CONFIG_KEY(Communications, BroadcastPort, int)
DEFINE_CONFIG_KEY(Communications, StateSendLapse, int);
DEFINE_CONFIG_KEY(Communications, StateExpireLapse, int);

DEFINE_CONFIG_KEY(Debugging, VisualDebugEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputHudVideoEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, RealTimeVideoOutputEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputRawVideoEnabled, bool);
DEFINE_CONFIG_KEY(Debugging, OutputPath, std::string);
DEFINE_CONFIG_KEY(Debugging, MapDebuggerScale, int);
DEFINE_CONFIG_KEY(Debugging, MapDebuggerEnabled, bool);

DEFINE_CONFIG_KEY(Body, SleepDelay, int)
DEFINE_CONFIG_KEY(Body, Hal, HalType)
DEFINE_CONFIG_KEY(Body, Start, bool)
DEFINE_CONFIG_KEY(Body, TestToExecute, std::string)
DEFINE_CONFIG_KEY(Body, DummyCameraVideoSource, std::string)
DEFINE_CONFIG_KEY(Body, CascadeDetector, std::string)
DEFINE_CONFIG_KEY(Body, TrackingSmoothingSamples, int)
DEFINE_CONFIG_KEY(Body, WhiteBalance, WhiteBalanceMode)
DEFINE_CONFIG_KEY(Body, Saturation, float)
DEFINE_CONFIG_KEY(Body, Exposure, float)

DEFINE_CONFIG_KEY(Body, TargetSlowdownRadius, float)
DEFINE_CONFIG_KEY(Body, AlignmentAngleThreshold, float)
DEFINE_CONFIG_KEY(Body, DisplacementMaxVelocity, float)
DEFINE_CONFIG_KEY(Body, YawMaxVelocity, float)
DEFINE_CONFIG_KEY(Body, AltitudeSlowDownRadius, float)
DEFINE_CONFIG_KEY(Body, FollowerTargetDistance, float)
DEFINE_CONFIG_KEY(Body, YawAproximationAngle, float)
DEFINE_CONFIG_KEY(Body, TargetReachedRadius, float)
DEFINE_CONFIG_KEY(Body, StartAlignementDistance, float)

DEFINE_CONFIG_KEY(Brain, Start, bool)
DEFINE_CONFIG_KEY(Brain, LowBatteryLevel, int)
DEFINE_CONFIG_KEY(Brain, CriticalBatteryLevel, int)

DEFINE_CONFIG_KEY(Land, MarkerDetectTolerance, float)
DEFINE_CONFIG_KEY(Land, PitchVelFactor, float)
DEFINE_CONFIG_KEY(Land, RollVelFactor, float)
DEFINE_CONFIG_KEY(Land, GazVelFactor, float)
DEFINE_CONFIG_KEY(Land, LandAltitude, float)
DEFINE_CONFIG_KEY(Land, PitchTolerance, float)
DEFINE_CONFIG_KEY(Land, RollTolerance, float)
DEFINE_CONFIG_KEY(Land, GazAdjustment, float)
DEFINE_CONFIG_KEY(Land, PitchAdjustment, float)
DEFINE_CONFIG_KEY(Land, RollAdjustment, float)
DEFINE_CONFIG_KEY(Land, NoReferenceGazVel, float)
DEFINE_CONFIG_KEY(Land, MoveWithoutRefereceTime, int)
DEFINE_CONFIG_KEY(Land, StabilizationTime, int)

DEFINE_CONFIG_KEY(VirtualBody, VirtualBodyEnabled, bool)
DEFINE_CONFIG_KEY(VirtualBody, VirtualBodySpeed, float)
DEFINE_CONFIG_KEY(VirtualBody, VirtualBatteryDuraion, int)
DEFINE_CONFIG_KEY(VirtualBody, VirtualBatteryChargeTime, int)