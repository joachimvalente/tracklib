#include "opencv.h"

#include "../common.h"

using namespace cv;

namespace tl {

Rect FrameCanvas(const Mat &frame) {
  Rect canvas;
  canvas.x = 0;
  canvas.y = 0;
  canvas.width = frame.cols;
  canvas.height = frame.rows;
  return canvas;
}

bool IsRectInsideFrame(const Mat &frame, Rect rect) {
  Rect canvas = FrameCanvas(frame);
  return canvas.contains(rect.tl()) && canvas.contains(rect.br());
}

}
