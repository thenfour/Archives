;
; 1kb vector balls
;
; by magey
;

.386
.model flat, stdcall
option casemap :none

include 1kb.inc
includelib D:\masm32\lib\kernel32.lib


.code   
start:

    ; Load user32.dll into our address space
    push offset szUser32
    call LoadLibrary
    ; Store the module handle in ebp
    mov ebp, eax
    
    ; Populate USER32.DLL function pointers
    mov esi, offset FunctionTable
    mov edi, offset szFunctionNames
    xor ebx, ebx             ; We're gonna use ebx as a counter because it doesn't get changed by GetProcAddress
InitLoop: 
    push edi                 ; Name of function to retrieve
    push ebp                 ; Module handle of user32.dll
    call GetProcAddress
    mov [esi + ebx * 4], eax ; Store the address in the fp table
    xor eax, eax
    repne scasb              ; Find the next string in the function names table
    inc ebx
    cmp bl, NumberOfFunctions
    js InitLoop

    mov al, 1
    shl eax, 31  ; for WS_POPUP/CW_USEDEFAULT
    xor ecx, ecx ; for push 0

    push ecx ; for the ShowCursor call later, save one byte :)
    ; Push the paramters for CreateWindowEx
    push ecx
    push ecx
    push ecx
    push ecx
    push TheHeight
    push TheWidth
    push ecx
    push eax ; CW_USEDEFAULT (80000000h)
    push eax ; WS_POPUP (80000000h)
    push ecx
    push offset szEDIT
    push ecx
    ; Create the window
    call DWORD PTR [esi + CreateWindowEx]
    mov edi, eax ; was hMainWnd, removed to save some bytes

    call DWORD PTR [esi + ShowCursor]
    
    ; Load ddraw.dll into our address space
    push offset szDDraw
    call LoadLibrary

    mov esi, offset lpDD
     
    ; Create the DirectDraw object
    ; Get the address of the function
    push offset szDirectDrawCreate
    push eax
    call GetProcAddress
    xor ecx, ecx
    ; And call it (eax contains the address)
    push ecx
    push esi
    push ecx
    call eax

    ; Give esi the value of lpDD
    mov esi, [esi]
    
    ; Call SetCooperativeLevel, to set all the attributes of our ddraw app
    push FINAL_DDSCL ; DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN
    push edi ; Window handle from CreateWindowEx
    mov edx, [esi]
    push esi
    call DWORD PTR [edx + DDrawSetCooperativeLevel]

    ; Set display mode to 400x300
    push eax
    push eax
    push 8 ; 8 bits
    push TheHeight
    push TheWidth
    mov edx, [esi]
    push esi
    call DWORD PTR [edx + DDrawSetDisplayMode]

    ; We're gonna use ebp from now on with relative displacements, so beware
    mov ebp, offset DDSurfaceDesc

    lea ebx, [ebp - 12] ; offset lpDDPrimary
    ; Populate the struct with our values
    mov BYTE PTR [ebp], size DDSurfaceDesc       ; DDSurfaceDesc.dwSize
    mov BYTE PTR [ebp + 4], FINAL_DDESC_FLAGS    ; DDSurfaceDesc.dwFlags
    mov WORD PTR [ebp + 104], FINAL_DDESC_DDSCAPS ; DDSurfaceDesc.ddsCaps
    mov BYTE PTR [ebp + 20], 1 ; DDSurfaceDesc.dwBackBufferCount
    ; Create the primary surface
    push eax
    push ebx ; offset lpDDPrimary
    push ebp ; offset DDSurfaceDesc
    mov edx, [esi]
    push esi
    call DWORD PTR [edx + DDrawCreateSurface]

    ; Get the backbuffer surface
    lea eax, [ebp + size DDSURFACEDESC2] ; offset DDSCaps
    lea ebx, [ebp - 8] ; offset lpDDBackBuffer
    
    mov BYTE PTR [eax], DDSCAPS_BACKBUFFER ; DDSCaps.dwCaps
    push ebx ; offset lpDDBackbuffer
    push eax ; offset DDSCaps
    mov edx, [ebp - 12] ; lpDDPrimary
    push edx
    mov edx, [edx]
    call DWORD PTR [edx + DDrawSurfaceGetAttachedSurface]

    lea ebx, [ebp - 4]  ; offset lpDDPalette
    ; Create the palette
    push eax
    push ebx ; offset lpDDPalette
    push offset BallPal
    push FINAL_DDPCAPS
    mov edx, [esi] ; Remember esi contains [lpDD]
    push esi
    call DWORD PTR [edx + DDrawCreatePalette]
    
    ; Set the palette on the primary surface
    push [ebx] ; lpDDPalette
    mov edx, [ebp - 12] ; lpDDPrimary
    push edx
    mov edx, [edx]
    call DWORD PTR [edx + DDrawSurfaceSetPalette]

MessageLoopStart:
    lea esi, [ebp - 28] ; FunctionTable
    
    xor ecx, ecx
    push PM_REMOVE
    push ecx
    push ecx
    push ecx
    push esi
    sub DWORD PTR [esp], 28 ; Msg
    call DWORD PTR [esi + PeekMessage]

    cmp BYTE PTR [ebp - 16 - 12 - 20], VK_ESCAPE  ; Msg.wParam
    je MessageLoopEnd

RenderMain:
    ; Lock the surface
    xor eax, eax
    push eax
    push FINAL_DDLOCK
    push ebp
    push eax
    mov eax, [ebp - 8] ; lpDDBackBuffer
    mov edx, [eax]
    push eax
    call DWORD PTR [edx + DDrawSurfaceLock]

    ; Clear buffer
    mov edi, [ebp + 36] ; Put our locked surface memory pointer into edi (DDSurfaceDesc.lpSurface)
    push edi            ; save it for later use
    push TheHeight / 4
    pop ecx
    imul ecx, DWORD PTR [ebp + 16] ; DDSurfaceDesc.lPitch
    rep stosd ; eax is alrady zero from the return value of lpDDBackBuffer->Lock()

    ; Restore the surface pointer to the initial position
    pop edi

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;;
    ;; Rotate, project, and draw the balls
    ;;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    add ebp, size DDSURFACEDESC2 ; We do this so we can have displacement
                                 ; in the +/-127 range, so we can save space
    
    ; Increase the angle
    fld REAL4 PTR [ebp + size DDSCAPS2 + 12] ; RotationAngle
    fadd AngleStep
    fstp REAL4 PTR [ebp + size DDSCAPS2 + 12] ; RotationAngle

    mov ebx, offset LENSDIST
    lea esi, [ebp + size DDSCAPS2 + 20]
    
    xor ecx, ecx
RotationLoop:
    ; Init ball values
    mov edx, [ebx + 14]
    mov [esi], edx
    mov edx, [ebx + 18]
    mov [esi + 4], edx
    xor edx, edx               ; equals 0, was REAL4 PTR BallZ, removed to save those 4 bytes
    mov [esi + 8], edx         ; edx is zero now, and we are going to take advantage of that :)
    
    ; Adjust the angle
    mov eax, ecx
    imul eax, 10
    mov [ebp + size DDSCAPS2 + 8], eax        ;AngleFactor
    fld REAL4 PTR [ebp + size DDSCAPS2 + 12]  ;RotationAngle
    fiadd DWORD PTR [ebp + size DDSCAPS2 + 8] ;AngleFactor
    fstp REAL4 PTR [ebp + size DDSCAPS2 + 16] ;IncAngle

    xor eax, eax
RotateAxis:
    call RotateBall
    inc eax
    cmp al, 3
    jne RotateAxis

    ; Calculate projected positions of X and Y
    fild WORD PTR [ebx]
    fld st(0)
    fsub REAL4 PTR [esi + 8]

    ; Calculate X
    fld REAL4 PTR [esi]
    fmul st(0), st(2)
    fdiv st(0), st(1)
    fistp DWORD PTR [ebp + size DDSCAPS2]

    ; Calculate Y
    fld REAL4 PTR [esi + 4]
    fmul st(0), st(2)
    fdiv st(0), st(1)
    fistp DWORD PTR [ebp + size DDSCAPS2 + 4]

    ; Clean FPU stack
    ffree st(0)
    ffree st(1)

    add DWORD PTR [ebp + size DDSCAPS2], TheWidth / 2
    mov al, TheHeight / 2
    sub eax, [ebp + size DDSCAPS2 + 4]
    mov [ebp + size DDSCAPS2 + 4], eax

    call DrawBall
    
    inc ecx
    cmp cl, NUMBALLS
    jne RotationLoop

    ; Unlock the surface
    push edx ; remember edx is zero? :)
    mov eax, [ebp - size DDSURFACEDESC2 - 8] ; lpDDBackbuffer
    mov edx, [eax]
    push eax
    call DWORD PTR [edx + DDrawSurfaceUnlock]
    
    ; Flip the backbuffer into the primary surface
    push FINAL_DDFLIP
    push eax
    mov eax, [ebp - size DDSURFACEDESC2 - 12] ; lpDDPrimary
    mov edx, [eax]
    push eax
    call DWORD PTR [edx + DDrawSurfaceFlip]

    sub ebp, size DDSURFACEDESC2 ; return the pointer to it's original state
    jmp MessageLoopStart
    
MessageLoopEnd:
    ret


;
; Rotate the ball struct around the specified axis.
;
; Paramter is the axis to rotate around (X_AXIS / Y_AXIS / Z_AXIS)
;
; Some equotes for local variables
APiSin    equ <REAL4 PTR [ebp - 4]>
APiCos    equ <REAL4 PTR [ebp - 8]>

RotateBall PROC
    enter 8, 0
    pushad
    
    cmp al, X_AXIS
    js RotateY
    je RotateX

    ; esi contains: offset TheBall
RotateZ:
    lea eax, [esi]     ; TheBall.x
    lea edx, [esi + 4] ; TheBall.y
    jmp DoRotation
RotateX:
    lea eax, [esi + 4] ; TheBall.y
    lea edx, [esi + 8] ; TheBall.z
    jmp DoRotation
RotateY:
    lea eax, [esi + 8] ; TheBall.z
    lea edx, [esi]     ; TheBall.x

    ;
    ; Our formula is this
    ;
    ; NewComponent1 = (OldComponent1 * Cos(Angle * PI/180 * Cos(Factor))) - (OldComponent2 * Sin(Angle * PI/180 / Sin(Factor)))
    ; NewComponent2 = (OldComponent2 * Cos(Angle * PI/180 / Sin(Factor))) + (OldComponent1 * Sin(Angle * PI/180 * Cos(Factor)))    
    ;
DoRotation:
    mov edi, offset ANGLECOS

    fld REAL4 PTR [edx]
    fld REAL4 PTR [eax]

    ; Calculate 1st component, and save common used values
    fld REAL4 PTR [edi]      ; ANGLECOS
    fmul REAL4 PTR [esi - 4]     ; IncAngle
    fst APiCos
    fcos
    fmul st(0), st(1)

    fld REAL4 PTR [edi + 4]  ; ANGLESIN
    fmul REAL4 PTR [esi - 4]     ; IncAngle
    fst APiSin
    fsin
    fmul st(0), st(3)
    
    fsubp st(1), st(0)

    ; Calculate 2nd component, use saved values from before
    fld APiSin
    fcos
    fmul st(0), st(3)

    fld APiCos
    fsin
    fmul st(0), st(3)
    
    faddp st(1), st(0)
    
    fstp REAL4 PTR [edx]
    fstp REAL4 PTR [eax]

EndRotation:
    ; Clean FPU stack
    ffree st(0)
    ffree st(1)

    popad
    leave
    ret
RotateBall ENDP

;
; Draw the ball sprite
;
DrawBall PROC
    pushad

    mov esi, offset PackedBall
    
    mov eax, [ebp + size DDSCAPS2 + 4] ; dereference to get pY
    add eax, 9
    mul DWORD PTR [ebp - 92]           ; DSurfaceDesc.lPitch
    add eax, [ebp + size DDSCAPS2]     ; dereference to get pX
    sub eax, 9
    add edi, eax

    ; again edx is zero, so no need to initialize it
    
    mov cl, 108
    push ecx
DrawLoop:
    lodsw
    mov ebx, eax
    xor ecx, ecx
    
PixelLoop:
    mov eax, ebx
    imul ecx, 5
    shr eax, cl
    xchg eax, ecx
    div BYTE PTR [PackedBall + 85] ; this equals 5
    xchg eax, ecx
    and al, 1fh
    test al, al
    jz DontDraw
    stosb
    jmp FinishDraw

DontDraw:
    inc edi

FinishDraw:
    inc edx
    inc ecx
    cmp cl, 3
    jne PixelLoop
    cmp dl, 18
    jne NoNewLine
    add edi, [ebp - 92] ; DSurfaceDesc.lPitch
    sub edi, edx
    xor edx, edx
    
NoNewLine:
    pop ebx
    dec ebx
    push ebx
    jnz DrawLoop

    pop ebx
    popad
    ret
DrawBall ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    szUser32           db "USER32", 0
    szEDIT             db "EDIT", 0
    
    szFunctionNames    db "PeekMessageA", 0
                       db "ShowCursor", 0
                       db "CreateWindowExA", 0
    
    szDDraw            db "DDRAW", 0
    szDirectDrawCreate db "DirectDrawCreate", 0

    LENSDIST           WORD 256
    ANGLECOS           REAL4 -0.007263132371
    ANGLESIN           REAL4 0.0191942621
    AngleStep          REAL4 1.5
    BallX              REAL4 80.0
    BallY              REAL4 20.0

    include blueball.inc

    ; Window message loop stuff
    Msg                 MSG <>

    NumberOfFunctions   equ <3>
    
    PeekMessage         equ <0>
    ShowCursor          equ <4>
    CreateWindowEx      equ <8>
  
    FunctionTable       DWORD NumberOfFunctions dup(?)
    
    ; Pointers to DirectDraw COM objects    
    lpDD               DWORD ?
    lpDDPrimary        DWORD ?
    lpDDBackbuffer     DWORD ?
    lpDDPalette        DWORD ?

    DDSurfaceDesc   DDSURFACEDESC2 <>
    DDSCaps         DDSCAPS2 <>

    pX               DWORD ?
    pY               DWORD ?
    AngleFactor      DWORD ?
    RotationAngle    REAL4 ?
    IncAngle         REAL4 ?
    
    TheBall          VERTEX<>

end start
