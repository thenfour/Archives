VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form fMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "ADD - ActiveDesktop Designer"
   ClientHeight    =   5730
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   4125
   Icon            =   "fMain.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5730
   ScaleWidth      =   4125
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cUpdate 
      Caption         =   "Update Desktop"
      Height          =   375
      Left            =   2640
      TabIndex        =   9
      Top             =   2280
      Width           =   1335
   End
   Begin VB.PictureBox pSample 
      Height          =   255
      Left            =   120
      ScaleHeight     =   195
      ScaleWidth      =   2355
      TabIndex        =   8
      Top             =   2400
      Width           =   2415
   End
   Begin VB.ListBox lList 
      Height          =   2010
      ItemData        =   "fMain.frx":2D9A
      Left            =   120
      List            =   "fMain.frx":2D9C
      TabIndex        =   0
      Top             =   360
      Width           =   2415
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   1560
      Top             =   2520
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
   End
   Begin VB.CommandButton cEdit 
      Caption         =   "Edit Link"
      Height          =   375
      Left            =   2640
      TabIndex        =   5
      Top             =   1680
      Width           =   1335
   End
   Begin VB.CommandButton cInsert 
      Caption         =   "Insert Link"
      Height          =   375
      Left            =   2640
      TabIndex        =   4
      Top             =   720
      Width           =   1335
   End
   Begin VB.TextBox Text1 
      Height          =   2655
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   3
      Top             =   3000
      Width           =   3855
   End
   Begin VB.CommandButton cDelete 
      Caption         =   "Delete Link"
      Height          =   375
      Left            =   2640
      TabIndex        =   2
      Top             =   1320
      Width           =   1335
   End
   Begin VB.CommandButton cAdd 
      Caption         =   "Add New Link"
      Height          =   375
      Left            =   2640
      TabIndex        =   1
      Top             =   360
      Width           =   1335
   End
   Begin VB.Label Label2 
      Caption         =   "Link Names:"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   120
      Width           =   1815
   End
   Begin VB.Label Label1 
      Caption         =   "HTML Code:"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   2760
      Width           =   1815
   End
   Begin VB.Menu mFile 
      Caption         =   "&File"
      Begin VB.Menu mFileNew 
         Caption         =   "&New"
      End
      Begin VB.Menu mFile3 
         Caption         =   "-"
      End
      Begin VB.Menu mFileOpen 
         Caption         =   "&Open HTML File..."
      End
      Begin VB.Menu mFileImport 
         Caption         =   "&Import TXT File..."
      End
      Begin VB.Menu mFile1 
         Caption         =   "-"
      End
      Begin VB.Menu mFileSave 
         Caption         =   "&Save"
         Enabled         =   0   'False
      End
      Begin VB.Menu mFileSaveAs 
         Caption         =   "Save &As..."
      End
      Begin VB.Menu mFile2 
         Caption         =   "-"
      End
      Begin VB.Menu mFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mEdit 
      Caption         =   "&Edit"
      Begin VB.Menu mEditBackground 
         Caption         =   "Change &background Color..."
      End
   End
End
Attribute VB_Name = "fMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'resizing the window

Dim FileName As String
Private Type LinkInfo
    Link As String
    Text As String
    Color As String
    FontSize As String
    FontName As String
    Alignment As String
    Italic As Boolean
    Bold As Boolean
End Type

Dim Text As String
Dim LinkCount As Integer


Private Sub cAdd_Click()
'add a new link
'add a comment <!Link3>
'then add the link:
Dim Link As LinkInfo
With Link
    .Link = "c:\"
    .Alignment = "RIGHT"
    .Text = "New Link"
    .Color = "#FFFFFF"
    .FontName = "Courier New"
    .FontSize = 3
End With

LinkCount = LinkCount + 1


Call AddLink(Link, LinkCount)
Call UpdateList
lList.Selected(LinkCount - 1) = True
Text1.Text = Text
End Sub

Private Sub cEdit_Click()
'call the edit box and get information.
'then delete the old one and then insert the new one.
If LinkCount > 0 And lList.SelCount > 0 Then
    Dim lnum As Integer
    Dim LinkCode As String
    Dim position As Integer
    Dim position2 As Integer
    lnum = lSelected() + 1
'figure out BGC (Background Color)...it's a string in the format "FFFF00" for yellow.
position = InStr(1, Text, "BGCOLOR=", vbTextCompare) + 10
position2 = InStr(position, Text, Chr$(34), vbTextCompare)
    
    
'figure out what the string is for this link; pass it to Edit - it will
'figure out the properties itself.

    position = InStr(1, Text, "<!Link" & Format(lnum, "000"), vbTextCompare)
    'now we have where it starts.
    If lnum = LinkCount Then
        'if it's the last one, tehn find </BODY>
        position2 = InStr(1, Text, "</BODY>", vbTextCompare)
    Else
        'otherwise, just find the next one
        position2 = InStr(1, Text, "<!Link" & Format(lnum + 1, "000"), vbTextCompare)
    End If
    LinkCode = Mid$(Text, position, position2 - position)
    'pass all the values, too.
    fEditLink.tPreview.BackColor = pSample.BackColor
    Call fEditLink.EditInit(lSelected() + 1, LinkCode)
End If
End Sub

Private Sub cInsert_Click()
'add a new link in between two
If lList.SelCount > 0 Then
    Dim lnum As Integer
    Dim Link As LinkInfo
    With Link
        .Link = "c:\"
        .Alignment = "RIGHT"
        .Text = "New Link"
        .Color = "#FFFFFF"
        .FontName = "Courier New"
        .FontSize = 3
    End With
    
    LinkCount = LinkCount + 1
    
    lnum = lSelected()
    Call InsertLink(Link, lSelected())
    Call UpdateList
    lList.Selected(lnum) = True
    Text1 = Text
End If
End Sub

Private Sub cDelete_Click()
Dim lnum As Integer
Dim t1 As Integer
Dim t2 As String
Dim position As Integer
Dim position2 As Integer
Dim Leftstring As String
Dim Rightstring As String
Dim HTMLCode As String

If LinkCount > 0 And lList.SelCount > 0 Then
    'deletes a link from the file.
    lnum = lSelected() + 1 'this is the number that's selected.
    'find the entry first.
    position = InStr(1, Text, "<!Link" & Format(lnum, "000"), vbTextCompare)
    position2 = position
    Leftstring = Left$(Text, position - 1)
    
    'now find either the next link or the </BODY> statement.
    If lnum = LinkCount Then
        'this is the last one.
        position = InStr(position + 1, Text, "</BODY>", vbTextCompare)
        Rightstring = Mid$(Text, position)
    Else
        'find the next link
        position = InStr(position + 1, Text, "<!Link" & Format(lnum + 1, "000"), vbTextCompare)
        Rightstring = Mid$(Text, position)
    End If
    'now we have the two pieces.
    Text = Leftstring & Rightstring
    
    position = position2 - 1
   'now we have to update every link from lnum and higher, subtracting one from them.
    If lnum < LinkCount Then
        For i = 1 To (LinkCount - lnum)
            position = InStr(position + 1, Text, "<!Link", vbTextCompare)
            position = position + 5
            t1 = Val(Mid$(Text, position + 1, 3)) - 1
            t2 = Mid$(Text, position + 1, 3)
            Leftstring = Left$(Text, position)
            Rightstring = Right$(Text, Len(Text) - (position + 3))
            Text = Leftstring & Format(t1, "000") & Rightstring
        Next
    End If

    LinkCount = LinkCount - 1
    Call UpdateList
    If LinkCount > 0 Then
        If LinkCount = lnum - 1 Then
            lList.Selected(lList.ListCount - 1) = True
        Else
            If LinkCount > 0 Then lList.Selected(lnum - 1) = True
        End If
    End If
    Text1.Text = Text
End If
End Sub

Private Sub cUpdate_Click()
If FileName = "" Then
    'if we haven't saved it then save the thing.
    Call mFileSaveAs_Click
End If
If FileName <> "" Then
'if we cancelled
mRegistry.UpdateRegistry (FileName)
End If
End Sub

Private Sub Form_Load()
'first initiate the text
Text = NewText()
pSample.BackColor = RGB(0, 0, 0)
Text1.Text = Text
LinkCount = 0

End Sub

Function NewText() As String
'this function returns a string of a newly opened document.
NewText = "<HTML>" & vbCrLf & _
          "<TITLE>ActiveDesktop Links</TITLE>" & vbCrLf & _
          "<BODY BGCOLOR=" & Chr$(34) & "#000000" & Chr$(34) & ">" & vbCrLf & _
          "</BODY>" & vbCrLf & _
          "</HTML>"
End Function

Private Sub ChangeBGCOLOR(NewColor As String)
'searches Text and replaces the BGColor with the NewColor
Dim position As Integer
Dim Leftstring As String
Dim Rightstring As String

position = InStr(1, Text, "BGCOLOR", vbTextCompare)
position = InStr(position + 1, Text, "=", vbTextCompare)
position = InStr(position + 1, Text, Chr$(34), vbTextCompare)
If position = 0 Then Text1.Text = "Corrupt HTML code)"

Leftstring = Left$(Text, position)
position = InStr(position + 1, Text, Chr$(34), vbTextCompare)
Rightstring = Right$(Text, Len(Text) - position + 1)
Text = ""
Text = Leftstring & NewColor & Rightstring

End Sub
Private Sub AddLink(Link As LinkInfo, LinkNumber As Integer)
Dim position As Integer
Dim Leftstring As String
Dim Rightstring As String
Dim HTMLLinkText As String
Dim Qt As String

Qt = Chr$(34)
'this routine adds a new link at the end of the file.
'so this link will always be inserted before </BODY>.
position = InStr(1, Text, "</BODY", vbTextCompare)
Leftstring = Left$(Text, position - 1)
Rightstring = Right$(Text, Len(Text) - position + 1)

'evaluate what the code will be
HTMLLinkText = Evaluate(LinkNumber, Link)

'and insert it.
Text = Leftstring & HTMLLinkText & Rightstring


End Sub

Private Sub InsertLink(Link As LinkInfo, LinkNumber As Integer)
Dim position As Integer
Dim Leftstring As String
Dim Rightstring As String
Dim HTMLLinkText As String
Dim Qt As String
Dim Linkx As String

Dim t1 As Integer
Dim t2 As String

Qt = Chr$(34)

'linknumber will be the number of the link we will replace, and all following
'links' numbers will be increased by one.

'find where to put the link, first.
'if linknumber is 1, then look for "<BODY>"
'but if it's any more than that, look for "<!Linkx>" where x is LinkNumber-1.
'and then after that, we refine it.  We look for either the next <!Linkx> or
'if LinkNumber=LinkCount, look for "</BODY>"
LinkNumber = LinkNumber + 1
If LinkNumber = 1 Then
    'look for <BODY>
    position = InStr(1, Text, "<BODY", vbTextCompare)
    position = InStr(position + 1, Text, ">", vbTextCompare) + 3
Else
    'look for <!Linkx> where x=LinkNumber.  That will point us to the
    'link right before where we will be adding our link.

    Linkx = Format(LinkNumber, "000")
    position = InStr(1, Text, "<!Link" & Linkx & ">", vbTextCompare)
End If
position = position - 1
'now position points to where to put it.
'figure out what the code will be to insert.
HTMLLinkText = Evaluate(LinkNumber, Link)
Leftstring = Left$(Text, position)
Rightstring = Right$(Text, Len(Text) - position)
Text = Leftstring & HTMLLinkText & Rightstring

'now we have to scan through the rest of it and increment all the !linkX x's
position = position + Len(HTMLLinkText) 'now we're at the end of what we've done.

For i = 1 To (LinkCount - LinkNumber)
    position = InStr(position + 1, Text, "<!Link", vbTextCompare) 'find the next !linkX
    position = position + 5 'now we're pointing to the X (the number)
    t1 = Val(Mid$(Text, position + 1, 3)) + 1
    t2 = Mid$(Text, position + 1, 3)
    Leftstring = Left$(Text, position)
    Rightstring = Right$(Text, Len(Text) - (position + 3))
    Text = Leftstring & Format(t1, "000") & Rightstring
Next

End Sub
Private Function Evaluate(LinkNumber As Integer, Link As LinkInfo)
'takes some parameters and returns the text for the hyperlink in HTML.
Dim Qt As String
Dim Bold1 As String
Dim Bold2 As String
Dim Italic1 As String
Dim Italic2 As String

Qt = Chr$(34)

With Link

If .Bold Then
    Bold1 = "<B>"
    Bold2 = "</B>"
End If
If .Italic Then
    Italic1 = "<I>"
    Italic2 = "</I>"
End If

Evaluate = "<!Link" & Format(LinkNumber, "000") & ">" & vbCrLf & _
               "<P ALIGN=" & Qt & .Alignment & Qt & ">" & _
               "<A HREF=" & Qt & .Link & Qt & ">" & _
               Bold1 & Italic1 & _
               "<FONT FACE=" & Qt & .FontName & Qt & _
               " SIZE=" & .FontSize & " COLOR=" & Qt & .Color & Qt & ">" & vbCrLf & _
               .Text & vbCrLf & "</FONT>" & Bold2 & Italic2 & "</A></P>" & vbCrLf
End With

End Function

Sub UpdateList()
'takes Text and converts it into the list of links in the lList list box
Dim position As Integer
Dim position2 As Integer

'first clear the list
lList.Clear

'now go through and do it.
position = 0
    For i = 1 To LinkCount
        'the beginning of the link name
        position = InStr(position + 1, Text, "<!Link" & Format(i, "000") & ">", vbTextCompare)
        position = InStr(position + 1, Text, "<a href", vbTextCompare)
        position = InStr(position + 1, Text, "<font", vbTextCompare)
        position = InStr(position + 1, Text, ">", vbTextCompare) + 2
        'the end of it.
        position2 = InStr(position + 1, Text, "</font>", vbTextCompare) - 2
        lList.List(i - 1) = Format(i, "000") & ":" & Mid$(Text, position + 1, position2 - position - 1)
    Next

End Sub

Sub Edit(eNumber As Integer, eAlignment As String, eColor As String, _
            eFontName As String, eFontSize As String, _
            eBold As Boolean, eItalic As Boolean, eLink As String, eText As String)
        'this sub is called when
        'we're done with the edit dialog box
Dim Link As LinkInfo
Link.Alignment = eAlignment
Link.Bold = eBold
Link.Color = eColor
Link.FontName = eFontName
Link.FontSize = eFontSize
Link.Italic = eItalic
Link.Link = eLink
Link.Text = eText
'now delete link # enumber, and then insert the new stuff.

'I'll do it the long way
Dim position As Integer
Dim Leftstring As String
Dim Rightstring As String
Dim Insertstring As String

position = InStr(1, Text, "<!Link" & Format(eNumber, "000"), vbTextCompare)
Leftstring = Left$(Text, position - 1)

If eNumber < LinkCount Then
    position = InStr(position + 1, Text, "<!Link" & Format(eNumber + 1, "000"), vbTextCompare)
    Rightstring = Mid$(Text, position)
Else
    position = InStr(position + 1, Text, "</BODY>", vbTextCompare)
    Rightstring = Mid$(Text, position)
End If
Insertstring = Evaluate(eNumber, Link)
Text = Leftstring & Insertstring & Rightstring

Call UpdateList
Text1.Text = Text

End Sub
Function lSelected() As Integer
'returns the number pointing to the list item that's selected.
Dim i As Integer
i = 0
While lList.Selected(i) = False
    i = i + 1
Wend
lSelected = i
End Function

Private Sub mEditBackground_Click()
'Function ConvertColor(Orig As Long) As String
'Ask for a color and then change to it.
Dim NewColor As String

CommonDialog1.Flags = cdCClFullOpen
CommonDialog1.ShowColor
pSample.BackColor = CommonDialog1.Color

NewColor = fEditLink.ConvertColor(CommonDialog1.Color)
ChangeBGCOLOR (NewColor)
Text1.Text = Text
End Sub

Private Sub mFileExit_Click()
Unload fMain
Unload fEditLink

End Sub

Private Sub mFileNew_Click()
'create a new file.
mFileSave.Enabled = False
FileName = ""
lList.Clear
Text = NewText()
Text1.Text = Text
End Sub

Private Sub mFileOpen_Click()
Dim TempText As String
Dim i As Integer
'show common dialog
CommonDialog1.CancelError = True
On Error GoTo CDCancel3
CommonDialog1.Filter = "ADD HTML Documents (*.htm)|*.htm"
CommonDialog1.Flags = cdlOFNHideReadOnly Or cdlOFNLongNames
CommonDialog1.ShowOpen
'now we have the filename
If CommonDialog1.FileName <> "" Then
    Open CommonDialog1.FileName For Binary Access Read As 1
    TempText = Input(LOF(1), 1)
    
    Close 1
    If InStr(1, TempText, "<!Link001>", vbTextCompare) Then
        Text = TempText
        'determine the number of links
        i = 1
        While (InStr(1, Text, "<!Link" & Format(i, "000"), vbTextCompare))
            i = i + 1
        Wend
        LinkCount = i - 1
        
        Call UpdateList
        Text1.Text = Text
    Else
        MsgBox "Not a valid ADD file", vbOKOnly, "Incorrect HTM file"
    End If
    FileName = CommonDialog1.FileName
    fMain.Caption = FileName
    mFileSave.Enabled = True
End If
CDCancel3:
End Sub

Private Sub mFileSave_Click()
If FileName <> "" Then
    'save the file
    Open FileName For Output As 1
        Print #1, Text
    Close 1
End If
End Sub

Private Sub mFileSaveAs_Click()
Dim result As Long

'show the save as commondialog box
CommonDialog1.CancelError = True
On Error GoTo CDCancel2
CommonDialog1.Filter = "ADD HTML Documents (*.htm)|*.htm"
CommonDialog1.Flags = cdlOFNHideReadOnly Or cdlOFNOverwritePrompt Or cdlOFNLongNames
CommonDialog1.ShowSave
If CommonDialog1.FileName <> "" Then
Open CommonDialog1.FileName For Output As 1
    Print #1, Text
Close 1
FileName = CommonDialog1.FileName
    fMain.Caption = FileName
mFileSave.Enabled = True
End If
CDCancel2:
End Sub

