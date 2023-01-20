LIB_DIR := -L$(HOME)/libraries/glfw/lib-arm64 -L$(HOME)/libraries/glew-2.2.0/build/lib
INCLUDE_DIR := -I$(HOME)/libraries/glfw/include -I$(HOME)/libraries/glew-2.2.0/include
FRAMEWORKS := -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL
DEFINITIONS := -D GLEW_STATIC
% : %.cpp
	g++ $@.cpp $(INCLUDE_DIR) $(LIB_DIR) -lglew -lglfw3 $(FRAMEWORKS) $(DEFINITIONS) -o $@ 

clean :
	find . -perm +111 -type f -not -path '*/.*' | xargs rm 
