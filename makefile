CXX = g++
CXXFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgmp -lgmpxx -std=c++17 -pthread -o bin/mandelbrot

# https://stackoverflow.com/questions/714100/os-detecting-makefile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CXXFLAGS += -lOpenCL
endif
ifeq ($(UNAME_S),Darwin)
	CXXFLAGS += -framework OpenCl
endif

FILES = src/ComplexNum.cpp src/mandelbrot.cpp src/WindowInfo.cpp src/calculations.cpp

# @ supresses output from command

all: $(FILES)
	@$(CXX) $(FILES) $(CXXFLAGS) -Ofast
debug: $(FILES)
	@$(CXX) $(FILES) $(CXXFLAGS) -g
clean:
	@$(RM) bin/mandelbrot