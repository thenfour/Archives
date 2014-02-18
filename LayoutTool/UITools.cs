/*
  January 25, 2005 carlc
  March 3. 2006 carlc - ported to C#

  Terminology:
  "Command" - a "Node" in the script that defines an area for placement. syntax is [* assignment list ] [{ children; }] [distance];
  "Assignment" - when you pin a place to an (identifier????)
  "Distance" - numeric or symbolic value that represents either the height or width of the command
  "Variable" - ?
  "Identifier" - ?
  "Symbol" - ?

*/

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

//--------------------------------------------------------------------------------
// allows nesting.
namespace AutoPlacement
{
  #region WinAPI imports
  class Winapi
  {
    [DllImport("gdi32.dll")]
    public static extern bool GetTextExtentPoint32(IntPtr hdc, string lpString, int cbString, ref SIZE lpSize);

    [DllImport("user32.dll")]
    public static extern IntPtr GetDC(IntPtr hWnd);

    [DllImport("gdi32.dll")]
    public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);

    [Serializable, StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
    public struct TEXTMETRIC
    {
      public int tmHeight;
      public int tmAscent;
      public int tmDescent;
      public int tmInternalLeading;
      public int tmExternalLeading;
      public int tmAveCharWidth;
      public int tmMaxCharWidth;
      public int tmWeight;
      public int tmOverhang;
      public int tmDigitizedAspectX;
      public int tmDigitizedAspectY;
      public char tmFirstChar;
      public char tmLastChar;
      public char tmDefaultChar;
      public char tmBreakChar;
      public byte tmItalic;
      public byte tmUnderlined;
      public byte tmStruckOut;
      public byte tmPitchAndFamily;
      public byte tmCharSet;
    }

    [DllImport("user32.dll")]
    public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, int wParam, int lParam);

    [DllImport("gdi32.dll", CharSet = CharSet.Unicode)]
    public static extern bool GetTextMetrics(IntPtr hdc, ref TEXTMETRIC lptm);

    [StructLayout(LayoutKind.Sequential)]
    public struct SIZE
    {
      public int cx;
      public int cy;
    } 

    [DllImport("user32.dll")]
    public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

    [DllImport("gdi32.dll")]
    public static extern bool DeleteObject(IntPtr hObject);

    public const UInt32 WM_GETFONT = 0x0031;
  }
  #endregion

  ////////////////////////////////////////////////////////////
  public class GUIMetrics
  {
    public GUIMetrics(System.Windows.Forms.Control wnd)
    {
      System.Drawing.Graphics g = System.Drawing.Graphics.FromHwnd(wnd.Handle);

      // DPI
      DPI = new System.Drawing.SizeF(g.DpiX, g.DpiY);

      // Dialog units
      System.Drawing.SizeF textSize = g.MeasureString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", wnd.Font);
      float baseUnitX = (textSize.Width / 26 + 1) / 2;
      float baseUnitY = textSize.Height;
      dialogBaseUnits = new System.Drawing.SizeF(baseUnitX, baseUnitY);
      g.Dispose();
    }

    public System.Drawing.SizeF dialogBaseUnits;
    public System.Drawing.SizeF DPI;
  }

  public class Util
  {
    public static double PointsToPixelsX(double pts, GUIMetrics m)
    {
      return pts * m.DPI.Width / 72;
    }
    public static double PointsToPixelsY(double pts, GUIMetrics m)
    {
      return pts * m.DPI.Height / 72;
    }
    public static double DLUsToPixelsX(double x, GUIMetrics m)
    {
      return x * m.dialogBaseUnits.Width / 4;
    }
    public static double DLUsToPixelsY(double y, GUIMetrics m)
    {
      return y * m.dialogBaseUnits.Height / 8;
    }
    public static bool IsA(System.Type lhs, System.Type rhs)
    {
      System.Type current = lhs;
      while(null != current)
      {
        if(current == rhs) return true;
        current = current.BaseType;
      }
      return false;
    }
    public static bool StringEqualsI(string a, string b)
    {
      return a.ToLower() == b.ToLower();
    }
    // parses "3.1123abcd" into 3.1123 and "abcd"
    // returns success
    public static bool StringToDouble(string in_, ref double num, ref string str)
    {
      try
      {
        System.Text.RegularExpressions.Regex r = new System.Text.RegularExpressions.Regex("([-+]?[0-9]*\\.?[0-9]+)(.*)$");
        System.Text.RegularExpressions.Match m = r.Match(in_);
        num = double.Parse(m.Groups[1].Value);
        str = m.Groups[2].Value;
        return true;
      }
      catch(FormatException)
      {
      }
      return false;
    }

    // does the string contain any chars that are NOT in chars?
    public static bool StringContainsNotOf(string s, string chars)
    {
      foreach(char c in s)
      {
        if(-1 == chars.IndexOf(c)) return true;
      }
      return false;
    }

    public static bool StartsWithNumber(string s)
    {
      System.Text.RegularExpressions.Regex r = new System.Text.RegularExpressions.Regex("^[-+]?[0-9]*\\.?[0-9]+");
      return r.IsMatch(s);
    }
  }

  // --------------------------------------------------------------------------------
  enum Orientation
  {
    None,
    Vertical,
    Horizontal
  };

  // --------------------------------------------------------------------------------
  class Message
  {
    public enum Severity
    {
      Debug,
      Warning,
      Error
    };

    public Message(string message_, Severity severity_, long line_, long column_)
    {
      message = message_;
      severity = severity_;
      line = line_;
      column = column_;
    }

    public Message(string message_, Severity severity_, long line_)
    {
      message = message_;
      severity = severity_;
      line = line_;
      column = 0;
    }

    public Message(string message_, Severity severity_)
    {
      message = message_;
      severity = severity_;
      line = 0;
      column = 0;
    }

    public Message(string message_)
    {
      message = message_;
      severity = Severity.Error;
      line = 0;
      column = 0;
    }

    public Message()
    {
      line = 0;
      column = 0;
      severity = Severity.Error;
    }

    public override string ToString()
    {
      string sev = "Unknown";
      switch(severity)
      {
      case Message.Severity.Debug:
        sev = "Debug";
        break;
      case Message.Severity.Warning:
        sev = "Warning";
        break;
      case Message.Severity.Error:
        sev = "Error";
        break;
      }
      return string.Format("({0}, {1}) {2}: {3}", line, column, sev, message);
    }

    long line;
    long column;
    string message;
    Severity severity;
  };

  // --------------------------------------------------------------------------------
  // units are always in pixels.
  class Position
  {
    public double _percent;
    public double pixels;
    public double dlus;
    public double points;

    public static Position Begin()
    {
      Position n = new Position();
      return n;
    }

    public static Position End()
    {
      Position n = new Position();
      n._percent = 1.0;
      return n;
    }

    public Position()
    {
      Clear();
    }
    public Position(Position rhs)
    {
      Assign(rhs);
    }

    public void Clear()
    {
      _percent = 0;
      pixels = 0;
      dlus = 0;
      points = 0;
    }

    public void Add(Position x)
    {
      _percent += x._percent;
      pixels += x.pixels;
      dlus += x.dlus;
      points += x.points;
    }

    public void Negate()
    {
      _percent = -_percent;
      pixels = -pixels;
      dlus = -dlus;
      points = -points;
    }

    public void Subtract(Position x)
    {
      _percent -= x._percent;
      pixels -= x.pixels;
      dlus -= x.dlus;
      points -= x.points;
    }

    public void Divide(double n)
    {
      _percent /= n;
      pixels /= n;
      dlus /= n;
      points /= n;
    }

    public void Multiply(double n)
    {
      _percent *= n;
      pixels *= n;
      dlus *= n;
      points *= n;
    }

    public void Assign(Position rhs)
    {
      _percent = rhs._percent;
      pixels = rhs.pixels;
      dlus = rhs.dlus;
      points = rhs.points;
    }

    /*
      Sort of hard to describe what "translate" does.  It's an arithmetical operation...
      basically the idea is to restate a position, relative to a different container.  consider the example figures:
          0%                                                   100%
      A:  |-----------------------------------------------------|   (distance = 100%+0)
             15%+5                                       90%-4
      B:       |-------------------------------------------|        (distance = 75%+1)
                   23%-6                       70%+13
      C:             |----------------------------|                 (distance = 47%+7)

      Right now, (23%-6) is relative to {B}.  In order to make it relative to {A}, we need to to a translation,
      which involves knowing the current parent (in this case, {B}).
    */
    public void TranslateRelativeTo(Position parent_start, Position parent_end)
    {
        Position temp = new Position(parent_end);
        temp.Subtract(parent_start);
        // now temp is the parent total distance. scale it by our percentage.
        temp.Multiply(_percent);
        temp.Add(parent_start);// now it's correct, but need to add in our non-percentage units.
        temp.dlus += dlus;
        temp.pixels += pixels;
        temp.points += points;
        Assign(temp);
    }

    public override string ToString()
    {
      string sPoints = (Math.Abs(points) < 0.001) ? "" : points.ToString("0.0") + "pt";
      string sPercent = (Math.Abs(_percent) < 0.001) ? "" : _percent.ToString("0.0") + "%";
      string sPixels = (Math.Abs(pixels) < 0.001) ? "" : pixels.ToString("0.0") + "px";
      string sDlus = (Math.Abs(dlus) < 0.001) ? "" : dlus.ToString("0.0") + "dlu";

      return "(" + string.Join(" + ", new string[] { sPercent, sPixels, sPoints, sDlus } ) + ")";
    }

    public int ToPixels(System.Windows.Forms.Control wnd, GUIMetrics m, System.Drawing.Rectangle rc, Orientation o)
    {
      double ret = pixels;
      if(o == Orientation.Vertical)
      {
        ret += Util.DLUsToPixelsY(dlus, m);
        ret += Util.PointsToPixelsY(points, m);
        ret += _percent * rc.Height;
      }
      else
      {
        ret += Util.DLUsToPixelsX(dlus, m);
        ret += Util.PointsToPixelsX(points, m);
        ret += _percent * rc.Width;
      }
      return System.Convert.ToInt32(ret);
    }
  };

  // --------------------------------------------------------------------------------
  // most of this gathered from
  // "Design Specifications and Guidelines - Visual Design"
  // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnwue/html/ch14e.asp
  // all items are in DLU's
  class UISettings
  {
    public UISettings()
    {
    }
    
    public void LoadDefaults()
    {
      ControlSpacingRelated = 4;
      ControlSpacingUnrelated = 7;
      LabelHeight = 8;
      ParagraphSpacing = 7;
      ClarityIndent = 10;
      LabelSpacingFromAssociatedControlTop = 3;

      // dialog
      DialogMarginLeft = 7;
      DialogMarginRight = 7;
      DialogMarginTop = 7;
      DialogMarginBottom = 7;
      DialogWideMarginLeft = 21;
      DialogWideMarginRight = 21;

      // group
      GroupMarginLeft = 9;
      GroupMarginTop = 11;
      GroupMarginRight = 9;
      GroupMarginBottom = 7;

      // controls
      ButtonWidth = 50;
      ButtonHeight = 14;
      CheckboxHeight = 10;
      ComboboxHeight = 14;
      RadioButtonHeight = 10;
      EditBoxHeight = 14;
    }

    public bool GetElement(ref double out_, string s)
    {
      if(Util.StringEqualsI(s, "spacing.relatedcontrol") || Util.StringEqualsI(s, "spacing"))
      {
        out_ = ControlSpacingRelated;
      }
      else if(Util.StringEqualsI(s, "spacing.unrelatedcontrol") || Util.StringEqualsI(s, "spacing.unrelated"))
      {
        out_ = ControlSpacingUnrelated;
      }
      else if(Util.StringEqualsI(s, "spacing.clarityindent") || Util.StringEqualsI(s, "spacing.clarityindent.left"))
      {
        out_ = ClarityIndent;
      }
      else if(Util.StringEqualsI(s, "margin.left"))
      {
        out_ = DialogMarginLeft;
      }
      else if(Util.StringEqualsI(s, "margin.right"))
      {
        out_ = DialogMarginRight;
      }
      else if(Util.StringEqualsI(s, "margin.top"))
      {
        out_ = DialogMarginTop;
      }
      else if(Util.StringEqualsI(s, "margin.bottom"))
      {
        out_ = DialogMarginBottom;
      }
      else if(Util.StringEqualsI(s, "widemargin.left"))
      {
        out_ = DialogWideMarginLeft;
      }
      else if(Util.StringEqualsI(s, "widemargin.right"))
      {
        out_ = DialogWideMarginRight;
      }
      else if(Util.StringEqualsI(s, "label.height"))
      {
        out_ = LabelHeight;
      }
      else if(Util.StringEqualsI(s, "label.paragraphspacing"))
      {
        out_ = ParagraphSpacing;
      }
      else if(Util.StringEqualsI(s, "label.offset.top") || Util.StringEqualsI(s, "label.offset"))
      {
        out_ = LabelSpacingFromAssociatedControlTop;
      }
      else if(Util.StringEqualsI(s, "group.margin.left"))
      {
        out_ = GroupMarginLeft;
      }
      else if(Util.StringEqualsI(s, "group.margin.top"))
      {
        out_ = GroupMarginTop;
      }
      else if(Util.StringEqualsI(s, "group.margin.right"))
      {
        out_ = GroupMarginRight;
      }
      else if(Util.StringEqualsI(s, "group.margin.bottom"))
      {
        out_ = GroupMarginBottom;
      }
      else if(Util.StringEqualsI(s, "button.width"))
      {
        out_ = ButtonWidth;
      }
      else if(Util.StringEqualsI(s, "button.height"))
      {
        out_ = ButtonHeight;
      }
      else if(Util.StringEqualsI(s, "checkbox.height"))
      {
        out_ = CheckboxHeight;
      }
      else if(Util.StringEqualsI(s, "combobox.height"))
      {
        out_ = ComboboxHeight;
      }
      else if(Util.StringEqualsI(s, "radio.height"))
      {
        out_ = RadioButtonHeight;
      }
      else if(Util.StringEqualsI(s, "editbox.height") || Util.StringEqualsI(s, "edit.height"))
      {
        out_ = EditBoxHeight;
      }
      else
      {
        return false;
      }
      return true;
    }

    // general layout
    public double ControlSpacingRelated;// 4 dlu
    public double ControlSpacingUnrelated;// 7 dlu
    public double LabelHeight;// 8 dlu per line
    public double ParagraphSpacing;// 7 dlu
    public double ClarityIndent;// 10 dlu  -- when there is a label and some controls below, indented.
    public double LabelSpacingFromAssociatedControlTop;//3 dlu -- a label with an edit box, for instance.. gets placed 3 dlu's below the top of the edit

    // dialog
    public double DialogMarginLeft;// 7 dlu
    public double DialogMarginRight;// 7 dlu
    public double DialogMarginTop;// 7 dlu
    public double DialogMarginBottom;// 7 dlu
    public double DialogWideMarginLeft;// 21 dlu
    public double DialogWideMarginRight;// 21 dlu

    // group
    public double GroupMarginLeft;// 9 dlu
    public double GroupMarginTop;// 11 dlu
    public double GroupMarginRight;// 9 dlu
    public double GroupMarginBottom;// 7 dlu

    // controls
    public double ButtonWidth;// 50 dlu
    public double ButtonHeight;// 14 dlu
    public double CheckboxHeight;// 10 dlu
    public double ComboboxHeight;// 14 dlu
    public double RadioButtonHeight;// 10 dlu
    public double EditBoxHeight;// 14 dlu
  };

  // --------------------------------------------------------------------------------
  /*
    An identifier is a symbolic or numeric value, with units.  after it's "translated" into being ALWAYS numeric,
    it's converted to a Position, in pixels
  */
  class Identifier
  {
    public Identifier()
    {
      m_Parts = new System.Collections.ArrayList();
      Clear();
    }
    public void Clear()
    {
      m_Type = Type.Empty;
      m_NumericValue = 0;
      m_Parts.Clear();
    }

    public enum Type
    {
      Empty,
      Flexible,
      Symbolic,// uses m_Parts
      Variable,
      Percent,
      Pixels,
      Dlus,
      Points
    };

    public Type m_Type;
    public double m_NumericValue;
    public System.Collections.ArrayList m_Parts;// of strings
    public Position m_evaluatedFlexDistance;

    // this function converts an identifier to a Position
    // vars maps string -> Identifier
    // output = array of message
    public bool Evaluate(ref Position out_, UISettings uiSettings,
      System.Collections.Hashtable vars, 
      Orientation orientation, ref System.Collections.ArrayList output)
    {
      bool r = false;
      out_.Clear();
 
      switch(m_Type)
      {
        case Identifier.Type.Flexible:
          output.Add(new Message("Unable to evaluate identifier (flexible)."));
          break;
      case Identifier.Type.FormerFlexible:
          out_ = m_evaluatedFlexDistance;
          r = true;
          break;
      case Identifier.Type.Empty:
          output.Add(new Message("Unable to evaluate identifier (empty)."));
          break;
        case Identifier.Type.Percent:
          out_._percent = m_NumericValue;
          r = true;
          break;
        case Identifier.Type.Pixels:
          out_.pixels = m_NumericValue;
          r = true;
          break;
        case Identifier.Type.Dlus:
          {
            out_.dlus = m_NumericValue;
            r = true;
            break;
          }
      case Identifier.Type.Points:
        {
          out_.points = m_NumericValue;
          r = true;
          break;
        }
      case Identifier.Type.Symbolic:
        {
          string identifier = string.Join(".", (string[])m_Parts.ToArray(typeof(string)));
          Identifier temp = new Identifier();
          double i = 0;
          if(!uiSettings.GetElement(ref i, identifier))
          {
            output.Add(new Message("Symbolic identifier '" + identifier + "' unknown"));
          }
          else
          {
            temp.m_Type = Type.Dlus;
            temp.m_NumericValue = i;
            r = temp.Evaluate(ref out_, uiSettings, vars, orientation, ref output);
          }
          break;
        }
      case Identifier.Type.Variable:
        {
          if(m_Parts.Count != 1)
          {
            output.Add(new Message("Invalid variable spec (part count != 1)."));
          }
          else
          {
            Identifier it = vars[m_Parts[0]] as Identifier;
            if(it == null)
            {
              output.Add(new Message("Undefined variable '" + m_Parts[0] + "'."));
            }
            else
            {
              switch(it.m_Type)
              {
              case Identifier.Type.Empty:
              case Identifier.Type.Flexible:
              case Identifier.Type.Symbolic:
              case Identifier.Type.Variable:
                output.Add(new Message("Variable '" + m_Parts[0] + "' is not a value type."));
                break;
              default:
                return it.Evaluate(ref out_, uiSettings, vars, orientation, ref output);
              }
            }
          }
          break;
        }
      default:
        output.Add(new Message("Unknown units."));
        break;
      }
      return r;
    }

    // *assignment,assignment2 (distance)
    public override string ToString()
    {
      // construct a string that represents assignments;
      string assignments = string.Join(".", (string[])m_Parts.ToArray(typeof(string)));
      if(assignments.Length > 0)
      {
        assignments = "*" + assignments;
      }

      string val = m_NumericValue.ToString();

      // type->string
      string type;
      switch(m_Type)
      {
      case Type.Empty:
        type = "empty"; break;
      case Type.Symbolic:
        type = "symbolic"; break;
      case Type.Percent:
        type = val + "%"; break;
      case Type.Pixels:
        type = val + "px"; break;
      case Type.Variable:
        type = "variable"; break;
      case Type.Dlus:
        type = val + "dlu"; break;
      case Type.Points:
        type = val + "pt"; break;
      case Type.Flexible:
        type = "flex"; break;
      default:
        type = "(unknown)"; break;
      }

      return assignments + "(" + type + ")";
    }
  };

  // --------------------------------------------------------------------------------
  class OptimizedPlacement
  {
    public Position m_HStart;
    public Position m_HEnd;
    public Position m_VStart;
    public Position m_VEnd;

    public double nHStart;
    public double nHEnd;
    public double nVStart;
    public double nVEnd;

    public OptimizedPlacement()
    {
      nHStart = 0;
      nHEnd = 0;
      nVStart = 0;
      nVEnd = 0;
    } 

    // resizes p to its parent, based on the placement stuff.
    public void OnSize(System.Windows.Forms.Control hWnd, GUIMetrics m)
    {
      System.Drawing.Rectangle rcClient;// current container's coordinates, based on the parent window.
      rcClient = hWnd.Parent.ClientRectangle;
      OnSize(hWnd, rcClient, m);
    }
    public void OnSize(System.Windows.Forms.Control hWnd, System.Drawing.Rectangle rcClient, GUIMetrics m)
    {
      System.Drawing.Rectangle rcNew = new System.Drawing.Rectangle();
      //      double left = (m_HStart == null) ? hWnd.Left : GetAdjustedCoord(m_HStart.percent, m_HStart.offset, rcClient.Left, rcClient.Right);
      //      double right = (m_HEnd == null) ? hWnd.Right : GetAdjustedCoord(m_HEnd.percent, m_HEnd.offset, rcClient.Left, rcClient.Right);
      //      double top = (m_VStart == null) ? hWnd.Top : GetAdjustedCoord(m_VStart.percent, m_VStart.offset, rcClient.Top, rcClient.Bottom);
      //      double bottom = (m_VEnd == null) ? hWnd.Bottom : GetAdjustedCoord(m_VEnd.percent, m_VEnd.offset, rcClient.Top, rcClient.Bottom);
      int left = (m_HStart == null) ? hWnd.Left : m_HStart.ToPixels(hWnd, m, rcClient, Orientation.Horizontal) ;
      int right = (m_HEnd == null) ? hWnd.Right : m_HEnd.ToPixels(hWnd, m, rcClient, Orientation.Horizontal) ;
      int top = (m_VStart == null) ? hWnd.Top : m_VStart.ToPixels(hWnd, m, rcClient, Orientation.Vertical) ;
      int bottom = (m_VEnd == null) ? hWnd.Bottom : m_VEnd.ToPixels(hWnd, m, rcClient, Orientation.Vertical) ;
      rcNew.X = left;
      rcNew.Y = top;
      rcNew.Width = right - left;
      rcNew.Height = bottom - top;
      hWnd.Bounds = rcNew;
    }

    public string ToString(int i)
    {
      return "";
    }
//
//    private static double GetAdjustedCoord(double percent, double offset, double originalStart, double originalEnd)
//    {
//      return originalStart + offset + ((originalEnd - originalStart) * percent);
//    }
  };


  // --------------------------------------------------------------------------------
  class Command
  {
    public Command()
    {
      m_Children = new System.Collections.ArrayList();
      m_Distance = new Identifier();
      m_Assignment = new System.Collections.ArrayList();
      start = new Position();
      end = new Position();
      Clear();
    }

    public void Clear()
    {
      m_Children.Clear();
      m_Distance.Clear();
      m_Assignment.Clear();
      m_Orientation = Orientation.None;
    }

    public Identifier m_Distance;
    public System.Collections.ArrayList m_Assignment;// array of Identifier

    // if it's a parent node:
    public Orientation m_Orientation;
    public System.Collections.ArrayList m_Children;// array of Command

    // used by the script runner
    public Position start;
    public Position end;

    public string ToString(int indent)
    {
      // construct a string of assignments
      System.Collections.ArrayList assignments = new System.Collections.ArrayList();// of strings

      foreach(Identifier it in m_Assignment)
      {
        assignments.Add(it.ToString());
      }

      string strAssignments = string.Join(", ", (string[])assignments.ToArray(typeof(string)));
      if(strAssignments.Length > 0)
      {
        strAssignments = "*" + strAssignments;
      }

      // orientation to string
      string orientation;
      switch(m_Orientation)
      {
      case Orientation.Vertical:
        orientation = "Vertical";
        break;
      case Orientation.Horizontal:
        orientation = "Horizontal";
        break;
      case Orientation.None:
        orientation = "None";
        break;
      default:
        orientation = "unknown";
        break;
      }

      string sIndent = new string('\t', indent);
      return "orient[" +
                      orientation + "]\r\n" + sIndent + "assign[" +
                      strAssignments + "]\r\n" + sIndent + "distan[" +
                      m_Distance.ToString() + " start=" + start.ToString() + " end=" + end.ToString() + "]";
    }
  };

  // --------------------------------------------------------------------------------
  class PreProcessor
  {
    public PreProcessor()
    {
      start = -1;
      cursor = -1;
      line = 0;
      column = 0;
    }
    public PreProcessor(PreProcessor rhs)
    {
      start = rhs.start;
      cursor = rhs.cursor;
      line = rhs.line;
      column = rhs.column;
      str = rhs.str;
    }
    public void Assign(PreProcessor rhs)
    {
      start = rhs.start;
      cursor = rhs.cursor;
      line = rhs.line;
      column = rhs.column;
      str = rhs.str;
    }
    public void SetString(string s)
    {
      str = s;
      start = 0;
      cursor = 0;
      line = 1;
      column = 1;
      AdvancePastComments();
    }
    public void Clear()
    {
      str = "";
      start = -1;
      cursor = -1;
      line = 0;
      column = 0;
    }
    public bool eof()
    {
      return (cursor == -1) || ((cursor + start) >= str.Length); 
    }

    public bool LessThan(PreProcessor rhs)
    {
      if(rhs.start != start) return false;
      return cursor < rhs.cursor;
    }

    public static PreProcessor LessOf(PreProcessor a, PreProcessor b)
    {
      return a.LessThan(b) ? a : b;
    }

    public static PreProcessor LessOf(PreProcessor a, PreProcessor b, PreProcessor c)
    {
      return LessOf(LessOf(a, b), c);
    }

    public char CurrentChar()
    {
      return cursor == -1 ? '?' : str[cursor];
    }
    public char this[int i]
    {
      get
      {
        if(start == -1) return '?';
        if(cursor == -1) return '?';
        int resultingCursor = i + cursor + start;
        if(resultingCursor < 0 || resultingCursor >= str.Length) return '?';
        return str[resultingCursor];
      }
    }
    public long GetLine() { return line; }
    public long GetColumn() { return column; }
    public PreProcessor FindChar(char c)
    {
      PreProcessor temp = new PreProcessor(this);
      while(!temp.eof())
      {
        if(temp[0] == c)
        {
          return temp;
        }
        temp.Advance();
      }
      return new PreProcessor();// make sure this will be EOF = true
    }
    public string Substring(PreProcessor end)
    {
      if(end.start != start) return "";
      PreProcessor temp = new PreProcessor(this);
      System.Text.StringBuilder r = new System.Text.StringBuilder();

      while(!temp.eof() && (end.cursor > temp.cursor))
      {
        r.Append(temp.CurrentChar());
        temp.Advance();
      }

      return r.ToString();
    }
    public void Advance()
    {
      InternalAdvance();
      AdvancePastComments();
    }

    private bool CursorStartsWith(string s)
    {
      return str.Substring(start + cursor).StartsWith(s);
    }
    private void AdvancePastComments()
    {
      for(;;)
      {
        if(CursorStartsWith("//"))
        {
          // advance until a newline.
          long oldLine = line;
          while((!eof()) && (line == oldLine))
          {
            InternalAdvance();
          }
        }
        else if(CursorStartsWith("/*"))
        {
          while(!CursorStartsWith("*/"))
          {
            InternalAdvance();
          }
          // go past the terminator
          InternalAdvance();
          InternalAdvance();
        }
        else
        {
          break;
        }
      }
    }
    // adjusts line & column while advancing 1 char.
    private void InternalAdvance()
    {
      // adjust new line
      if(CurrentChar() == '\n')
      {
        line ++;
        column = 0;
      }

      if((start + cursor) < str.Length)
      {
        cursor ++;
        column ++;
      }

      if((start + cursor) < str.Length)
      {
        // skip \r characters altogether
        if(CurrentChar() == '\r')
        {
          cursor ++;
        }
      }
    }

    private int start;// position relative to str[0]
    private int cursor;// position relative to start
    private string str;
    private long line;
    private long column;
  };

  // --------------------------------------------------------------------------------
  class Manager
  {
    public Manager()
    {
      m_output = new System.Collections.ArrayList();
      m_root = new Command();
      m_script = new PreProcessor();
      m_symbols = new System.Collections.Hashtable();
      m_variables = new System.Collections.Hashtable();
      m_settings = new UISettings();
      m_settings.LoadDefaults();
    }

    public void RegisterSymbol(string name, System.Windows.Forms.Control hwnd)
    {
      m_symbols[name.ToLower()] = new SymbolStorage(hwnd);
    }

    public void RegisterSymbolsAutomatically(System.Windows.Forms.Control parent)
    {
      System.Reflection.MemberInfo[] members = parent.GetType().GetMembers(
        System.Reflection.BindingFlags.GetField |
        System.Reflection.BindingFlags.Instance |
        System.Reflection.BindingFlags.NonPublic |
        System.Reflection.BindingFlags.Public);

      foreach(System.Reflection.MemberInfo member in members)
      {
        // must be a field
        if(member.MemberType == System.Reflection.MemberTypes.Field)
        {
          System.Reflection.FieldInfo field = member as System.Reflection.FieldInfo;
          // must be a Control
          if(field.FieldType.IsSubclassOf(typeof(System.Windows.Forms.Control)))
          {
            // ok wire it up.
            Warning("Automatic symbol: " + field.Name);
            RegisterSymbol(field.Name, field.GetValue(parent) as System.Windows.Forms.Control);
          }
        }
      }
    }

    public void RegisterVariable(string name, double pixels)
    {
      Identifier i = new Identifier();
      i.m_NumericValue = pixels;
      i.m_Type = Identifier.Type.Pixels;
      string nameLower = name.ToLower();
      m_variables[nameLower] = i;
    }
    public void RegisterVariable(string name, string spec)
    {
      Identifier i = new Identifier();
      if(ParseIdentifier(spec, ref i))
      {
        string nameLower = name.ToLower();
        m_variables[nameLower] = i;
      }
    }
    public bool RunString(string script, System.Windows.Forms.Control parent)
    {
      bool r = false;
      m_root.Clear();
      m_output.Clear();

      m_parent = parent;

      // register all symbols automatically.
      RegisterSymbolsAutomatically(parent);

      m_script.SetString(script);
  
      string tok = "";
      while(true)
      {
        SkipWhitespaceAndNewlines();
        if(m_script.eof())
        {
          // end of file
          break;
        }
        Command This = new Command();
        ParseIdentifierPart(ref tok);
        if(m_script.eof())
        {
          Error("Expected global 'vertical' or 'horizontal' block.  EOF found while parsing the block.");
          break;
        }
        SkipWhitespaceAndNewlines();
        if(m_script.CurrentChar() != '{')
        {
          Error("Expected '{', not found.");
          break;
        }
  
        m_script.Advance();// skip past the open brace.
  
        if(tok == "vertical")
        {
          This.m_Orientation = Orientation.Vertical;
          ParseChildBlock(ref This);
          m_root.m_Children.Add(This);
        }
        else if(tok == "horizontal")
        {
          This.m_Orientation = Orientation.Horizontal;
          ParseChildBlock(ref This);
          m_root.m_Children.Add(This);
        }
        else
        {
          Error("Unrecognized characters at global scope (expected 'vertical' or 'horizontal')");
        }
  
        // now skip past the semi-colon.  there should be NO cursor movement at this level.
        SkipWhitespaceAndNewlines();
        if(m_script.eof() || m_script.CurrentChar() != ';')
        {
          Error("Missing semi-colon after '" + tok + "' block");
        }
        if(!m_script.eof())
        {
          m_script.Advance();
        }
      }
  
      m_script.Clear();
  
      int indent = 0;
  
      // now interpret everything.
      r = Run();

      OnSize();

      foreach(string name in m_symbols.Keys)
      {
        SymbolStorage ss = m_symbols[name] as SymbolStorage;
        if(ss.optimizedPlacement == null)
        {
          Warning("Symbol '" + name + "' was never assigned to. It will be ignored when resizing.");
        }
      }

      DumpNode(m_root, ref indent);
      DumpSymbols();
  
      parent.Resize += new EventHandler(Internal_OnSize);

      return r;
    }

    public bool RunFile(string fileName, System.Windows.Forms.Control parent)
    {
      m_output.Clear();
      System.IO.TextReader f = new System.IO.StreamReader(fileName);
      string s = f.ReadToEnd();
      f.Close();
      return RunString(s, parent);
    }

    public bool OnSize()
    {
      GUIMetrics m = new GUIMetrics(m_parent);

      foreach(SymbolStorage ss in m_symbols.Values)
      {
        if(ss.optimizedPlacement != null)
        {
          ss.optimizedPlacement.OnSize(ss.hwnd, m);
        }
      }
      return true;
    }

    public void DumpOutput()
    {
      foreach(Message it in m_output)
      {
        Debug.WriteLine(it.ToString());
      }
    }
    public static void DumpNode(Command n, ref int indent)
    {
      string sIndent = new string('\t', indent);
      Debug.Write(sIndent);
      Debug.WriteLine(n.ToString(indent));
      Debug.WriteLine(sIndent + "{");
      indent ++;
      foreach(Command child in n.m_Children)
      {
        DumpNode(child, ref indent);
      }
      indent --;
      Debug.WriteLine(sIndent + "}");
    }

    public void DumpSymbols()
    {
      foreach(string key in m_symbols.Keys)// TODO
      {
        SymbolStorage val = m_symbols[key] as SymbolStorage;
        Debug.WriteLine("[" + key + "]");
        Debug.WriteLine("{");
        Debug.WriteLine(val.ToString(1));
        Debug.WriteLine("}");
        Debug.WriteLine("");
      }
    }

    public System.Collections.ArrayList m_output;// array of Message
    public UISettings m_settings;

    private void SkipWhitespaceAndNewlines()
    {
      while(!m_script.eof())
      {
        if(!IsWhitespace(m_script.CurrentChar()) && !IsNewline(m_script.CurrentChar()))
        {
          break;
        }
        m_script.Advance();
      }
    }

    private bool ParseIdentifierPart(ref string token)
    {
      SkipWhitespaceAndNewlines();
      token = "";
      while(!m_script.eof())
      {
        if(!IsIdentifierChar(m_script.CurrentChar()))
        {
          break;
        }
        token += m_script.CurrentChar();
        m_script.Advance();
      }
      return token.Length != 0;
    }

    private bool ParseIdentifier(string s_, ref Identifier out_)
    {
      bool r = true;
      string s = s_.Trim(" \t\r\n".ToCharArray());

      // determine if it's a value like '100%' or '15px', or if it's a real identifier.
      if(Util.StartsWithNumber(s))// hmm this doesnt account for negative numbers or something like ".14". need to revise.
      {
        // parse a numeric identifier.
        string szSuffix = "";
        double val = 0;
        if(!Util.StringToDouble(s, ref val, ref szSuffix))
        {
          Error("Error parsing identifier '" + s + "'. The number format may be incorrect.");
        }
        out_.m_NumericValue = val;

        string suffix = szSuffix.ToLower().Trim(" \t\r\n".ToCharArray());
        if(suffix == "px" || suffix == "pixel" || suffix == "pixels" || suffix == "")
        {
          out_.m_Type = Identifier.Type.Pixels;
        }
        else if(suffix == "%" || suffix == "percent")
        {
            out_.m_NumericValue = val / 100.0;
            out_.m_Type = Identifier.Type.Percent;
        }
        else if(suffix == "pt" || suffix == "point" || suffix == "points")
        {
          out_.m_Type = Identifier.Type.Points;
        }
        else if(suffix == "dlu" || suffix == "dlus")
        {
          out_.m_Type = Identifier.Type.Dlus;
        }
        else
        {
          // error: unknown numeric prefix.  expected something like 'px', '%', 'points'
          Error("Unknown units '" + suffix + "'");
          r = false;
        }
      }
      else
      {
        // parse a symbolic identifier.
        out_.m_Type = Identifier.Type.Symbolic;

        if(s == "flex" || s == "flexible")
        {
          out_.m_Type = Identifier.Type.Flexible;
        }
        else if((s.Length > 0) && s[0] == '<')
        {
          // variable.
          out_.m_Type = Identifier.Type.Variable;
          int n = s.IndexOf('>');
          if(n == -1)
          {
            Error("Malformed variable name... missing '>' on identifier '" + s + "'");
          }
          else
          {
            out_.m_Parts.Clear();
            // remove < > and trim
            string trimmed = s.Substring(1, n - 1).Trim(" \t\r\n".ToCharArray()).ToLower();
            
            if(Util.StringContainsNotOf(trimmed, GetIdentifierChars()))
            {
              // error - unrecognized character in variable
              Error("Unrecognized character in variable '" + trimmed + "'");
              r = false;
            }
            else
            {
              out_.m_Parts.Add(trimmed);
            }
          }
        }
        else
        {
          // symbolic
          string[] Parts = s.Split(".".ToCharArray());

          foreach(string part in Parts)
          {
            string trimmed = part.Trim(" \t\r\n".ToCharArray()).ToLower();

            // verify.  at this point we should have a part with nothing but identifier chars.
            if(Util.StringContainsNotOf(trimmed, GetIdentifierChars()))
            {
              // error - unrecognized character in identifier
              Error("Unrecognized character in identifier '" + trimmed + "'");
              r = false;
            }
            else
            {
              out_.m_Parts.Add(trimmed);
            }
          }
        }
      }
      return r;
    }

    private bool ParseIdentifierList(string sList, ref System.Collections.ArrayList out_)// out = Identifier list
    {
      bool r = true;
      string[] Identifiers = sList.Split(",".ToCharArray());
  
      foreach(string it in Identifiers)
      {
        Identifier x = new Identifier();
        if(ParseIdentifier(it, ref x))
        {
          out_.Add(x);
        }
        else
        {
          r = false;
        }
      }
      return r;
    }

    // --------------------------------------------------------------------------------
    // a command is in the form [* assignment list ] [{ children; }] [distance];
    private bool ParseCommand(ref Command This)
    {
      bool r = false;
      // part . part . part (something) ;
      bool bErrors = false;
      SkipWhitespaceAndNewlines();
      bool bHasAssignments = false;
  
      if(m_script.CurrentChar() != '*' && m_script.CurrentChar() != '(' && m_script.CurrentChar() != '{')
      {
        Error("Unrecognized character in command.  Expected '(', '{', or '*'.  Make sure you prefix assignments with an asterisk.");
        bErrors = true;
      }
      else
      {
        if(m_script.CurrentChar() == '*')
        {
          bHasAssignments = true;
          m_script.Advance();// ignore the asterisk; it's really not that important.
          SkipWhitespaceAndNewlines();
        }
  
        PreProcessor openbrace = m_script.FindChar('{');
        PreProcessor semicolon = m_script.FindChar(';');
        PreProcessor openparen = m_script.FindChar('(');
        PreProcessor closedparen = m_script.FindChar(')');
  
        if((openbrace.LessThan(openparen)) && (openbrace.LessThan(semicolon)))
        {
          // we have ourselves a parent node.  parse it out before continuing.
          PreProcessor temp = new PreProcessor(m_script);
          m_script.Assign(openbrace);
          m_script.Advance();
          ParseChildBlock(ref This);
  
          semicolon = m_script.FindChar(';');
          openparen = m_script.FindChar('(');
          closedparen = m_script.FindChar(')');
  
          m_script.Assign(temp);
        }
  
        if(!openparen.eof() && closedparen.eof())
        {
          Error("Syntax error while parsing a command.  Closed parenthesis not found.");
          bErrors = true;
        }
        if(semicolon.eof())
        {
          Error("Syntax error while parsing a command.  Semicolon not found.");
          bErrors = true;
        }
  
        string distance = "";
  
        if(openparen.eof() || (closedparen.LessThan(openparen)) || (semicolon.LessThan(openparen)))
        {
          // no parenthesis.  this means no distance.
          distance = "flex";
        }
  
        if(!bErrors)
        {
          if(bHasAssignments)
          {
            // extract tokens
            string assignment;
  
            PreProcessor endOfAssignments = PreProcessor.LessOf(openbrace, openparen, semicolon);// in order of preference, 1) open brace, 2) open paren, 3) semicolon, whichever comes first & is not EOL.
  
            assignment = m_script.Substring(endOfAssignments);
            ParseIdentifierList(assignment, ref This.m_Assignment);
          }
  
          if((0 == distance.Length) && !openparen.eof())
          {
            openparen.Advance();
            distance = openparen.Substring(closedparen);
          }
          r = ParseIdentifier(distance, ref This.m_Distance);
  
          m_script.Assign(semicolon);
          m_script.Advance();// advance past the semicolon.
        }
      }
  
      return r;
    }

    private bool ParseChildBlock(ref Command parent)
    {
      bool r = true;
      while(true)
      {
        Command This = new Command();
        This.m_Orientation = parent.m_Orientation;
        SkipWhitespaceAndNewlines();
        if(m_script.eof())
        {
          // error: no closing brace found.
          Error("No closing brace found on block.");
          r = false;
          break;
        }
        if(m_script.CurrentChar() == '}')
        {
          m_script.Advance();
          // clean break;
          break;
        }
        if(!ParseCommand(ref This))
        {
          r = false;
          break;
        }
        parent.m_Children.Add(This);
      }
      return r;
    }

    private bool Run()
    {
      System.Collections.Stack st = new System.Collections.Stack();// stack of Commands
      bool r = true;

    /*
     
     * THIS NEEDS TO BE PORTED FROM TEH c++ VERSION
     
     */

      //m_root.start = Position.Begin();
      //m_root.end = Position.End();

      //foreach(Command it in m_root.m_Children)
      //{
      //  st.Push(it);
      //  // all "root" blocks (vertical/horizontal) start at 0 and end at 100%
      //  it.start = Position.Begin();
      //  it.end = Position.End();
      //  ParseIdentifier("100%", ref it.m_Distance);
      //  r = RunCommand(ref st);
      //  st.Pop();
      //}

      return r;
    }
    
    //private bool RunCommand(ref System.Collections.Stack st)// stack of Commands
    //{
    //  bool r = true;
    //  Command node = st.Peek() as Command;
  
    //  /*
    //      fill start and end cursors for all child nodes.
    //      1) first go through and figure out how much distance we have total (and what's left for flex nodes)
    //      2) evenly distribute the rest to flex nodes' distances.
    //      3) go through and iteratively fill in start/end.
    //  */
    //  int nFlexible = 0;

    //  Position totalDistance = new Position();
    //  foreach(Command it in node.m_Children)
    //  {
    //    if(it.m_Distance.m_Type == Identifier.Type.Flexible)
    //    {
    //      nFlexible ++;
    //    }
    //    else
    //    {
    //      Position thisDistanceXX = new Position();
    //      if(!it.m_Distance.Evaluate(ref thisDistanceXX, m_settings, m_variables, it.m_Orientation, ref m_output))
    //      {
    //        Error("Error while advancing the internal cursor.");
    //        r = false;
    //      }
    //      totalDistance.Add(thisDistanceXX);
    //    }
    //  }

    //  Position flexDistance = Position.End();
    //  flexDistance.Subtract(totalDistance);
    //  if(nFlexible > 0)
    //  {
    //    flexDistance.Divide(nFlexible);
    //  }

    //  Position tempCursorStart = Position.Begin();
    //  Position tempCursorEnd = new Position();
    //  Position thisDistance = new Position();
    //  foreach(Command it in node.m_Children)
    //  {
    //    // calculate cursor start & end
    //    tempCursorEnd.Assign(tempCursorStart);
    //    if(it.m_Distance.m_Type == Identifier.Type.Flexible)
    //    {
    //      thisDistance.Assign(flexDistance);
    //    }
    //    else
    //    {
    //      if(!it.m_Distance.Evaluate(ref thisDistance, m_settings, m_variables, it.m_Orientation, ref m_output))
    //      {
    //        Error("Error while advancing the internal cursor.");
    //        r = false;
    //      }
    //    }

    //    tempCursorEnd.Add(thisDistance);
    //    it.start.Assign(tempCursorStart);
    //    it.end.Assign(tempCursorEnd);
    //    tempCursorStart.Assign(tempCursorEnd);
    //  }

    //  if(r)
    //  {
    //    // ok now all children have start/end filled in.  do our own assignments
    //    if(!DoCommandAssignments(st))
    //    {
    //      Error("Error while assigning.");
    //    }
    //    else
    //    {
    //      // run children
    //      foreach(Command itNode in node.m_Children)
    //      {
    //        st.Push(itNode);
    //        if(!RunCommand(ref st))
    //        {
    //          r = false;
    //        }
    //        st.Pop();
    //      }
    //    }
    //  }
    //  return r;
    //}


    // --------------------------------------------------------------------------------
    // does assignments for the top of the stack.
    private bool DoCommandAssignments(System.Collections.Stack st)
    {
      bool r = true;
      Command pc = st.Peek() as Command;
  
      System.Collections.IEnumerator it;
  
      // build our placement spec that we will assign to everything in the assignment list.
      Position start = new Position();
      Position end = new Position();
      /*
        Right here, we have a hierarchy of regions.  Need to calculate this into a placement spec.
        We start at child and work our way out, adjusting the coords to the parent's each iteration.
        0%                                                   100%
        |-----------------------------------------------------|   (distance = 100%+0)
            15%+5                                       90%-4
              |-------------------------------------------|        (distance = 75%+1)
                  23%-6                       70%+13
                    |----------------------------|                 (distance = 47%+7)
                          35%+32    62%-64
                              |---------|                          (distance = 27%-32)

        Each child gets this sort of treatment to adjust it's coords to be at the paren't's scope:
        child_position = parent_start + (parent_distance * child_position)

        to re-word that, calculate the child position in terms of the parent, and offset it to where the parent's position is.

        so, starts for the above would look something like:
        x = 10% + (60% * 25%) = 10% + 15% = 25%
        40% + (50% * x) = 40% + (50% * 25%) = 40% + 12.5% = 52.5%
        0 + (100% * 52.5%) = ** 52.5% **
      */
      if(0 == st.Count)
      {
        start = Position.Begin();
        end = Position.End();
      }
      else
      {
        // use the first one
        it = st.GetEnumerator();
        it.MoveNext();// guaranteed.
        Command child = it.Current as Command;

        start.Assign(child.start);
        end.Assign(child.end);

        while(it.MoveNext())// iterate from child towards parent
        {
          // it points to parent.  child points to the ... child.
          Command parent = it.Current as Command;
          // start = parent_start + (parent_distance * start)
          start.TranslateRelativeTo(parent.start, parent.end);
          end.TranslateRelativeTo(parent.start, parent.end);
        }
      }
  
      // now do the assignments.
      foreach(Identifier i in pc.m_Assignment)
      {
        // find the identifier in the list.
        // meaning of identifier parts at this point:
        // first: the actual symbol that's been registered
        // second: the member.  you could choose to only set the beginning cursor to a particular coordinate.
        string Symbol = "";
        string Member = "";
  
        if(i.m_Type != Identifier.Type.Symbolic)
        {
          Error("L-value expected. Can only assign values to symbols.");
          r = false;
          break;
        }
  
        switch(i.m_Parts.Count)
        {
        case 1:
          Symbol = i.m_Parts[0] as string;
          break;
        case 2:
          Symbol = i.m_Parts[0] as string;
          Member = i.m_Parts[1] as string;
          break;
        default:
          Error("Wrong number of identifier parts.");
          r = false;
          break;
        }
        if(!r) break;
  
        // find the symbol.
        SymbolStorage ss = m_symbols[Symbol] as SymbolStorage;
        if(ss == null)// TODO: actually range check instead of throwing
        {
          // error: symbol '' not found.
          Error("Symbol '" + Symbol + "' not registered.");
          r = false;
          break;
        }

        if(ss.optimizedPlacement == null)
        {
          ss.optimizedPlacement = new OptimizedPlacement();
        }

        if(!DoCommandAssignment(pc, start, end, ref ss.optimizedPlacement, Member))
        {
          // error propagating
          Error("Error while assigning to '" + Symbol + "'.");
          r = false;
          break;
        }
      }
  
      return r;
    }
  
    // --------------------------------------------------------------------------------
    bool DoCommandAssignment(Command pc, Position start, Position end, ref OptimizedPlacement pPlacement, string member)
    {
      bool r = true;
  
      // figure out what parts will be set.
      if(member.Length == 0)
      {
        // figure it out based on orientation.
        if(pc.m_Orientation == Orientation.Vertical)
        {
          pPlacement.m_VStart = new Position(start);
          pPlacement.m_VEnd = new Position(end);
        }
        else if(pc.m_Orientation == Orientation.Horizontal)
        {
          pPlacement.m_HStart = new Position(start);
          pPlacement.m_HEnd = new Position(end);
        }
        else
        {
          Error("Unknown orientation.");
          r = false;
        }
      }
      else
      {
        // here, they have specified which member to set. valid are left, top, right, bottom.  it must match the current orientation.
        if(pc.m_Orientation == Orientation.Vertical)
        {
          if(member == "top")
          {
            pPlacement.m_VStart = new Position(start);
          }
          else if(member == "bottom")
          {
            pPlacement.m_VEnd = new Position(end);
          }
          else if(member == "left" || member == "right")
          {
            Error("Mismatched orientation on member '" + member + "'.");
            r = false;
          }
          else
          {
            Error("Unrecognized member '" + member + "'.");
            r = false;
          }
        }
        else if(pc.m_Orientation == Orientation.Horizontal)
        {
          if(member == "left")
          {
            pPlacement.m_HStart = new Position(start);
          }
          else if(member == "right")
          {
            pPlacement.m_HEnd = new Position(end);
          }
          else if(member == "top" || member == "bottom")
          {
            Error("Mismatched orientation on member '" + member + "'.");
            r = false;
          }
          else
          {
            Error("Unrecognized member '" + member + "'.");
            r = false;
          }
        }
        else
        {
          Error("Unknown orientation.");
          r = false;
        }
      }
  
      return r;
    }
  
    private void Internal_OnSize(object sender, System.EventArgs e)
    {
      OnSize();
    }

    private void Warning(string msg)
    {
      Message m = new Message(msg, Message.Severity.Warning, m_script.GetLine(), m_script.GetColumn());
      m_output.Add(m);
      Debug.WriteLine("RealTime: " + m.ToString());
    }
    private void Error(string msg)
    {
      Message m = new Message(msg, Message.Severity.Error, m_script.GetLine(), m_script.GetColumn());
      m_output.Add(m);
      Debug.WriteLine("RealTime: " + m.ToString());
    }

    private static string GetIdentifierChars()
    {
      return "abcdefghijklmnopqrstuvwxyz0123456789_";
    }
    private static bool IsWhitespace(char x)
    {
      return (x == ' ') || (x == '\t');
    }
    private static bool IsNewline(char x)
    {
      return (x == '\r') || (x == '\n');
    }
    private static bool IsIdentifierChar(char x)
    {
      return GetIdentifierChars().IndexOf(x) != -1;
    }

    public Command m_root;
    System.Windows.Forms.Control m_parent;
    // line numbers.
    private PreProcessor m_script;

    private class SymbolStorage
    {
      public SymbolStorage(System.Windows.Forms.Control h)
      {
        optimizedPlacement = null;
        hwnd = h;
      }
      public SymbolStorage()
      {
        optimizedPlacement = null;
        hwnd = null;
      }
      public SymbolStorage(SymbolStorage rhs)
      {
        hwnd = rhs.hwnd;
        optimizedPlacement = rhs.optimizedPlacement;
      }
      public string ToString(int indent)
      {
        if(optimizedPlacement == null)
        {
          return "(null)";
        }
        return optimizedPlacement.ToString(indent);
      }

      public OptimizedPlacement optimizedPlacement;
      public System.Windows.Forms.Control hwnd;
    };

    private System.Collections.Hashtable m_symbols;// maps string -> SymbolStorage
    private System.Collections.Hashtable m_variables;// maps string -> Identifier
  };
}

