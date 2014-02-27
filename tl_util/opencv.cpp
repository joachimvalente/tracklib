#include "opencv.h"

#include "../common.h"

using namespace cv;

namespace tl {

cv::Rect FrameCanvas(const cv::Mat &frame) {
  Rect canvas;
  canvas.x = 0;
  canvas.y = 0;
  canvas.width = frame.cols;
  canvas.height = frame.rows;
  return canvas;
}

bool IsRectInsideFrame(const cv::Mat &frame, cv::Rect rect) {
  Rect canvas = FrameCanvas(frame);
  return canvas.contains(rect.tl()) && canvas.contains(rect.br());
}

}
