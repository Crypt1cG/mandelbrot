CXX = g++
CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx -Ofast -std=c++17 -pthread -o bin/mandelbrot

# @ supresses output from command

all: src/ComplexNum.cpp src/ComplexNum.hpp src/mandelbrotbetter.cpp
	@$(CXX) src/ComplexNum.cpp src/mandelbrotbetter.cpp $(CXXFLAGS)
debug: src/ComplexNum.cpp src/ComplexNum.hpp src/mandelbrotbetter.cpp
	@$(CXX) src/ComplexNum.cpp src/mandelbrotbetter.cpp $(CXXFLAGS) -g
clean:
	@$(RM) bin/mandelbrot