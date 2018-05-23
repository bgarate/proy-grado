#include <iostream>
#include <src/navigation/MarkerTracker.h>
#include <sys/stat.h>
#include <opencv2/imgcodecs.hpp>


int main(int argc, const char* args[]) {

    std::string dir = "markers";

    MarkerTracker markerTracker;

    struct stat st = {0};

    if (stat(dir.c_str(), &st) == -1) {
        mkdir(dir.c_str(), 0700);
    }

    for (int i = 0; i < markerTracker.getDictionarySize(); ++i) {
        cv::Mat marker = markerTracker.getImage(i);

        cv::imwrite(dir + "/" + std::to_string(i) + ".png", marker);
    }

    return 0;
}
