@echo off
if exist 1kb.obj del 1kb.obj
if exist 1kb.exe del 1kb.exe

\masm32\bin\ml /c /Zi /coff /nologo 1kb.asm
if errorlevel 1 goto ErrASM

\masm32\bin\Link /DEBUG /ALIGN:4 /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /MERGE:.rdata=.text 1kb.obj
if errorlevel 1 goto ErrLink

goto GoodBuild

:ErrASM
echo Assemble error
goto TheEnd

:ErrLink
echo Link error
goto TheEnd

:GoodBuild
echo Build successful.
if "%1"=="windbg" goto InvokeDebugger
goto TheEnd

:InvokeDebugger
echo Invoking WinDBG...
windbg -WF 1kb.wew 1kb.exe

:TheEnd
