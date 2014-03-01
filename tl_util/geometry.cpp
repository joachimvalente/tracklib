#include "tl_util/geometry.h"

#include "common.h"

using namespace cv;

namespace tl {
namespace internal {

bool IsRectInsideFrame(cv::Rect rect, const cv::Mat &frame) {
  return 0 <= rect.tl().x && rect.br().x <= frame.cols &&
      0 <= rect.tl().y && rect.br().y <= frame.rows;
}

}  // namespace internal
}  // namespace tl
