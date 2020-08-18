PROG_NAME := voxeltest
run: linux/build/$(PROG_NAME)
	cd linux/build && ./$(PROG_NAME)
build:
	cp -r src/shaders linux/build
	g++ src/*.cpp src/conovox/*.cpp src/*.c -I"include" -O2 -lm -lglfw -lGL -lX11 -lpthread -lXrandr -ldl -o linux/build/$(PROG_NAME)
debug:
	cp -r src/shaders linux/debug
	g++ src/*.cpp src/conovox/*.cpp src/*.c -I"include" -g -lm -lglfw -lGL -lX11 -lpthread -lXrandr -ldl -o linux/debug/$(PROG_NAME)
clean:
	rm linux/build/*
	rm linux/debug/*