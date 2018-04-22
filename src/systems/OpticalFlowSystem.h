//
// Created by bruno on 21/04/18.
//

#include "SystemBase.h"

#ifndef PROY_GRADO_OPTICALFLOWSYSTEM_H

class OpticalFlowSystem : public SystemBase {
public:
    void Cleanup() override {

    }

    std::string GetName() override {
        return "Optical flow";
    }

protected:

    void internalInit() override {
        opticalFlow = new OpticalFlow();
    }

    void internalUpdate(double deltaTime) override {

        std::shared_ptr<cv::Mat> frame = hal->getFrame(Camera::Front);

        if(frame != NULL && !frame->empty()) {
            opticalFlow->Update(frame, deltaTime);
            visualDebugger->OpticalFlow(opticalFlow->GetPoints());
        }

    }

private:

    OpticalFlow* opticalFlow;

};

#define PROY_GRADO_OPTICALFLOWSYSTEM_H

#endif //PROY_GRADO_OPTICALFLOWSYSTEM_H
