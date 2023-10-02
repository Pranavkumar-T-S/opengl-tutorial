@echo off
set LIB_DIR=%HOMEPATH%/libraries
set LIBS="%LIB_DIR%/glew/lib/Release/x64/glew32s.lib" "%LIB_DIR%/glfw/lib-vc2022/glfw3_mt.lib" shell32.lib user32.lib opengl32.lib gdi32.lib 
set INCLUDE_DIR=/I. /I "%LIB_DIR%/glfw/include" /I "%LIB_DIR%/glew/include" /I "%LIB_DIR%/glm"
set DEFINITIONS=/D GLEW_STATIC=1

WHERE cl
IF %ERRORLEVEL% NEQ 0 (
    CALL "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

IF EXIST %1.cpp (
cl %1.cpp opengl_template.cc /Fe%1  %DEFINITIONS% %INCLUDE_DIR% /link %LIBS%
) ELSE (
    echo "File not found"
)