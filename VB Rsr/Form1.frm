VERSION 5.00
Object = "{00028C01-0000-0000-0000-000000000046}#1.0#0"; "DBGRID32.OCX"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.3#0"; "COMCTL32.OCX"
Begin VB.Form MainForm 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Rushmore Shadows Registration v.4"
   ClientHeight    =   6975
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   9600
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   465
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   640
   StartUpPosition =   1  'CenterOwner
   WindowState     =   2  'Maximized
   Begin VB.Frame fGrid 
      BorderStyle     =   0  'None
      Caption         =   "Frame1"
      Height          =   5895
      Left            =   3240
      TabIndex        =   26
      Top             =   6600
      Width           =   9135
      Begin MSDBGrid.DBGrid DBGrid1 
         Bindings        =   "Form1.frx":030A
         Height          =   5655
         Left            =   0
         OleObjectBlob   =   "Form1.frx":031E
         TabIndex        =   27
         TabStop         =   0   'False
         Top             =   240
         Width           =   9135
      End
   End
   Begin VB.Frame fNewEntry 
      BorderStyle     =   0  'None
      Caption         =   "Frame1"
      Height          =   5895
      Left            =   240
      TabIndex        =   25
      Top             =   480
      Width           =   9015
      Begin VB.TextBox tRecord1 
         DataField       =   "RecordNumber"
         DataSource      =   "Data1"
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   14.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   360
         Left            =   2280
         TabIndex        =   34
         TabStop         =   0   'False
         Text            =   "Text3"
         Top             =   2760
         Visible         =   0   'False
         Width           =   1695
      End
      Begin VB.CommandButton cDelete 
         Caption         =   "Delete this Record"
         Height          =   375
         Left            =   6435
         TabIndex        =   33
         TabStop         =   0   'False
         Top             =   5280
         Width           =   2500
      End
      Begin VB.CheckBox chActive 
         Caption         =   "Reservation Active? (Uncheck this to cancel the reservation)"
         DataField       =   "Active"
         DataSource      =   "Data1"
         Height          =   735
         Left            =   5880
         TabIndex        =   32
         TabStop         =   0   'False
         Top             =   4440
         Width           =   2175
      End
      Begin VB.CommandButton cAddNew 
         Caption         =   "New Record"
         Height          =   375
         Left            =   195
         TabIndex        =   31
         TabStop         =   0   'False
         Top             =   5280
         Width           =   2500
      End
      Begin VB.TextBox tNumberInParty 
         DataField       =   "NumberInParty"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   5760
         TabIndex        =   17
         Text            =   "NumberInParty"
         Top             =   2520
         Width           =   1335
      End
      Begin VB.HScrollBar hsScroller 
         Height          =   255
         LargeChange     =   10
         Left            =   6360
         TabIndex        =   28
         TabStop         =   0   'False
         Top             =   360
         Width           =   1695
      End
      Begin VB.TextBox tRecord2 
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   14.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   360
         Left            =   6360
         TabIndex        =   29
         TabStop         =   0   'False
         Text            =   "Text3"
         Top             =   0
         Width           =   1695
      End
      Begin VB.TextBox tDY 
         Height          =   375
         Left            =   7680
         TabIndex        =   23
         Text            =   "dY"
         Top             =   3720
         Width           =   495
      End
      Begin VB.TextBox tDD 
         Height          =   375
         Left            =   7080
         MaxLength       =   2
         TabIndex        =   22
         Text            =   "dD"
         Top             =   3720
         Width           =   495
      End
      Begin VB.TextBox tDM 
         Height          =   375
         Left            =   6480
         MaxLength       =   2
         TabIndex        =   21
         Text            =   "dM"
         Top             =   3720
         Width           =   495
      End
      Begin VB.TextBox tAY 
         Height          =   375
         Left            =   7680
         TabIndex        =   20
         Text            =   "aY"
         Top             =   3240
         Width           =   495
      End
      Begin VB.TextBox tAD 
         Height          =   375
         Left            =   7080
         MaxLength       =   2
         TabIndex        =   19
         Text            =   "aD"
         Top             =   3240
         Width           =   495
      End
      Begin VB.TextBox tAM 
         Height          =   375
         Left            =   6480
         MaxLength       =   2
         TabIndex        =   18
         Text            =   "aM"
         Top             =   3240
         Width           =   495
      End
      Begin VB.TextBox tLength 
         DataField       =   "Length"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   5760
         TabIndex        =   16
         Text            =   "Length"
         Top             =   1920
         Width           =   2895
      End
      Begin VB.TextBox tVehicle 
         DataField       =   "TypeOfVehicle"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   5760
         TabIndex        =   15
         Text            =   "Type Of Vehicle"
         Top             =   1320
         Width           =   2895
      End
      Begin VB.TextBox tSite 
         DataField       =   "Site #"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   5760
         TabIndex        =   14
         Text            =   "Site Number"
         Top             =   840
         Width           =   855
      End
      Begin VB.TextBox tMembership 
         DataField       =   "MembershipNumber"
         DataSource      =   "Data1"
         Height          =   405
         Left            =   1080
         TabIndex        =   13
         Text            =   "Membership Number"
         Top             =   4680
         Width           =   3375
      End
      Begin VB.CheckBox chRPI 
         Caption         =   "RPI"
         DataField       =   "RPI"
         DataSource      =   "Data1"
         Height          =   255
         Left            =   1800
         TabIndex        =   12
         Top             =   4320
         Width           =   1815
      End
      Begin VB.CheckBox chACN 
         Caption         =   "ACN"
         DataField       =   "ACN"
         DataSource      =   "Data1"
         Height          =   255
         Left            =   1800
         TabIndex        =   11
         Top             =   4080
         Width           =   2295
      End
      Begin VB.CheckBox chRSR 
         Caption         =   "RSR"
         DataField       =   "RSR"
         DataSource      =   "Data1"
         Height          =   255
         Left            =   1800
         TabIndex        =   10
         Top             =   3840
         Width           =   1935
      End
      Begin VB.CheckBox chCTC 
         Caption         =   "CTC"
         DataField       =   "CTC"
         DataSource      =   "Data1"
         Height          =   255
         Left            =   1800
         TabIndex        =   9
         Top             =   3600
         Width           =   1695
      End
      Begin VB.TextBox tMemo 
         DataField       =   "Notes"
         DataSource      =   "Data1"
         Height          =   1575
         Left            =   1080
         MultiLine       =   -1  'True
         TabIndex        =   8
         Text            =   "Form1.frx":276D
         Top             =   1800
         Width           =   3255
      End
      Begin VB.TextBox tCountry 
         DataField       =   "Country"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   1080
         TabIndex        =   7
         Text            =   "Country"
         Top             =   1320
         Width           =   1575
      End
      Begin VB.TextBox tZip 
         DataField       =   "Zip"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   3120
         TabIndex        =   6
         Text            =   "Zip"
         Top             =   960
         Width           =   1215
      End
      Begin VB.TextBox tState 
         DataField       =   "State"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   2640
         MaxLength       =   2
         TabIndex        =   5
         Text            =   "State"
         Top             =   960
         Width           =   495
      End
      Begin VB.TextBox tCity 
         DataField       =   "City"
         DataSource      =   "Data1"
         Height          =   375
         Left            =   1080
         TabIndex        =   4
         Text            =   "City"
         Top             =   960
         Width           =   1575
      End
      Begin VB.TextBox tStreet 
         DataField       =   "Street"
         DataSource      =   "Data1"
         Height          =   405
         Left            =   1080
         TabIndex        =   3
         Text            =   "Street"
         Top             =   600
         Width           =   3255
      End
      Begin VB.TextBox tLastName 
         DataField       =   "LastName"
         DataSource      =   "Data1"
         Height          =   405
         Left            =   2640
         TabIndex        =   2
         Text            =   "Last"
         Top             =   240
         Width           =   1695
      End
      Begin VB.TextBox tFirstName 
         DataField       =   "FirstName"
         DataSource      =   "Data1"
         Height          =   405
         Left            =   1080
         TabIndex        =   1
         Text            =   "First"
         Top             =   240
         Width           =   1575
      End
      Begin VB.CommandButton cSubmit 
         Caption         =   "Update/Submit Record"
         Height          =   375
         Left            =   3315
         TabIndex        =   24
         Top             =   5280
         Width           =   2500
      End
      Begin VB.Data Data1 
         Caption         =   "Data1"
         Connect         =   "Access"
         DatabaseName    =   "Rsr.mdb"
         DefaultCursorType=   0  'DefaultCursor
         DefaultType     =   2  'UseODBC
         Exclusive       =   0   'False
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   15.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   450
         Left            =   4560
         Options         =   0
         ReadOnly        =   0   'False
         RecordsetType   =   1  'Dynaset
         RecordSource    =   "MainTable"
         Top             =   240
         Visible         =   0   'False
         Width           =   1500
      End
      Begin VB.Label Label13 
         Caption         =   "Departure Date: MM/DD/YY"
         Height          =   495
         Left            =   5160
         TabIndex        =   47
         Top             =   3720
         Width           =   1215
      End
      Begin VB.Label Label12 
         Caption         =   "Arrival Date: MM/DD/YY"
         Height          =   375
         Left            =   5160
         TabIndex        =   46
         Top             =   3240
         Width           =   1215
      End
      Begin VB.Label Label11 
         Caption         =   "Number in Party"
         Height          =   375
         Left            =   4680
         TabIndex        =   45
         Top             =   2520
         Width           =   855
      End
      Begin VB.Label Label10 
         Caption         =   "Length"
         Height          =   255
         Left            =   4680
         TabIndex        =   44
         Top             =   2040
         Width           =   975
      End
      Begin VB.Label Label9 
         Caption         =   "Type of Vehicle"
         Height          =   375
         Left            =   4680
         TabIndex        =   43
         Top             =   1320
         Width           =   855
      End
      Begin VB.Label Label8 
         Caption         =   "Site #"
         Height          =   255
         Left            =   4680
         TabIndex        =   42
         Top             =   960
         Width           =   975
      End
      Begin VB.Label Label7 
         Caption         =   "Membership Number"
         Height          =   375
         Left            =   0
         TabIndex        =   41
         Top             =   4680
         Width           =   855
      End
      Begin VB.Label Label6 
         Caption         =   "Membership Affiliation"
         Height          =   495
         Left            =   0
         TabIndex        =   40
         Top             =   3840
         Width           =   855
      End
      Begin VB.Label Label5 
         Caption         =   "Rate/Notes"
         Height          =   255
         Left            =   0
         TabIndex        =   39
         Top             =   2400
         Width           =   975
      End
      Begin VB.Label Label4 
         Caption         =   "Country"
         Height          =   255
         Left            =   0
         TabIndex        =   38
         Top             =   1440
         Width           =   975
      End
      Begin VB.Label Label3 
         Caption         =   "City, State, Zip"
         Height          =   255
         Left            =   0
         TabIndex        =   37
         Top             =   1080
         Width           =   1095
      End
      Begin VB.Label Label2 
         Caption         =   "Street"
         Height          =   255
         Left            =   0
         TabIndex        =   36
         Top             =   720
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Name First, Last"
         Height          =   375
         Left            =   0
         TabIndex        =   35
         Top             =   240
         Width           =   1095
      End
   End
   Begin ComctlLib.TabStrip TabStrip1 
      Height          =   6360
      Left            =   120
      TabIndex        =   0
      Tag             =   "0"
      Top             =   120
      Width           =   9360
      _ExtentX        =   16510
      _ExtentY        =   11218
      _Version        =   327682
      BeginProperty Tabs {0713E432-850A-101B-AFC0-4210102A8DA7} 
         NumTabs         =   2
         BeginProperty Tab1 {0713F341-850A-101B-AFC0-4210102A8DA7} 
            Caption         =   "Edit/New"
            Key             =   "Tab1"
            Object.Tag             =   "1"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab2 {0713F341-850A-101B-AFC0-4210102A8DA7} 
            Caption         =   "Grid"
            Key             =   "Tab2"
            Object.Tag             =   "2"
            ImageVarType    =   2
         EndProperty
      EndProperty
   End
   Begin ComctlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   30
      Top             =   6720
      Width           =   9600
      _ExtentX        =   16933
      _ExtentY        =   450
      SimpleText      =   ""
      _Version        =   327682
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   3
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   11536
            Text            =   "Rushmore Shadows Registration Database Program v.4, (c) 1999 Carl Corcoran"
            TextSave        =   "Rushmore Shadows Registration Database Program v.4, (c) 1999 Carl Corcoran"
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Style           =   5
            Alignment       =   1
            AutoSize        =   2
            Object.Width           =   2646
            MinWidth        =   2646
            TextSave        =   "4:51 PM"
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel3 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Style           =   6
            Alignment       =   1
            AutoSize        =   2
            Object.Width           =   2646
            MinWidth        =   2646
            TextSave        =   "3/31/99"
            Object.Tag             =   ""
         EndProperty
      EndProperty
   End
End
Attribute VB_Name = "MainForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub cAddNew_Click()
Data1.UpdateControls

Data1.Recordset.AddNew
chActive.Value = 1
Data1.Recordset.Update

Call SetUpScroller
Data1.Recordset.MoveLast
tFirstName.SetFocus

End Sub

Private Sub cDelete_Click()
'delete the current record and move to the one that followed it.
If Data1.Recordset.RecordCount = 0 Then
    'no records to delete
Else
    Data1.Recordset.Delete
    Data1.Recordset.MoveNext
End If


End Sub

Private Sub cSubmit_Click()
If Data1.Recordset.RecordCount = 0 Then GoTo SubmitExit
Data1.Recordset.Edit

'Get the dates from the text boxes.
Data1.Recordset("Arrival") = GetArrivalDate()
'if nothing was entered for the date, set it to null.
If Data1.Recordset("Arrival") = -1 Then Data1.Recordset("Arrival") = Null

Data1.Recordset("Departure") = GetDepartureDate()
'if nothing was entered for the date, set it to null.
If Data1.Recordset("Departure") = -1 Then Data1.Recordset("Departure") = Null

'make sure that the data in number-type fields isn't going to cause a
'type mismatch
If IsText(tSite.Text) Then
    'it's invalid
    tSite.Text = "0"
End If


Data1.UpdateRecord

'if we're updating the last one in the bunch, add a new one.
If Data1.Recordset.AbsolutePosition = Data1.Recordset.RecordCount - 1 Then
    Call cAddNew_Click
    Data1.Recordset.MoveLast
Else
    Data1.Recordset.MoveNext
End If
SubmitExit:
tFirstName.SetFocus

End Sub

Private Sub Data1_Reposition()
Dim ArrivalDate As String
Dim DepartureDate As String
If Data1.Recordset.RecordCount = 0 Then GoTo RepositionExit

'if we try to move past the EOF then don't let us.
If Data1.Recordset.EOF Then Data1.Recordset.MoveLast
'update teh scroller
hsScroller.Value = Data1.Recordset.AbsolutePosition


'Display the date information correctly
ArrivalDate = Data1.Recordset("Arrival") & ""
If ArrivalDate <> "" Then
    tAD = Day(ArrivalDate)
    tAM = Month(ArrivalDate)
    tAY = Year(ArrivalDate)
Else
    tAD = ""
    tAM = ""
    tAY = ""
End If

DepartureDate = Data1.Recordset("Departure") & ""
If DepartureDate <> "" Then
    tDD = Day(DepartureDate)
    tDM = Month(DepartureDate)
    tDY = Year(DepartureDate)
Else
    tDD = ""
    tDM = ""
    tDY = ""
End If


tRecord2.Text = tRecord1.Text

RepositionExit:
End Sub

Private Sub Data1_Validate(Action As Integer, Save As Integer)
'
Select Case Action
    Case vbDataActionAddNew
        If Data1.Recordset.RecordCount > 0 Then Data1.Recordset.MoveLast
End Select
End Sub

Private Sub DBGrid1_AfterDelete()
Call SetUpScroller
If Data1.Recordset.AbsolutePosition = Data1.Recordset.RecordCount Then
    'the last record was deleted, so we have to move to the last record.
    Data1.Recordset.MoveLast
    
End If
End Sub


Private Sub DBGrid1_HeadClick(ByVal ColIndex As Integer)

'if they click on a header, then sort the thing
Dim TempR1 As Recordset, TempR2 As Recordset
Dim SortStr As String

SortStr = DBGrid1.Columns(ColIndex).DataField

Set TempR1 = Data1.Recordset
TempR1.Sort = SortStr
Set TempR2 = TempR1.OpenRecordset(TempR1.Type)
Set Data1.Recordset = TempR2

'    Set recRecordset1 = Data1.Recordset            'copy the recordset
'    recRecordset1.Sort = SortStr
'
'    'establish the Sort
'    Set recRecordset2 = recRecordset1.OpenRecordset(recRecordset1.Type)
'    Set Data1.Recordset = recRecordset2

End Sub

Private Sub Form_Load()
Call AlignContainers
Call TabStrip1_Click

Data1.Refresh

'if there are no records then create one new one.
If Data1.Recordset.RecordCount = 0 Then
    Data1.Recordset.AddNew
    Data1.Recordset.Update
End If
Data1.Recordset.MoveLast

Call SetUpScroller
hsScroller.Value = hsScroller.Max

End Sub

Private Sub hsScroller_Change()
Data1.Recordset.AbsolutePosition = hsScroller.Value
End Sub

Private Sub TabStrip1_Click()
Select Case TabStrip1.SelectedItem.Index
    Case 2:
        fNewEntry.Visible = False
        fGrid.Visible = True
    Case 1:
        fGrid.Visible = False
        fNewEntry.Visible = True
End Select

End Sub
Private Sub AlignContainers()
fGrid.Top = fNewEntry.Top
fGrid.Left = fNewEntry.Left
End Sub

Private Sub SetUpScroller()
'set up the hscroller
hsScroller.Min = 0
hsScroller.Max = Data1.Recordset.RecordCount - 1
hsScroller.Enabled = True
If Data1.Recordset.RecordCount = 0 Then hsScroller.Enabled = False

End Sub
Function GetArrivalDate() As Date

If tAY <> "" And tAM <> "" And tAD <> "" Then
    GetArrivalDate = DateSerial(Val(tAY), Val(tAM), Val(tAD))
Else
    GetArrivalDate = -1
End If
End Function
Function GetDepartureDate() As Date
If tDY <> "" And tDM <> "" And tDD <> "" Then
    GetDepartureDate = DateSerial(Val(tDY), Val(tDM), Val(tDD))
Else
    GetDepartureDate = -1
End If
End Function

Function NotAtTheEnd() As Boolean
If Data1.Recordset.AbsolutePosition = Data1.Recordset.RecordCount - 1 Then
    NotAtTheEnd = True
Else
    NotAtTheEnd = False
End If

End Function
Function IsText(a$) As Boolean
IsText = True

If Str$(Val(a$)) = " " & a$ Then IsText = False

End Function

Private Sub tAY_GotFocus()
tAY.SelStart = 0
tAY.SelLength = Len(tAY.Text)
End Sub
Private Sub tAM_GotFocus()
tAM.SelStart = 0
tAM.SelLength = Len(tAM.Text)
End Sub
Private Sub tAD_GotFocus()
tAD.SelStart = 0
tAD.SelLength = Len(tAD.Text)
End Sub

Private Sub tDY_GotFocus()
tDY.SelStart = 0
tDY.SelLength = Len(tDY.Text)
End Sub
Private Sub tDM_GotFocus()
tDM.SelStart = 0
tDM.SelLength = Len(tDM.Text)
End Sub
Private Sub tDD_GotFocus()
tDD.SelStart = 0
tDD.SelLength = Len(tDD.Text)
End Sub
Private Sub tFirstName_GotFocus()
tFirstName.SelStart = 0
tFirstName.SelLength = Len(tFirstName.Text)
End Sub
Private Sub tLastName_GotFocus()
tLastName.SelStart = 0
tLastName.SelLength = Len(tLastName.Text)
End Sub
Private Sub tStreet_GotFocus()
tStreet.SelStart = 0
tStreet.SelLength = Len(tStreet.Text)
End Sub
Private Sub tCity_GotFocus()
tCity.SelStart = 0
tCity.SelLength = Len(tCity.Text)
End Sub
Private Sub tState_GotFocus()
tState.SelStart = 0
tState.SelLength = Len(tState.Text)
End Sub
Private Sub tZip_GotFocus()
tZip.SelStart = 0
tZip.SelLength = Len(tZip.Text)
End Sub

Private Sub tRecord2_Change()
'if it's changed, search for that record

End Sub
