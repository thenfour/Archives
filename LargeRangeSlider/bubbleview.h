#pragma once

#include "stdafx.h"
#include "distortionfield.h"

class DistortedView
{
public:
  DistortedView() :
    m_realRange(1000),
    m_displayRange(100),
    m_focus(500),
    m_uppSpecified(false)
  {
  }

  // properties
  void SetRealRange(int n)
  {
    m_realRange = n;
    RecalculateDistortion();
  }
  void SetDisplayRange(int n)
  {
    m_displayRange = n;
    RecalculateDistortion();
  }
  int GetRealRange() const
  {
    return m_realRange;
  }
  int GetDisplayRange() const
  {
    return m_displayRange;
  }

  // distortion parameters
  void SetFocus(int focus)// in REAL coords
  {
    m_focus = focus;
    RecalculateDistortion();
  }
  int GetFocus() const
  {
    return m_focus;
  }
  void SetDistortionSize(int n)// in REAL coords
  {
    m_distortion.SetLength(LinearRealToDisplay(n), n);
    RecalculateDistortion();
  }
  void SetUPP(float upp)
  {
    m_uppSpecified = true;
    m_upp = upp;
    RecalculateDistortion();
  }
  void UnsetUPP()
  {
    m_uppSpecified = false;
    RecalculateDistortion();
  }

  // transformation
  float DisplayToReal(float x) const
  {
    if(IsDisplayInDistortionRange(x))
    {
      return m_distortionLeftReal + m_distortion.DisplayToReal(x - m_distortionLeftDisplay);
    }
    return LinearDisplayToReal(x);
  }
  float RealToDisplay(float x) const
  {
    if(IsRealInDistortionRange(x))
    {
      return m_distortionLeftDisplay + m_distortion.RealToDisplay(x - m_distortionLeftReal);
    }
    return LinearRealToDisplay(x);
  }

  float LinearRealToDisplay(float n) const
  {
    return n * m_displayRange / m_realRange;
  }

  float LinearDisplayToReal(float n) const
  {
    return n * m_realRange / m_displayRange;
  }

  float GetZoomFactor(int display) const
  {
    if(IsDisplayInDistortionRange(display))
    {
      return m_distortion.GetZoomFactor(display - m_distortionLeftDisplay);
    }
    return 1.0f;
  }

private:
  bool IsDisplayInDistortionRange(int x) const
  {
    if(x < m_distortionLeftDisplay) return false;
    if(x >= m_distortionRightDisplay) return false;
    return true;
  }
  bool IsRealInDistortionRange(int x) const
  {
    if(x < m_distortionLeftReal) return false;
    if(x >= m_distortionRightReal) return false;
    return true;
  }

  void RecalculateDistortion()
  {
    m_distortionLeftReal = m_focus - (m_distortion.GetLengthReal() / 2);
    m_distortionRightReal = m_distortionLeftReal + m_distortion.GetLengthReal();
    m_distortionLeftDisplay = LinearRealToDisplay(m_distortionLeftReal);
    m_distortionRightDisplay = LinearRealToDisplay(m_distortionRightReal);
    m_distortion.SetTotalLengths(m_displayRange, m_realRange);
    /*
      here let's calculate the minimum slope (units per pixel).
      basically, we can't ever let it go below 1, and we can't ever let it go higher 
      than the real slope.
      of course 1 is pretty nuts. realistically it should be something like 100ms.
    */

    // here i target 150 ms. if the actual slope is more precise, use it.
    // don't go below 1
    if(m_uppSpecified)
    {
      m_distortion.SetMinimumUnitsPerPixel(m_upp);
    }
    else
    {
      m_distortion.SetMinimumUnitsPerPixel(max(1, min(m_realRange / m_displayRange, 50)));
    }
  }

  DistortionField m_distortion;// deals in real coords
  int m_realRange;
  int m_displayRange;
  int m_focus;// in REAL coords

  bool m_uppSpecified;
  float m_upp;

  int m_distortionLeftReal;// first
  int m_distortionRightReal;// one past the last.
  int m_distortionLeftDisplay;// first
  int m_distortionRightDisplay;// one past the last.
};




