CXX = g++
CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx -std=c++17 -pthread -o bin/mandelbrot
FILES = src/ComplexNum.cpp src/mandelbrot.cpp src/WindowInfo.cpp src/calculations.cpp

# @ supresses output from command

all: $(FILES)
	@$(CXX) $(FILES) $(CXXFLAGS) -Ofast
debug: $(FILES)
	@$(CXX) $(FILES) $(CXXFLAGS) -g
clean:
	@$(RM) bin/mandelbrot