@echo off & setlocal EnableDelayedExpansion

set PATH=%VK_SDK_PATH%\bin

for /f "delims=" %%i in ('"dir /a/s/b/on *.vert *.frag"') do (
set file=%%~nxi
glslangValidator -V %%~nxi -o %%~nxi.spv
)

pause