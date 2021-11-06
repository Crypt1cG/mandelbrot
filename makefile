CXX = g++
CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx -std=c++17 -pthread -o bin/mandelbrot

# @ supresses output from command

all: src/ComplexNum.cpp src/ComplexNum.hpp src/mandelbrotbetter.cpp
	@$(CXX) src/ComplexNum.cpp src/mandelbrotbetter.cpp $(CXXFLAGS) -Ofast
debug: src/ComplexNum.cpp src/ComplexNum.hpp src/mandelbrotbetter.cpp
	@$(CXX) src/ComplexNum.cpp src/mandelbrotbetter.cpp $(CXXFLAGS) -g
clean:
	@$(RM) bin/mandelbrot