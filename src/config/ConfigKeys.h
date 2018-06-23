//
// Created by bruno on 24/09/17.
//

#ifndef PROY_GRADO_CONFIGKEYS_H
#define PROY_GRADO_CONFIGKEYS_H


#include "../config/Config.h"
#include "../hal/HalType.hpp"
#include "../hal/WhiteBalanceMode.h"

struct ConfigKeys {

    struct Drone {
        CONFIG_KEY(Drone, Name, std::string)
        CONFIG_KEY(Drone, Id, int)
        CONFIG_KEY(Drone, FOV, double)
        CONFIG_KEY(Drone, FrameSize, cv::Size)
        CONFIG_KEY(Drone, CameraTilt, double)
        CONFIG_KEY(Drone, CameraMatrix, cv::Mat)
        CONFIG_KEY(Drone, DistortionCoefficients, cv::Mat)
        CONFIG_KEY(Drone, FrameSizeRatio, double)
        CONFIG_KEY(Drone, VerticalFOV, double)
    };

    struct Communications {
        CONFIG_KEY(Communications, BroadcastPort, int)
        CONFIG_KEY(Communications, BroadcastLocalIp, std::string)
        CONFIG_KEY(Communications, StateSendLapse, int);
        CONFIG_KEY(Communications, StateExpireLapse, int);
    };

    struct Debugging {
        CONFIG_KEY(Debugging, VisualDebugEnabled, bool);
        CONFIG_KEY(Debugging, OutputHudVideoEnabled, bool);
        CONFIG_KEY(Debugging, RealTimeVideoOutputEnabled, bool);
        CONFIG_KEY(Debugging, OutputRawVideoEnabled, bool);
        CONFIG_KEY(Debugging, OutputPath, std::string);
        CONFIG_KEY(Debugging, MapDebuggerScale, int);
        CONFIG_KEY(Debugging, MapDebuggerEnabled, bool);
    };

    struct Body {
        CONFIG_KEY(Body, SleepDelay, int)
        CONFIG_KEY(Body, Hal, HalType)
        CONFIG_KEY(Body, Start, bool)
        CONFIG_KEY(Body, TestToExecute, std::string)
        CONFIG_KEY(Body, DummyCameraVideoSource, std::string)
        CONFIG_KEY(Body, CascadeDetector, std::string)
        CONFIG_KEY(Body, TrackingSmoothingSamples, int)
        CONFIG_KEY(Body, WhiteBalance, WhiteBalanceMode)
        CONFIG_KEY(Body, Saturation, float)
        CONFIG_KEY(Body, Exposure, float)

        CONFIG_KEY(Body, TargetSlowdownRadius, float)
        CONFIG_KEY(Body, AlignmentAngleThreshold, float)
        CONFIG_KEY(Body, DisplacementMaxVelocity, float)
        CONFIG_KEY(Body, YawMaxVelocity, float)
        CONFIG_KEY(Body, AltitudeSlowDownRadius, float)
        CONFIG_KEY(Body, FollowerTargetDistance, float)
        CONFIG_KEY(Body, YawAproximationAngle, float)
        CONFIG_KEY(Body, TargetReachedRadius, float)
        CONFIG_KEY(Body, StartAlignementDistance, float)
    };

    struct Brain {
        CONFIG_KEY(Body, Start, bool)
        CONFIG_KEY(Body, LowBatteryLevel, int)
        CONFIG_KEY(Body, CriticalBatteryLevel, int)
    };

    struct Land {
        CONFIG_KEY(Land, MarkerDetectTolerance, float)
        CONFIG_KEY(Land, PitchVelFactor, float)
        CONFIG_KEY(Land, RollVelFactor, float)
        CONFIG_KEY(Land, GazVelFactor, float)
        CONFIG_KEY(Land, LandAltitude, float)
        CONFIG_KEY(Land, PitchTolerance, float)
        CONFIG_KEY(Land, RollTolerance, float)
        CONFIG_KEY(Land, GazAdjustment, float)
        CONFIG_KEY(Land, PitchAdjustment, float)
        CONFIG_KEY(Land, RollAdjustment, float)
        CONFIG_KEY(Land, NoReferenceGazVel, float)
        CONFIG_KEY(Land, MoveWithoutRefereceTime, int)
        CONFIG_KEY(Land, StabilizationTime, int)
    };

    struct VirtualBody {
        CONFIG_KEY(VirtualBody, VirtualBodyEnabled, bool)
        CONFIG_KEY(VirtualBody, VirtualBodySpeed, float)
        CONFIG_KEY(VirtualBody, VirtualBatteryDuraion, int)
        CONFIG_KEY(VirtualBody, VirtualBatteryChargeTime, int)
    };
};

#include "Config.h"

#endif //PROY_GRADO_CONFIGKEYS_H
