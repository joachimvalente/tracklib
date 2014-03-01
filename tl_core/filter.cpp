#include "tl_core/filter.h"

namespace tl {

Filter::Filter() {}

const cv::Mat &Filter::predicted_x() const {
  return predicted_x_;
}

const cv::Mat &Filter::x() const {
  return x_;
}

}  // namespace tl
