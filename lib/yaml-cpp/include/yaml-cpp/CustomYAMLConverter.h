//
// Created by bruno on 24/09/17.
//

#ifndef PROY_GRADO_CUSTOMYAMLCONVERTER_H
#define PROY_GRADO_CUSTOMYAMLCONVERTER_H

#include <yaml-cpp/node/node.h>
#include <opencv2/core/types.hpp>


namespace YAML {
template<>
struct convert<cv::Size_<int>> {
  static Node encode(const cv::Size_<int>& rhs) {
    Node node;
    node.push_back(rhs.width);
    node.push_back(rhs.height);
    return node;
  }

  static bool decode(const Node& node, cv::Size_<int>& rhs) {
    if(!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs.width = node[0].as<int>();
    rhs.height = node[1].as<int>();
    return true;
  }
};

template<>
struct convert<cv::Mat> {
    static Node encode(const cv::Mat& rhs) {
        Node node;
        node.push_back(rhs.width);
        node.push_back(rhs.height);
        return node;
    }

    static bool decode(const Node& node, cv::Mat& rhs) {

        rhs = cv::Mat(node["rows"].as<int>(), node["cols"].as<int>(), );

        rhs = node[0];
        rhs.height = node[1].as<int>();
        return true;
    }
};

}

#endif //PROY_GRADO_CUSTOMYAMLCONVERTER_H
