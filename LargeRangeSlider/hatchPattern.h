

#pragma once

#include "bubbleview.h"

namespace Hatch2
{
  /*
    this very specialized function renders a line, but works only on a 1-pixel-wide slice of it.
    you specify the y coords for the start & end of the line, and the rest is history.
    check the docs on this for details.
    a = angle of the line
    h = height of line (different than line width - imagine a 2-wide line tipped 45-degrees. the vertical height of it is going to be more than 2)
    w = max horizontal width of line. determines how strong to make it.
    
    a = tan-1(end-start)
    h = (lw/2) / cos(a)
    w = 2 * h / (end-start)

    aaproc is void (*)(int y, float mix)
  */
  template<typename T, typename TAASetPixelProc, typename TSetPixelProc>
  inline void RenderVScanForLine(float lineWidth, float ya, float yb, T* obj, TAASetPixelProc AASetPixelProc, TSetPixelProc setPixelProc)
  {
    float start = min(ya, yb);// because it's all in 1 pixel, it doesnt matter if the slope is negative or positive. to make calculations easier, force it positive.
    float end = max(ya, yb);
    float lh = end - start;
    float a = tanh(lh);// angle of the line
    float h = (lineWidth / 2) / cos(a);// this is the half-height of the line at any 1 point
    float w = 2 * h / lh;

    // what actual pixels will i draw?
    float realStart = start - h;// where does the line start in sub-pixel space
    int pixelStart = (int)floor(realStart);
    float realEnd = end + h;
    int pixelEnd = (int)ceil(realEnd);
    float halfHeight = (realEnd - realStart) / 2;//half the height of the entire real area.
    float center = (ya + yb) / 2;

    for(int y = pixelStart; y <= pixelEnd; y ++)
    {
      // determine the strength of this pixel. distance from edge is key.
      float distanceFromEdge = halfHeight - abs(center - (y + 0.5f));// add 0.5 to get the center of the pixel, not the edge.
      float strength = (distanceFromEdge * w) / halfHeight;// distanceFromEdge gets scaled from 0-halfHeight to 0-w
      if(strength > 0.0f)
      {
        if(strength > 1.0f)
        {
          (obj->*setPixelProc)(y);
        }
        else
        {
          (obj->*AASetPixelProc)(y, strength);
        }
      }
    }
  }

  // handles rendering of a single vertical slice of a hatch pattern.
  // the hatch pattern may be distorted though. this 1 pixel may actually
  // hold the hatch patterns for multiple sub-things, so there's a start & end
  // offset.
  // if it's a straight line across horizontally, then of course the full color intensity would be used.
  // but if start & end are very different, thus meaning a very vertical line, well then the intensity needs to be
  // adjusted because it's spread out over many pixels.
  // 
  // or.... is that true? there would actually be more pixels there, but it may mean that more complex antialiasing needs to happen.
  // this needs to be sorted out.
  template<typename T, typename TAASetPixelProc, typename TSetPixelProc>
  void RenderVScanForHatch(
    int visibleHeight, 
    float hatchSize, float hatchBeginOffset, float hatchEndOffset,
    float lineWidth, float zoom,
    T* obj, TAASetPixelProc AASetPixelProc, TSetPixelProc setPixelProc)
  {
    float zoomedHatchSpan = zoom * (hatchEndOffset - hatchBeginOffset);// the virtual line we draw is this tall.
    hatchBeginOffset = fmod(min(hatchEndOffset, hatchBeginOffset), hatchSize);

    float zoomedOffsetStartA = zoom * hatchBeginOffset;
    float zoomedOffsetStartB = zoom * (hatchSize - 1 - hatchBeginOffset);

    for(float y = zoomedOffsetStartA; y < visibleHeight; y += hatchSize * zoom)
    {
      RenderVScanForLine(zoom * lineWidth, y, y + zoomedHatchSpan, obj, AASetPixelProc, setPixelProc);
    }
    for(float y = zoomedOffsetStartB; y < visibleHeight; y += hatchSize * zoom)
    {
      RenderVScanForLine(zoom * lineWidth, y, y + zoomedHatchSpan, obj, AASetPixelProc, setPixelProc);
    }
  }


}




namespace Hatch1
{
  /*
    this renders a vertical line.  the line works on sub-pixels, and is anti-aliased, but of course it's only vertical.
  */
  template<typename T, typename Tmixproc, typename Tsetproc>
  inline void RenderVLine(float ya, float yb, T* obj, Tmixproc mixproc, Tsetproc setproc)
  {
    int top = int(ya);// the first pixel to render
    int bottom = int(yb);// the last pixel to render
    float topMix = 1.0f - (ya - top);// the first pixel represents THIS much of the first virtual pixel (it will be 1.0 if ya is integral, or less if ya has fraction)
    float bottomMix = (yb - bottom);

    if(top == bottom)// for <1 pixel sized lines
    {
      (obj->*mixproc)(top, (yb - ya));
      return;
    }

    // draw the anti-aliased edges
    (obj->*mixproc)(top, topMix);
    (obj->*mixproc)(bottom, bottomMix);

    int y = top;

    for(int y = top + 1; y < bottom; y ++)
    {
      (obj->*setproc)(y);
    }
  }

  // handles rendering of a single vertical slice of a hatch pattern.
  // the hatch pattern may be distorted though. this 1 pixel may actually
  // hold the hatch patterns for multiple sub-things, so there's a start & end
  // offset
  template<typename T, typename Tmixproc, typename Tsetproc>
  void RenderHatchVLine(int topVisibleBound, int visibleSize, 
      int hatchSize, int beginOffset, float lineWidth,
      float zoom, T* obj, Tmixproc mixproc, Tsetproc setproc)
  {
    float ycenter = topVisibleBound + ((float)visibleSize / 2);
    float zoomedSkip = zoom * hatchSize;
    float zoomedHalfLineWidth = zoom * lineWidth / 2;
    float zoomedBeginOffset1 = zoom * beginOffset - zoomedHalfLineWidth;
    float zoomedLineHeight = zoom * lineWidth;
    float zoomedBeginOffset2 = zoom * (hatchSize - 1 - beginOffset) - zoomedHalfLineWidth;

    float y1 = zoomedBeginOffset1;
    float y2 = zoomedBeginOffset2;
    for(; y1 < (visibleSize / 2); y1 += zoomedSkip, y2 += zoomedSkip)// from center + offset to edge
    {
      RenderVLine(
        max(ycenter, ycenter + y1 + 1),// don't start above ycenter
        min(topVisibleBound + visibleSize, ycenter + y1 + 1 + zoomedLineHeight),// and not past 
        obj, mixproc, setproc);// bottom
      RenderVLine(
        max(topVisibleBound, ycenter - y1),
        min(ycenter, ycenter - y1 + zoomedLineHeight),
        obj, mixproc, setproc);// top

      RenderVLine(
        max(ycenter, ycenter + y2 + 1),// don't start above ycenter
        min(topVisibleBound + visibleSize, ycenter + y2 + 1 + zoomedLineHeight),// and not past 
        obj, mixproc, setproc);// bottom
      RenderVLine(
        max(topVisibleBound, ycenter - y2),
        min(ycenter, ycenter - y2 + zoomedLineHeight),
        obj, mixproc, setproc);// top
    }
  }
}

