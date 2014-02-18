@echo off

if exist 1kb.obj del 1kb.obj
if exist 1kb.exe del 1kb.exe

\masm32\bin\ml /c /coff /Fl1kb.lst /nologo 1kb.asm
\masm32\bin\Link /ALIGN:4 /SUBSYSTEM:WINDOWS /MERGE:.rdata=.text 1kb.obj

dir 1kb.*

pause
