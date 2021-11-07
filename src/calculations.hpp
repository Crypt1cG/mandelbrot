#include "ComplexNum.hpp"
#include "WindowInfo.hpp"
#include <chrono>
#include <math.h>
#include <thread>
#include <future>
#pragma once

extern double numIterations;
extern unsigned int NUM_CPUS;

double iterate(ComplexNum& z, const ComplexNum& c, int itLeft);

int iterateNotNormalized(ComplexNum&z, const ComplexNum& c, int itLeft);

double* calculate(const WindowInfo& info, int numIterations);

double* getResults(const WindowInfo& info);