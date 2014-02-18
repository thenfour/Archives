; Some definitions :)

LoadLibraryA PROTO :DWORD
LoadLibrary equ <LoadLibraryA>
GetProcAddress PROTO :DWORD,:DWORD

TheWidth    equ <400>
TheHeight   equ <300>
NUMBALLS    equ <10>
Y_AXIS      equ <0>
X_AXIS      equ <1>
Z_AXIS      equ <2>

WS_POPUP      equ <80000000h>
CW_USEDEFAULT equ <80000000h>
PM_REMOVE     equ <1h>
VK_ESCAPE     equ <1bh>

WM_CREATE   equ <1h>
WM_DESTROY  equ <2h>
WM_CLOSE    equ <10h>
WM_KEYDOWN  equ <100h>
WM_QUIT     equ <12h>

POINT STRUCT
  x  DWORD ?
  y  DWORD ?
POINT ENDS

MSG STRUCT
  hwnd      DWORD      ?
  message   DWORD      ?
  wParam    DWORD      ?
  lParam    DWORD      ?
  time      DWORD      ?
  pt        POINT      <>
MSG ENDS

WNDCLASSEX STRUCT
  cbSize            DWORD      ?
  style             DWORD      ?
  lpfnWndProc       DWORD      ?
  cbClsExtra        DWORD      ?
  cbWndExtra        DWORD      ?
  hInstance         DWORD      ?
  hIcon             DWORD      ?
  hCursor           DWORD      ?
  hbrBackground     DWORD      ?
  lpszMenuName      DWORD      ?
  lpszClassName     DWORD      ?
  hIconSm           DWORD      ?
WNDCLASSEX ENDS

; Used DirectDraw structs definitions
DDPIXELFORMAT STRUCT
    dwSize               DWORD ?
    dwFlags              DWORD ?
    dwFourCC             DWORD ?
    dwRGBBitCount        DWORD ?
    dwRBitMask           DWORD ?
    dwGBitMask           DWORD ?
    dwBBitMask           DWORD ?
    dwRGBAlphaBitMask    DWORD ?
DDPIXELFORMAT ENDS

DDSCAPS2 STRUCT
    dwCaps  DWORD ?
    dwCaps2 DWORD ?
    dwCaps3 DWORD ?
    dwCaps4 DWORD ? 
DDSCAPS2 ENDS

DDCOLORKEY STRUCT
    dwColorSpaceLowValue   DWORD ?
    dwColorSpaceHighValue  DWORD ?
DDCOLORKEY ENDS

DDSURFACEDESC2 STRUCT
    dwSize              DWORD ?
    dwFlags             DWORD ?
    dwHeight            DWORD ?
    dwWidth             DWORD ?
    lPitch              DWORD ?
    dwBackBufferCount   DWORD ?
    dwRefreshRate       DWORD ?
    dwAlphaBitDepth     DWORD ?
    dwReserved          DWORD ?
    lpSurface           DWORD ?
    ddckCKDestOverlay   DDCOLORKEY <>
    ddckCKDestBlt       DDCOLORKEY <>
    ddckCKSrcOverlay    DDCOLORKEY <>
    ddckCKSrcBlt        DDCOLORKEY <>
    ddpfPixelFormat     DDPIXELFORMAT <>
    ddsCaps             DWORD ?
;    dwTextureStage      DWORD ?
DDSURFACEDESC2 ENDS

PALETTEENTRY STRUCT
    peRed     BYTE ?
    peGreen   BYTE ?
    peBlue    BYTE ?
    peFlags   BYTE ?
PALETTEENTRY ENDS

VERTEX STRUCT
    x   REAL4 ?
    y   REAL4 ?
    z   REAL4 ?
VERTEX ENDS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; DirectDraw vtable offsets
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IUnknownRelease                 equ <8h>

DDrawCreateSurface              equ <18h>
DDrawCreatePalette              equ <14h>
DDrawSetCooperativeLevel        equ <50h>
DDrawSetDisplayMode             equ <54h>
DDrawCreateSurface              equ <18h>
DDrawCreatePalette              equ <14h>
DDrawRestoreDisplayMode         equ <4ch>

DDrawSurfaceGetAttachedSurface  equ <30h>
DDrawSurfaceSetPalette          equ <7ch>
DDrawSurfaceLock                equ <64h>
DDrawSurfaceUnlock              equ <80h>
DDrawSurfaceFlip                equ <2ch>

DDrawPaletteSetEntries          equ <18h>

FINAL_DDSCL                     equ <53h>
FINAL_DDESC_FLAGS               equ <21h>
FINAL_DDESC_DDSCAPS             equ <4218h>
FINAL_DDLOCK                    equ <1h>
FINAL_DDFLIP                    equ <1h>
FINAL_DDPCAPS                   equ <44h>
DDSCAPS_BACKBUFFER              equ <4h>
