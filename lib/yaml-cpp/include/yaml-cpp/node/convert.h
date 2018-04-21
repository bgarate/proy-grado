#ifndef NODE_CONVERT_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define NODE_CONVERT_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include <array>
#include <limits>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#include "yaml-cpp/binary.h"
#include "yaml-cpp/node/impl.h"
#include "yaml-cpp/node/iterator.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/type.h"
#include "yaml-cpp/null.h"

#include <opencv2/opencv.hpp>
#include "../../../../../src/hal/HalType.hpp"
#include "../../../../../src/hal/WhiteBalanceMode.h"

namespace YAML {
class Binary;
struct _Null;
template <typename T>
struct convert;
}  // namespace YAML

namespace YAML {
namespace conversion {
inline bool IsInfinity(const std::string& input) {
  return input == ".inf" || input == ".Inf" || input == ".INF" ||
         input == "+.inf" || input == "+.Inf" || input == "+.INF";
}

inline bool IsNegativeInfinity(const std::string& input) {
  return input == "-.inf" || input == "-.Inf" || input == "-.INF";
}

inline bool IsNaN(const std::string& input) {
  return input == ".nan" || input == ".NaN" || input == ".NAN";
}
}

// Node
template <>
struct convert<Node> {
  static Node encode(const Node& rhs) { return rhs; }

  static bool decode(const Node& node, Node& rhs) {
    rhs.reset(node);
    return true;
  }
};

// std::string
template <>
struct convert<std::string> {
  static Node encode(const std::string& rhs) { return Node(rhs); }

  static bool decode(const Node& node, std::string& rhs) {
    if (!node.IsScalar())
      return false;
    rhs = node.Scalar();
    return true;
  }
};

// C-strings can only be encoded
template <>
struct convert<const char*> {
  static Node encode(const char*& rhs) { return Node(rhs); }
};

template <std::size_t N>
struct convert<const char[N]> {
  static Node encode(const char(&rhs)[N]) { return Node(rhs); }
};

template <>
struct convert<_Null> {
  static Node encode(const _Null& /* rhs */) { return Node(); }

  static bool decode(const Node& node, _Null& /* rhs */) {
    return node.IsNull();
  }
};

#define YAML_DEFINE_CONVERT_STREAMABLE(type, negative_op)                \
  template <>                                                            \
  struct convert<type> {                                                 \
    static Node encode(const type& rhs) {                                \
      std::stringstream stream;                                          \
      stream.precision(std::numeric_limits<type>::digits10 + 1);         \
      stream << rhs;                                                     \
      return Node(stream.str());                                         \
    }                                                                    \
                                                                         \
    static bool decode(const Node& node, type& rhs) {                    \
      if (node.Type() != NodeType::Scalar)                               \
        return false;                                                    \
      const std::string& input = node.Scalar();                          \
      std::stringstream stream(input);                                   \
      stream.unsetf(std::ios::dec);                                      \
      if ((stream >> std::noskipws >> rhs) && (stream >> std::ws).eof()) \
        return true;                                                     \
      if (std::numeric_limits<type>::has_infinity) {                     \
        if (conversion::IsInfinity(input)) {                             \
          rhs = std::numeric_limits<type>::infinity();                   \
          return true;                                                   \
        } else if (conversion::IsNegativeInfinity(input)) {              \
          rhs = negative_op std::numeric_limits<type>::infinity();       \
          return true;                                                   \
        }                                                                \
      }                                                                  \
                                                                         \
      if (std::numeric_limits<type>::has_quiet_NaN &&                    \
          conversion::IsNaN(input)) {                                    \
        rhs = std::numeric_limits<type>::quiet_NaN();                    \
        return true;                                                     \
      }                                                                  \
                                                                         \
      return false;                                                      \
    }                                                                    \
  }

#define YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(type) \
  YAML_DEFINE_CONVERT_STREAMABLE(type, -)

#define YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(type) \
  YAML_DEFINE_CONVERT_STREAMABLE(type, +)

YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(int);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(short);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(long);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(long long);
YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(unsigned);
YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(unsigned short);
YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(unsigned long);
YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(unsigned long long);

YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(char);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(signed char);
YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED(unsigned char);

YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(float);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(double);
YAML_DEFINE_CONVERT_STREAMABLE_SIGNED(long double);

#undef YAML_DEFINE_CONVERT_STREAMABLE_SIGNED
#undef YAML_DEFINE_CONVERT_STREAMABLE_UNSIGNED
#undef YAML_DEFINE_CONVERT_STREAMABLE

// bool
template <>
struct convert<bool> {
  static Node encode(bool rhs) { return rhs ? Node("true") : Node("false"); }

  YAML_CPP_API static bool decode(const Node& node, bool& rhs);
};

// std::map
template <typename K, typename V>
struct convert<std::map<K, V>> {
  static Node encode(const std::map<K, V>& rhs) {
    Node node(NodeType::Map);
    for (typename std::map<K, V>::const_iterator it = rhs.begin();
         it != rhs.end(); ++it)
      node.force_insert(it->first, it->second);
    return node;
  }

  static bool decode(const Node& node, std::map<K, V>& rhs) {
    if (!node.IsMap())
      return false;

    rhs.clear();
    for (const_iterator it = node.begin(); it != node.end(); ++it)
#if defined(__GNUC__) && __GNUC__ < 4
      // workaround for GCC 3:
      rhs[it->first.template as<K>()] = it->second.template as<V>();
#else
      rhs[it->first.as<K>()] = it->second.as<V>();
#endif
    return true;
  }
};

// std::vector
template <typename T>
struct convert<std::vector<T>> {
  static Node encode(const std::vector<T>& rhs) {
    Node node(NodeType::Sequence);
    for (typename std::vector<T>::const_iterator it = rhs.begin();
         it != rhs.end(); ++it)
      node.push_back(*it);
    return node;
  }

  static bool decode(const Node& node, std::vector<T>& rhs) {
    if (!node.IsSequence())
      return false;

    rhs.clear();
    for (const_iterator it = node.begin(); it != node.end(); ++it)
#if defined(__GNUC__) && __GNUC__ < 4
      // workaround for GCC 3:
      rhs.push_back(it->template as<T>());
#else
      rhs.push_back(it->as<T>());
#endif
    return true;
  }
};

// std::list
template <typename T>
struct convert<std::list<T>> {
  static Node encode(const std::list<T>& rhs) {
    Node node(NodeType::Sequence);
    for (typename std::list<T>::const_iterator it = rhs.begin();
         it != rhs.end(); ++it)
      node.push_back(*it);
    return node;
  }

  static bool decode(const Node& node, std::list<T>& rhs) {
    if (!node.IsSequence())
      return false;

    rhs.clear();
    for (const_iterator it = node.begin(); it != node.end(); ++it)
#if defined(__GNUC__) && __GNUC__ < 4
      // workaround for GCC 3:
      rhs.push_back(it->template as<T>());
#else
      rhs.push_back(it->as<T>());
#endif
    return true;
  }
};

// std::array
template <typename T, std::size_t N>
struct convert<std::array<T, N>> {
  static Node encode(const std::array<T, N>& rhs) {
    Node node(NodeType::Sequence);
    for (const auto& element : rhs) {
      node.push_back(element);
    }
    return node;
  }

  static bool decode(const Node& node, std::array<T, N>& rhs) {
    if (!isNodeValid(node)) {
      return false;
    }

    for (auto i = 0u; i < node.size(); ++i) {
#if defined(__GNUC__) && __GNUC__ < 4
      // workaround for GCC 3:
      rhs[i] = node[i].template as<T>();
#else
      rhs[i] = node[i].as<T>();
#endif
    }
    return true;
  }

 private:
  static bool isNodeValid(const Node& node) {
    return node.IsSequence() && node.size() == N;
  }
};

// std::pair
template <typename T, typename U>
struct convert<std::pair<T, U>> {
  static Node encode(const std::pair<T, U>& rhs) {
    Node node(NodeType::Sequence);
    node.push_back(rhs.first);
    node.push_back(rhs.second);
    return node;
  }

  static bool decode(const Node& node, std::pair<T, U>& rhs) {
    if (!node.IsSequence())
      return false;
    if (node.size() != 2)
      return false;

#if defined(__GNUC__) && __GNUC__ < 4
    // workaround for GCC 3:
    rhs.first = node[0].template as<T>();
#else
    rhs.first = node[0].as<T>();
#endif
#if defined(__GNUC__) && __GNUC__ < 4
    // workaround for GCC 3:
    rhs.second = node[1].template as<U>();
#else
    rhs.second = node[1].as<U>();
#endif
    return true;
  }
};

// binary
template <>
struct convert<Binary> {
  static Node encode(const Binary& rhs) {
    return Node(EncodeBase64(rhs.data(), rhs.size()));
  }

  static bool decode(const Node& node, Binary& rhs) {
    if (!node.IsScalar())
      return false;

    std::vector<unsigned char> data = DecodeBase64(node.Scalar());
    if (data.empty() && !node.Scalar().empty())
      return false;

    rhs.swap(data);
    return true;
  }
};

/* CUSTOM CONVERSIONS */


template<>
struct convert<cv::Size_<int>> {
static Node encode(const cv::Size_<int>& rhs) {
    Node node;
    node.push_back(rhs.width);
    node.push_back(rhs.height);
    return node;
}

    static bool decode(const Node& node, cv::Size2i& rhs) {
        if(!node.IsSequence() || node.size() != 2) {
            return false;
        }

        rhs.width = node[0].as<int>();
        rhs.height = node[1].as<int>();
        return true;
    }
};


template<typename T, int n>
struct convert<cv::Vec<T,n>> {
    static Node encode(const cv::Vec<T,n>& rhs) {
        Node node;
        for(int i = 0; i < n; i++)
            node.push_back(rhs[i]);
        return node;
    }

    static bool decode(const Node& node, cv::Vec<T,n>& rhs) {
        if(!node.IsSequence() || node.size() != n) {
            return false;
        }

        for(int i = 0; i < n; i++)
            rhs[i] = node[i].as<T>();

        return true;
    }
};



template<>
struct convert<cv::Mat> {
    static Node encode(const cv::Mat& rhs) {
        Node node;
        node["rows"] = rhs.rows;
        node["cols"] = rhs.cols;
        node["type"] = rhs.type();

        // TODO: Solo funciona para matrices con un solo canal!

        Node data = node["data"];

        for (int i = 0; i < rhs.cols * rhs.rows; ++i) {
            double d = rhs.at<double>(i/rhs.cols,i%rhs.cols);
            data[i] = d;
        }

        return node;
    }

    static bool decode(const Node& node, cv::Mat& rhs) {

        cv::Mat mat(node["rows"].as<int>(), node["cols"].as<int>(), node["type"].as<int>());

        // TODO: Solo funciona para matrices con un solo canal!

        Node data = node["data"];

        for (int i = 0; i < node["data"].size(); ++i) {
            mat.at<double>(i/mat.cols,i%mat.cols) = data[i].as<double>();
        }

        mat.copyTo(rhs);

        return true;
    }
};

    template<>
    struct convert<HalType> {
        static Node encode(const HalType& rhs) {
            Node node;

            switch (rhs) {
                case HalType::Vrep:
                    node = "Vrep";
                    break;
                case HalType::Pb2:
                    node = "Pb2";
                    break;
                case HalType::Dummy:
                    node = "Dummy";
                    break;
                default:
                    throw std::runtime_error("Haltype desconocido");
            }

            return node;
        }

        static bool decode(const Node& node, HalType& rhs) {

            std::string haltype = node.as<std::string>();

            if(haltype == "Vrep")
                rhs = HalType::Vrep;
            else if(haltype == "Pb2")
                rhs =  HalType::Pb2;
            else if(haltype == "Dummy")
                rhs =  HalType::Dummy;
            else
                throw std::runtime_error("Representacion de haltype desconocido");

            return true;
        }
    };


    template<>
    struct convert<WhiteBalanceMode> {
        static Node encode(const WhiteBalanceMode& rhs) {
            Node node;

            switch (rhs) {
                case WhiteBalanceMode::Auto:
                    node = "Auto";
                    break;
                case WhiteBalanceMode::Cloudy:
                    node = "Cloudy";
                    break;
                case WhiteBalanceMode::CoolWhite:
                    node = "CoolWhite";
                    break;
                case WhiteBalanceMode::Daylight:
                    node = "Daylight";
                    break;
                case WhiteBalanceMode::Tungsten:
                    node = "Tungsten";
                    break;
                default:
                    throw std::runtime_error("WhiteBalanceMode desconocido");
            }

            return node;
        }

        static bool decode(const Node& node, WhiteBalanceMode& rhs) {

            std::string wb = node.as<std::string>();

            if(wb == "Auto")
                rhs = WhiteBalanceMode::Auto;
            else if(wb == "Cloudy")
                rhs =  WhiteBalanceMode::Cloudy;
            else if(wb == "CoolWhite")
                rhs =  WhiteBalanceMode::CoolWhite;
            else if(wb == "Daylight")
                rhs =  WhiteBalanceMode::Daylight;
            else if(wb == "Tungsten")
                rhs =  WhiteBalanceMode::Tungsten;
            else
                throw std::runtime_error("Representacion de WhiteBalanceMode desconocido");

            return true;
        }
    };



}

#endif  // NODE_CONVERT_H_62B23520_7C8E_11DE_8A39_0800200C9A66
