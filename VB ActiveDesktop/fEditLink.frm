VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form fEditLink 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Edit Link"
   ClientHeight    =   5445
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6285
   Icon            =   "fEditLink.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5445
   ScaleWidth      =   6285
   StartUpPosition =   3  'Windows Default
   Begin VB.DirListBox dPathDirectory 
      Height          =   4590
      Left            =   4080
      TabIndex        =   20
      Top             =   585
      Width           =   2055
   End
   Begin VB.DriveListBox dPathDrive 
      Height          =   315
      Left            =   4080
      TabIndex        =   19
      Top             =   240
      Width           =   2055
   End
   Begin VB.CommandButton cColor 
      Caption         =   "Font Color"
      Height          =   375
      Left            =   240
      TabIndex        =   18
      Top             =   4800
      Width           =   975
   End
   Begin VB.Frame Frame2 
      Caption         =   "Alignment"
      Height          =   615
      Left            =   120
      TabIndex        =   14
      Top             =   2280
      Width           =   3735
      Begin VB.OptionButton oRight 
         Caption         =   "Right"
         Height          =   195
         Left            =   2648
         TabIndex        =   17
         Top             =   240
         Width           =   975
      End
      Begin VB.OptionButton oCenter 
         Caption         =   "Center"
         Height          =   195
         Left            =   1448
         TabIndex        =   16
         Top             =   240
         Width           =   975
      End
      Begin VB.OptionButton oLeft 
         Caption         =   "Left"
         Height          =   195
         Left            =   368
         TabIndex        =   15
         Top             =   240
         Width           =   975
      End
   End
   Begin VB.CommandButton cFont 
      Caption         =   "Font"
      Height          =   375
      Left            =   240
      TabIndex        =   12
      Top             =   4440
      Width           =   975
   End
   Begin VB.CommandButton cCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   2640
      TabIndex        =   11
      Top             =   4800
      Width           =   1095
   End
   Begin VB.CommandButton cOK 
      Caption         =   "Ok"
      Height          =   375
      Left            =   2640
      TabIndex        =   10
      Top             =   4440
      Width           =   1095
   End
   Begin MSComDlg.CommonDialog CommonDialog 
      Left            =   0
      Top             =   -120
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.Frame Frame1 
      Height          =   1095
      Left            =   120
      TabIndex        =   5
      Top             =   1080
      Width           =   3735
      Begin VB.CommandButton cBrowsePath 
         Caption         =   "Browse Path..."
         Enabled         =   0   'False
         Height          =   375
         Left            =   1560
         TabIndex        =   9
         Top             =   600
         Width           =   1815
      End
      Begin VB.CommandButton cBrowseFile 
         Caption         =   "Browse File..."
         Enabled         =   0   'False
         Height          =   375
         Left            =   1560
         TabIndex        =   8
         Top             =   240
         Width           =   1815
      End
      Begin VB.OptionButton oPath 
         Caption         =   "Link to Path"
         Height          =   195
         Left            =   120
         TabIndex        =   7
         Top             =   720
         Width           =   1815
      End
      Begin VB.OptionButton oFile 
         Caption         =   "Link to File"
         Enabled         =   0   'False
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   240
         Width           =   1335
      End
   End
   Begin VB.TextBox tLink 
      Height          =   615
      Left            =   960
      MultiLine       =   -1  'True
      TabIndex        =   1
      Top             =   480
      Width           =   2895
   End
   Begin VB.TextBox tLinkName 
      Height          =   375
      Left            =   960
      TabIndex        =   0
      Top             =   180
      Width           =   2895
   End
   Begin VB.TextBox tPreview 
      Height          =   2055
      Left            =   120
      Locked          =   -1  'True
      TabIndex        =   13
      Top             =   3240
      Width           =   3735
   End
   Begin VB.Label Label3 
      Caption         =   "Preview:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   3000
      Width           =   1575
   End
   Begin VB.Label Label2 
      Caption         =   "Path/File:"
      Height          =   255
      Left            =   180
      TabIndex        =   3
      Top             =   660
      Width           =   735
   End
   Begin VB.Label Label1 
      Caption         =   "Link Name"
      Height          =   255
      Left            =   60
      TabIndex        =   2
      Top             =   240
      Width           =   855
   End
End
Attribute VB_Name = "fEditLink"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Dim eNumber As Integer 'the LinkNumber we're editing.
Dim eAlignment As String
Dim eText As String
Dim eColor As String
Dim eFontName As String
Dim eFontSize As String
Dim eBold As Boolean
Dim eItalic As Boolean
Dim eLink As String
Dim eLinkName As String


Private Sub cColor_Click()
'change the font's color
CommonDialog.CancelError = True
On Error GoTo CDCancel

CommonDialog.Flags = cdCClFullOpen
CommonDialog.ShowColor
tPreview.ForeColor = CommonDialog.Color

CDCancel:
End Sub

Private Sub cOK_Click()
'check if we've entered all the required data
If tLink.Text = "" Or tLinkName.Text = "" Then
    If tLink.Text = "" Then
        MsgBox "You need to enter the link content.", vbOKOnly, "Required field missing"
        tLink.SetFocus
    Else
        If tLinkName.Text = "" Then
            MsgBox "You need to enter the link name.", vbOKOnly, "Required field missing"
            tLinkName.SetFocus
        End If
    End If
Else
        If oLeft = True Then eAlignment = "LEFT"
        If oCenter = True Then eAlignment = "CENTER"

        eColor = ConvertColor(tPreview.ForeColor) 'this one
        eFontName = tPreview.Font
        eFontSize = Format(Int(tPreview.FontSize / 3))
        eBold = tPreview.FontBold
        eItalic = tPreview.FontItalic
        eLink = tLink.Text
        eText = tLinkName.Text
    Unload fEditLink 'close teh Edit dialog
    'and now update the TEXT file by passing these values to fMain.
    fMain.Edit eNumber, eAlignment, eColor, eFontName, eFontSize, eBold, eItalic, eLink, eText
End If
End Sub

Private Sub cCancel_Click()
Unload fEditLink
End Sub


Private Sub cBrowseFile_Click()
oFile.Value = True
'open the commondialogbox
CommonDialog.Flags = cdlOFNHideReadOnly
CommonDialog.Filter = "All Files (*.*)|*.*"
CommonDialog.ShowOpen

tLink = CommonDialog.FileName

End Sub

Private Sub cBrowsePath_Click()
oPath.Value = True
'get the path

End Sub


Private Sub cFont_Click()
CommonDialog.CancelError = True
On Error GoTo CDCancel1

CommonDialog.Flags = cdlCFScreenFonts
CommonDialog.ShowFont

'now set the attributes of the tPreview box to those that were selected.
With tPreview
.Font = CommonDialog.FontName
.FontSize = CommonDialog.FontSize
.FontBold = CommonDialog.FontBold
.FontItalic = CommonDialog.FontItalic
.FontUnderline = True
End With
CDCancel1:
End Sub


Private Sub dPathDirectory_Change()
tLink.Text = dPathDirectory.Path

End Sub

Private Sub dPathDrive_Change()
dPathDirectory.Path = dPathDrive.Drive

End Sub

Private Sub tLinkName_Change()
tPreview.Text = tLinkName.Text

End Sub
Function ConvertColor(Orig As Long) As String
ConvertColor = "#" & fHex$(Orig Mod 256) & _
                     fHex$((Orig Mod 65536) \ 256) & _
                     fHex$(Orig \ 65536)
If Len(ConvertColor) > 7 Then ConvertColor = "#FFFFFF"
'B = Orig \ 65536
'G = (Orig Mod 65536) \ 256
'R = Orig Mod 256
End Function
Sub EditInit(LinkNumber As Integer, Code As String)
    eNumber = LinkNumber
    'display all the characteristics of the code.
    Dim position As Integer
    Dim position2 As Integer
    Dim tString As String
    Dim R As Integer
    Dim G As Integer
    Dim B As Integer
    'we need:
    
    'alignment - not used in this module,though.
    'fontname
    'size
    'color
    'link
    'name
    'bold
    'italic
        
    'find the alignment
    position = InStr(1, Code, "ALIGN=", vbTextCompare) + 7
    position2 = InStr(position, Code, Chr$(34), vbTextCompare)
    eAlignment = Mid(Code, position, position2 - position)
    Select Case eAlignment
        Case "RIGHT":
            oRight = True
        Case "LEFT":
            oLeft = True
        Case "CENTER":
            oCenter = True
    End Select
    'find fontname
    position = InStr(1, Code, "FACE=", vbTextCompare) + 6
    position2 = InStr(position, Code, Chr$(34), vbTextCompare)
    tPreview.FontName = Mid(Code, position, position2 - position)
    'find size
    position = InStr(1, Code, "SIZE=", vbTextCompare) + 5
    position2 = InStr(position, Code, Chr$(32), vbTextCompare)
    tPreview.FontSize = Val(Mid(Code, position, position2 - position)) * 4
    'find color
    position = InStr(1, Code, "COLOR=", vbTextCompare) + 8
    position2 = InStr(position + 1, Code, Chr$(34), vbTextCompare)
    eColor = Mid(Code, position, position2 - position)
    'now convert it to RGB.
        R = Val("&H" & Left(eColor, 2))
        G = Val("&H" & Mid(eColor, 2, 2))
        B = Val("&H" & Right(eColor, 2))
    tPreview.ForeColor = RGB(R, G, B)
    
    'find link
    position = InStr(1, Code, "HREF=", vbTextCompare) + 6
    position2 = InStr(position + 1, Code, Chr$(34), vbTextCompare)
    tLink.Text = Mid(Code, position, position2 - position)
    'find name
    position = InStr(1, Code, vbCrLf, vbTextCompare)
    position = InStr(position + 1, Code, vbCrLf, vbTextCompare) + 2
    position2 = InStr(position + 1, Code, vbCrLf, vbTextCompare)
    tLinkName.Text = Mid(Code, position, position2 - position)
    'find bold
    tPreview.FontBold = False
    If InStr(1, Code, "<B>", vbTextCompare) Then tPreview.FontBold = True
    'find italic
    tPreview.FontItalic = False
    If InStr(1, Code, "<I>", vbTextCompare) Then tPreview.FontItalic = True
    tPreview.Text = tLinkName.Text
    oPath.Value = True
    On Error GoTo Err1
    dPathDirectory.Path = tLink.Text
Err1:
    
    
    fEditLink.Show
    
End Sub
Function fHex$(Number As Long)
fHex$ = Hex$(Number)
If Len(fHex$) = 1 Then fHex$ = "0" & fHex$

End Function
