#include "ComplexNum.hpp"
#include "WindowInfo.hpp"
#include <chrono>
#include <math.h>
#include <thread>
#include <future>
#pragma once

extern const unsigned int NUM_CPUS;

// double iterate(ComplexNum& z, const ComplexNum& c, int itLeft);

//int iterateNotNormalized(ComplexNum&z, const ComplexNum& c, int itLeft);

double* calculate(const WindowInfo& info);

double* getResults(const WindowInfo& info);