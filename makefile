CXX = g++
# CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -pthread -o bin/mandelbrot `wx-config --libs` `wx-config --cxxflags`
CXXFLAGS = -std=c++17 -pthread #`wx-config --libs` `wx-config --cxxflags`
WXFLAGS = `wx-config --libs` `wx-config --cxxflags`

# https://stackoverflow.com/questions/714100/os-detecting-makefile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OPENCL_FLAGS = -lOpenCL
	WXTHING = wx-config-gtk3
endif
ifeq ($(UNAME_S),Darwin)
	OPENCL_FLAGS = -framework OpenCl
	WXTHING = wx-config
endif

FILES = src/ComplexNum.cpp src/mandelbrot.cpp src/WindowInfo.cpp src/calculations.cpp
OBJECTS = bin/ComplexNum.o bin/WindowInfo.o bin/calculations.o bin/mandelbrot.o

# @ supresses output from command
all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) `$(WXTHING) --libs` -Ofast -o bin/mandelbrot

bin/ComplexNum.o: src/ComplexNum.cpp
	$(CXX) $^ $(CXXFLAGS) -c -o bin/ComplexNum.o

bin/WindowInfo.o: src/WindowInfo.cpp
	$(CXX) $^ $(CXXFLAGS) -c -o bin/WindowInfo.o

bin/mandelbrot.o: src/mandelbrot.cpp
	$(CXX) $^ $(CXXFLAGS) `$(WXTHING) --cxxflags` -c -o bin/mandelbrot.o

bin/calculations.o: src/calculations.cpp
	$(CXX) $^ $(CXXFLAGS) -c -o bin/calculations.o

bin/openclStuff.o: src/openclStuff.c
	$(CXX) $^ $(CXXFLAGS) -c -o bin/openclStuff.o 

opencl: bin/ComplexNum.o bin/WindowInfo.o bin/openclStuff.o
	$(CXX) $^ $(CXXFLAGS) $(OPENCL_FLAGS) `$(WXTHING) --cxxflags` `$(WXTHING) --libs` src/mandelbrot.cpp src/calculations.cpp -DOPENCL -Ofast -o bin/mandelbrot

debug: $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(WXFLAGS) -g -o bin/mandelbrot

clean:
	$(RM) bin/mandelbrot $(OBJECTS) bin/openclStuff.o
