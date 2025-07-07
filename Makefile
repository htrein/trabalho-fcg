./bin/Linux/main: src/main.cpp src/glad.c src/textrendering.cpp include/matrices.h include/utils.h include/dejavufont.h src/collisions.cpp src/object.cpp src/boundingBox.cpp src/stb_image.cpp src/colliders.cpp src/scene_object.hpp
	mkdir -p bin/Linux
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -O1 -o ./bin/Linux/main src/main.cpp src/collisions.cpp src/object.cpp src/colliders.cpp src/glad.c src/stb_image.cpp src/boundingBox.cpp src/textrendering.cpp src/scene_object.hpp src/tiny_obj_loader.cpp ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor

.PHONY: clean run
clean:
	rm -f bin/Linux/main

run: ./bin/Linux/main
	cd bin/Linux && ./main
