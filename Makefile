# LIB_DIR := -L$(HOME)/libraries/glfw/lib-arm64 -L$(HOME)/libraries/glew-2.2.0/build/lib
# INCLUDE_DIR := -I$(HOME)/libraries/glfw/include -I$(HOME)/libraries/glew-2.2.0/include -I$(HOME)/libraries/glm-0.9.9.8 -I$(PWD)
# FRAMEWORKS := -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL
LIB_DIR := D:/glew/lib/Release/x64/glew32s.lib D:/glfw/lib-vc2022/glfw3_mt.lib shell32.lib user32.lib opengl32.lib gdi32.lib
INCLUDE_DIR := /I. /I D:/glfw/include /I D:/glew/include /I D:/glm
DEFINITIONS := /D GLEW_STATIC=1
% : %.cpp opengl_template.cc
	"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
	cl $@.cpp opengl_template.cc /Fe$@  $(DEFINITIONS) $(INCLUDE_DIR) /link $(LIB_DIR)

clean :
	find . -perm +111 -type f -not -path '*/.*' | xargs rm 