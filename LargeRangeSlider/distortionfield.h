

#include <vector>

class DistortionField
{
public:
  DistortionField() :
    m_widthPixels(1),
    m_widthReal(1),
    m_upp(1),
    m_displayLength(1),
    m_mediaLength(1),
    m_dirty(true)
  {
  }

  // length of the distortion field, in pixels, and in media time.
  void SetLength(int pixels, int real)
  {
    if((m_widthPixels == pixels) && (m_widthReal == real))
    {
      return;
    }
    m_widthPixels = pixels;
    m_widthReal = real;
    m_dirty = true;
  }
  int GetLengthReal()
  {
    return m_widthReal;
  }
  int GetLengthDisplay()
  {
    return m_widthPixels;
  }

  // use minimum slope to set exactly how "zoomed" the slider will go.
  // this could also be referred to as "units per pixel at the flattest point"
  // caller needs to take into account the slope of the original line here.
  // if n is greater than the original slope then you are going to be fucked.
  void SetMinimumUnitsPerPixel(float n)
  {
    m_upp = n;
    m_dirty = true;
  }
  float GetMinimumUnitsPerPixel()
  {
    return m_upp;
  }

  // this is the slope of the linear graph.
  void SetTotalLengths(int displayLength, int mediaLength)
  {
    if((m_displayLength = displayLength) && (m_mediaLength == mediaLength))
    {
      return;
    }
    m_displayLength = displayLength;
    m_mediaLength = mediaLength;
    m_dirty = true;
  }
  int GetDisplayLength()
  {
    return m_displayLength;
  }
  int GetMediaLength()
  {
    return m_mediaLength;
  }

  float RealToDisplay(float n) const
  {
    PrepareLUT();
    // do a binary lookup.
    int left = 0;
    int right = m_widthPixels - 1;
		if(left == right)
			return 0;
    int mid;
    while(left <= right)
    {
      mid = (right + left)/2;
      if((m_lut[mid].begin <= n) && (m_lut[mid + 1].end > n))
      {
        // interpolate
        int sourcerange = m_lut[mid].end - m_lut[mid].begin;
        int thisval = (int)(n - m_lut[mid].begin);// TODO: make this a float?
        // if sourcerange = 2 and n = 0.5, then rutern mid + 0.25.
        // because 0.5 / 2
        return mid + ((float)thisval / sourcerange);
      }
      if(n < m_lut[mid].begin)
      {
        right = mid-1;
      }
      else
      {
        left = mid+1;
      }
    }
    return 0;    
  }

  float GetZoomFactor(int display) const
  {
    PrepareLUT();
    if(display < 0) return 1.0f;
    if(display >= m_widthPixels) return 1.0f;
    return m_lut[display].zoom;
  }

  float DisplayToReal(float n) const
  {
    PrepareLUT();
    if(n < 0) return 0;
    if(n >= m_widthPixels) return 0;
    // interpolate
    Element x = m_lut[(int)n];
    float f = n - floor(n);// f is the fractional portion
    return (x.begin * (1.0f - f)) + (x.end * f);// if f is lower, then more begin val.
  }

private:
  mutable bool m_dirty;
  float m_upp;// units per pixel
  int m_widthPixels;
  int m_widthReal;
  int m_displayLength;
  int m_mediaLength;

  struct Element
  {
    int begin;
    int end;
    float zoom;
  };

  mutable std::vector<Element> m_lut;// index = pixels, value = distortion

  // the basic formula to start with is
  // sin(x) * sin(x/2).
  // where sin(x) is the amount of distortion to apply (either making media later or earlier than the linear)
  // this gives a curve that begins at 0, accelerates, decelerates, then ends at 0 again.
  // the slope of this distortion at the middle is -1

  // to account for the length (period), it turns into
  // sin(x/f)*sin(x/(2*f))
  // now to determine f. if the period is 1000, and the original period is 2pi, then just divide.
  // f = period / 2pi. so if the period is 1, then sin(x/(1/2pi))
  // now with period,
  // sin((2*x*pi)/p) * sin(x/(2*(p/(2*pi)))) =
  // sin((2*x*pi)/p) * sin((x*pi)/p)

  // since period also affects the slope, we need to scale the graph vertically to get it back to a slope of 1
  // (p/(2*pi)) * sin((2*x*pi)/p) * sin((x*pi)/p)

  // now if you were to apply this to the graph, it would flatten out to a slope of 0 at the center.

  // so let's account for this "minimum slope" setting, and do 1 more scaling.
  // the smaller the distortion is, the more slope the resulting line will have.
  // multiply it by 1.0 would result in 0 slope at the center
  // multiply it by 0.0 would result in linear slope at the center
  // so it's just a matter of mixing.
  // (1.0 - (upp / linear)).
  // so if linear is 100, and upp is 1, we'll multiply by .99 and get almost 0 slope.
  // if linear is 100 and upp is 99, we'll mult by 0.01 and get almost linear slope. this is correct.
  void PrepareLUT() const
  {
    static const float my_PI = 3.1415926535f;
    if(!m_dirty) return;
    m_lut.clear();
    m_lut.reserve(m_widthPixels);
    float linearSlope = (float)m_mediaLength / m_displayLength;
    float scalePeriod = (m_widthReal / (2.0f * my_PI));
    float scaleUPP = 1.0f - (m_upp / linearSlope);
    float a = scalePeriod * scaleUPP;
    float maxZoomFactor = scaleUPP;
    Element e;
		int x = 0;
    for(; x < m_widthPixels; x ++)
    {
      // y = sin((2*x*pi)/p) * sin((x*pi)/p))
      // y =      b                 c
      float c = x * my_PI / m_widthPixels;// this is the affect curve - just one "hump" resulting in...at either end the distion will be very little
      float b = c * 2.0f;
      float sinb = sin(b);
      float sinc = sin(c);
      float y = (a * sinb * sinc);

      // now add it to the original line to get a real media time, not just a distortion offset.
      float mediaTime = (float)x * linearSlope;

      e.end = (int)(mediaTime + y);
      e.zoom = (sinc * maxZoomFactor) + 1;
      if(x > 0)
      {
				m_lut.push_back(e);
      }
      e.begin = e.end;// prepare for the next.
    }
    e.end = m_widthReal;
    if(x > 0)
			m_lut.push_back(e);


    m_dirty = false;
  }
};
