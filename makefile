CXX = g++
CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -Ofast -std=c++17 -pthread -o bin/mandelbrot

# @ supresses output from command

all: src/ComplexNum.cpp src/ComplexNum.hpp src/mandelbrotbetter.cpp
	@$(CXX) src/ComplexNum.cpp src/mandelbrotbetter.cpp $(CXXFLAGS)
clean:
	@$(RM) bin/mandelbrot