//
// Created by bruno on 12/08/17.
//

#include "../utils/Helpers.h"
#include "../tracking/Follower.h"
#include "../logging/Logger.h"
#include "../config/ConfigKeys.h"
#include "PathFollower.h"
#include "CommandGenerator.h"

PathFollower::PathFollower(Config *config, World *world) : PositionsHistory(1000), DeltaTimeHistory(1000) {
    this->config = config;
    this->world = world;
    drone = world->getDrones()[0];

    startAlignementDistance = config->Get(ConfigKeys::Body::StartAlignementDistance);
    targetReachedDistance = config->Get(ConfigKeys::Body::TargetReachedRadius);
    alignmentAngleThreshold = config->Get(ConfigKeys::Body::AlignmentAngleThreshold);

}

void PathFollower::setPath(Path path) {
    this->path = path;
    currentTarget = 0;
}

NavigationCommand PathFollower::update(std::vector<Marker> markers, double altitude, double deltaTime) {

    runningTime += deltaTime;

    if(markers.size() == 0) {
        timeSinceLastMarkerSeen += deltaTime;

        if(timeSinceLastMarkerSeen > 1000000) {
            return NavigationCommand(0,0,lastYawSign, 0);
        } else {
            lastCommand.Gaz = 0;
            return lastCommand;
        }

    }
    timeSinceLastMarkerSeen = 0;

    EstimatePosition(markers, altitude);

    if(std::isnan(EstimatedPosition[0]) || std::isnan(EstimatedPosition[1]) || std::isnan(EstimatedPosition[2])) {
        if (PositionsHistory.size() > 0)
            EstimatedPosition = PositionsHistory[PositionsHistory.size() - 1];
        else
            EstimatedPosition = cv::Vec3d(0,0,0);
    }
    // TODO: Para el smoothin se usan valores smootheados. Lo mejor hacer smoothing sobre los valores RAW
    // TODO: El smoothing no toma en cuenta el tiempo, se asume que el deltaTime es fijo
    SmoothEstimation();

    PositionsHistory.push_back(EstimatedPosition);
    DeltaTimeHistory.push_back(deltaTime);

    EstimateNextPosition();
    ProjectNextPosition();

    PathPoint targetPathPoint = path.GetPoints()[(currentTarget + 1) % path.GetPoints().size()];

    // No considera altura para asumir que llegó
    cv::Vec3d v = targetPathPoint.position - EstimatedPosition;
    double distanceToPathPoint = cv::norm(cv::Vec2d(v[0],v[1]));

    double alignmentAngle = Helpers::angleDifference(targetPathPoint.rotation,EstimatedPose[2]);

    if(distanceToPathPoint <= targetReachedDistance &&
            std::abs(alignmentAngle) < alignmentAngleThreshold) {

        currentTarget = (currentTarget + 1) % path.GetPoints().size();
        return NavigationCommand();

    }

    double targetRotation = distanceToPathPoint < startAlignementDistance ?
                            targetPathPoint.rotation : EstimatedPose[2];

    CommandGenerator generator(EstimatedPosition, EstimatedPose[2], config);

    lastCommand = generator.getCommand(targetPathPoint.position, targetRotation);
    lastYawSign = lastCommand.YawSpeed < 0 ? -1 : 1;

    return lastCommand;
}

int PathFollower::getTargetId() {

    return currentTarget;
}

void PathFollower::setTarget(int target) {
    currentTarget = target;
}

void PathFollower::EstimatePosition(const std::vector<Marker> &markers, double altitude) {

    if(markers.size() == 0)
        return;

    EstimatedPositions.clear();
    EstimatedPoses.clear();

    for(const Marker& marker : markers) {

        WorldObject* markerDescription = world->getMarker(marker.Id);

        if(markerDescription == NULL)
            return;

        cv::Vec3d posXyz = marker.getCameraTranslation();
        double groundDistance = sqrt(posXyz[0]*posXyz[0] + posXyz[1]*posXyz[1]);
        double estimatedMarkerAngle = marker.getEulerAngles()[2];
        double estimatedCameraAngle = markerDescription->getRotation()[2] - estimatedMarkerAngle;

/*
        cv::Vec2d estimatedTranslation =
                cv::Vec2d(sin(toRadians(estimatedCameraAngle)) * groundDistance,
                          cos(toRadians(estimatedCameraAngle)) * groundDistance);
*/
        EstimatedPoses.push_back(cv::Vec3d(0, 0, estimatedCameraAngle));

        cv::Vec3d estimatedPosition = markerDescription->getPosition();

        estimatedPosition[2] = altitude;
        estimatedPosition[0] += posXyz[0];
        estimatedPosition[1] += posXyz[1];

        EstimatedPositions.push_back(estimatedPosition);

    }

    EstimatedPosition = cv::Vec3d(0, 0, 0);
    EstimatedPose = cv::Vec3d(0, 0, 0);

    double sines = 0;
    double cosines = 0;

    for(int i = 0; i < EstimatedPositions.size(); i++) {
        EstimatedPosition += EstimatedPositions[i];
        sines += sin(Helpers::toRadians(EstimatedPoses[i][2]));
        cosines += cos(Helpers::toRadians(EstimatedPoses[i][2]));
    }

    EstimatedPosition[0] /= EstimatedPositions.size();
    EstimatedPosition[1] /= EstimatedPositions.size();
    EstimatedPosition[2] /= EstimatedPositions.size();

    // Ver https://en.wikipedia.org/wiki/Mean_of_circular_quantities
    EstimatedPose[2] = Helpers::toDegrees(atan2(sines, cosines));

}


/*
 * Ver https://en.wikipedia.org/wiki/Moving_average#Weighted_moving_average
 */
void PathFollower::SmoothEstimation() {

    int numberOfSmoothingSamples = config->Get(ConfigKeys::Body::TrackingSmoothingSamples);

    if(PositionsHistory.size() < numberOfSmoothingSamples)
        return;

    cv::Vec3d weightedSum = EstimatedPosition * numberOfSmoothingSamples;

    for(int i = 1; i < numberOfSmoothingSamples; i++){
        weightedSum += (numberOfSmoothingSamples - i) * PositionsHistory[PositionsHistory.size() - i];
    }

    EstimatedPosition = weightedSum * 2 / ((float)numberOfSmoothingSamples*(numberOfSmoothingSamples + 1));

}

void PathFollower::EstimateNextPosition() {

    int numberOfSmoothingSamples = std::min(config->Get(ConfigKeys::Body::TrackingSmoothingSamples), (int)PositionsHistory.size() - 1);

    cv::Vec3d weightedSumOfDisplacements = cv::Vec3d(0,0,0);
    double weightedSumOfDeltaTimes = 0;

    for(int i = 1; i <= numberOfSmoothingSamples; i++){

        cv::Vec3d posA = PositionsHistory[PositionsHistory.size() - i - 1];
        cv::Vec3d posB = PositionsHistory[PositionsHistory.size() - i];

        weightedSumOfDisplacements += (posB - posA) * (numberOfSmoothingSamples - i + 1);
        weightedSumOfDeltaTimes += DeltaTimeHistory[DeltaTimeHistory.size() - i] * (numberOfSmoothingSamples - i + 1);
    }

    cv::Vec3d smoothedDisplacement = (weightedSumOfDisplacements / weightedSumOfDeltaTimes) * NEXT_POSITION_MICROSECONDS;
    PredictedPosition = PositionsHistory[PositionsHistory.size() - 1] + smoothedDisplacement;

}

void PathFollower::ProjectNextPosition() {

    std::vector<PathPoint> points = path.GetPoints();

    PathPoint a = points[currentTarget];
    PathPoint b = points[(currentTarget + 1) % points.size()];

    cv::Vec3d direction = cv::normalize(b.position-a.position);
    cv::Vec3d toPredicted = PredictedPosition - a.position;

    ProjectedPredictedPosition = a.position + direction * (toPredicted.dot(direction));
    TargetOnPath = ProjectedPredictedPosition + direction * 0.25;

    float multiplier = toPredicted.dot(direction) + 0.25;
    float norm = cv::norm(b.position - a.position);

    if(multiplier > norm)
        TargetOnPath = b.position;
    else if (multiplier < 0)
        TargetOnPath = a.position;
}
