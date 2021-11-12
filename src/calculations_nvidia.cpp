#include "include/calculations.hpp"

unsigned int NUM_CPUS;
double numIterations = 30;

double iterate(ComplexNum& z, const ComplexNum& c, int itLeft);

int iterateNotNormalized(ComplexNum&z, const ComplexNum& c, int itLeft);

double* calculate(const WindowInfo& info, int numIterations);

double* getResults(const WindowInfo& info);