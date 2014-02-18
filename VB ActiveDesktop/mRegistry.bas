Attribute VB_Name = "mRegistry"
'Public Declare Function GetShortPathName Lib _
    "kernel32" (ByVal lpszLongPath As String, ByVal lpszShortPath As String, _
    ByVal cchBuffer As Long) As Long

Private Declare Function SystemParametersInfo Lib "user32" Alias _
   "SystemParametersInfoA" (ByVal uAction As Long, ByVal uParam _
   As Long, ByVal lpvParam As String, ByVal fuWinIni As Long) As Long
Const SPIF_UPDATEINIFILE = &H1
Const SPI_SETDESKWALLPAPER = 20
Const SPIF_SENDWININICHANGE = &H2



Private Declare Function RegCloseKey Lib "advapi32.dll" _
   (ByVal hKey As Long) As Long

Private Declare Function RegCreateKeyEx Lib _
   "advapi32.dll" _
   Alias "RegCreateKeyExA" _
   (ByVal hKey As Long, ByVal lpSubKey As String, _
   ByVal Reserved As Long, ByVal lpClass As String, _
   ByVal dwOptions As Long, ByVal samDesired As Long, _
   lpSecurityAttributes As Any, _
   hKeyHandle As Long, lpdwDisposition As Long) As Long

Private Declare Function RegQueryValueExString Lib _
   "advapi32.dll" _
   Alias "RegQueryValueExA" _
   (ByVal hKey As Long, ByVal lpValueName As String, _
   ByVal lpReserved As Long, lpType As Long, _
   ByVal lpData As String, lpcbData As Long) As Long
   
Private Declare Function RegOpenKeyEx Lib _
   "advapi32.dll" _
   Alias "RegOpenKeyExA" _
   (ByVal hKey As Long, ByVal lpSubKey As String, _
   ByVal ulOptions As Long, ByVal samDesired As Long, _
   hKeyHandle As Long) As Long
   
Private Declare Function RegSetValueEx Lib "advapi32.dll" _
   Alias "RegSetValueExA" _
   (ByVal hKey As Long, ByVal lpValueName As String, _
   ByVal Reserved As Long, ByVal dwType As Long, _
   lpData As Any, ByVal cbData As Long) As Long

Public Const ERROR_SUCCESS = 0&

Public Const HKEY_CLASSES_ROOT = &H80000000
Public Const HKEY_CURRENT_CONFIG = &H80000005
Public Const HKEY_CURRENT_USER = &H80000001
Public Const HKEY_DYN_DATA = &H80000006
Public Const HKEY_LOCAL_MACHINE = &H80000002
Public Const HKEY_USERS = &H80000003

Public Const KEY_CREATE_SUB_KEY = &H4
Public Const KEY_ENUMERATE_SUB_KEYS = &H8
Public Const KEY_QUERY_VALUE = &H1
Public Const KEY_SET_VALUE = &H2
Public Const KEY_NOTIFY = &H10
Public Const KEY_CREATE_LINK = &H20
Public Const REG_OPTION_NON_VOLATILE = 0
Public Const REG_SZ = 1
Public Const STANDARD_RIGHTS_ALL = &H1F0000
Public Const SYNCHRONIZE = &H100000

Private Const BUFFER_LENGTH = 255


Public Const KEY_ALL_ACCESS = ((STANDARD_RIGHTS_ALL Or _
   KEY_QUERY_VALUE Or _
   KEY_SET_VALUE Or _
   KEY_CREATE_SUB_KEY Or _
   KEY_ENUMERATE_SUB_KEYS Or _
   KEY_NOTIFY Or _
   KEY_CREATE_LINK) And (Not SYNCHRONIZE))



Sub UpdateRegistry(HTMFile As String)
Dim x As Long

    SaveRegistryKey _
   "\.default\software\microsoft\internet explorer\desktop\general", _
   "wallpaper", _
   HTMFile, bUsers:=True
   

'x = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0&, HTMFile, SPIF_UPDATEINIFILE Or SPIF_SENDWININICHANGE)
'x = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, HTMFile, SPIF_UPDATEINIFILE Or SPIF_SENDWININICHANGE)
'    If x = 0 Then MsgBox "Error updating"

End Sub
Public Function GetRegistryKey(sKeyName As String, _
   sValue As String, _
   Optional bClasses_Root As Boolean = False, _
   Optional bCurrent_User As Boolean = False, _
   Optional bLocal_Machine As Boolean = False, _
   Optional bUsers As Boolean = False, _
   Optional bCurrent_Config As Boolean = False, _
   Optional bDyn_Data As Boolean = False) As String
   
   Dim sReturnBuffer As String
   Dim iBufLen As Long
   Dim iReturn As Long
   Dim iTree As Long
   Dim hKeyHandle As Long
   
   If bClasses_Root Then
      iTree = HKEY_CLASSES_ROOT
   ElseIf bCurrent_User Then
      iTree = HKEY_CURRENT_USER
   ElseIf bLocal_Machine Then
      iTree = HKEY_LOCAL_MACHINE
   ElseIf bUsers Then
      iTree = HKEY_USERS
   ElseIf bCurrent_Config Then
      iTree = HKEY_CURRENT_CONFIG
   ElseIf bDyn_Data Then
      iTree = HKEY_DYN_DATA
   Else
      GetRegistryKey = ""
      Exit Function
   End If
   '
   ' Set up return buffer
   '
   sReturnBuffer = Space(BUFFER_LENGTH)
   iBufLen = BUFFER_LENGTH
   
   iReturn = RegOpenKeyEx(iTree, sKeyName, _
      0, KEY_ALL_ACCESS, hKeyHandle)
   
   If iReturn = ERROR_SUCCESS Then
      iReturn = RegQueryValueExString(hKeyHandle, _
         sValue, 0, 0, sReturnBuffer, iBufLen)
      If iReturn = ERROR_SUCCESS Then
         GetRegistryKey = Left$(sReturnBuffer, _
            iBufLen - 1)
      Else
         GetRegistryKey = ""
      End If
   Else
      GetRegistryKey = ""
   End If
   RegCloseKey hKeyHandle
   
End Function


Public Sub SaveRegistryKey(sKey As String, _
   sValueName As String, sNewValue As String, _
   Optional bClasses_Root As Boolean = False, _
   Optional bCurrent_User As Boolean = False, _
   Optional bLocal_Machine As Boolean = False, _
   Optional bUsers As Boolean = False, _
   Optional bCurrent_Config As Boolean = False, _
   Optional bDyn_Data As Boolean = False)

   Dim sKeyName As String
   Dim iReturn As Long
   Dim iTree As Long
   Dim hKeyHandle As Long
   
   If bClasses_Root Then
      iTree = HKEY_CLASSES_ROOT
   ElseIf bCurrent_User Then
      iTree = HKEY_CURRENT_USER
   ElseIf bLocal_Machine Then
      iTree = HKEY_LOCAL_MACHINE
   ElseIf bUsers Then
      iTree = HKEY_USERS
   ElseIf bCurrent_Config Then
      iTree = HKEY_CURRENT_CONFIG
   ElseIf bDyn_Data Then
      iTree = HKEY_DYN_DATA
   Else
      Exit Sub
   End If
   
   '
   ' RegCreateKeyEx will open the named key if it exists,
   ' and create a new one if it doesn't.
   '
   iReturn = RegCreateKeyEx(iTree, sKey, 0&, _
      vbNullString, REG_OPTION_NON_VOLATILE, _
      KEY_ALL_ACCESS, 0&, hKeyHandle, iReturn)
   
   '
   ' RegSetValueEx saves the value to the string within
   ' the key that was just opened.
   '
   iReturn = RegSetValueEx(hKeyHandle, sValueName, 0&, _
      REG_SZ, ByVal sNewValue, Len(sNewValue))
   
   '
   ' Close the key
   '
   RegCloseKey hKeyHandle
   
End Sub


