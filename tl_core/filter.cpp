#include "tl_core/filter.h"

namespace tl {

//------------------------ Constructor ---------------------------
Filter::Filter() {}

//----------------------- Core functions -------------------------
std::string Filter::ToString() const {
  return "undocumented filter";
}

//----------------------- Public accessors ------------------------
const cv::Mat &Filter::predicted_x() const {
  return predicted_x_;
}

const cv::Mat &Filter::x() const {
  return x_;
}

}  // namespace tl
