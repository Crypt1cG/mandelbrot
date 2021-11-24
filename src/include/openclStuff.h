#pragma once

void setup();

double* cl_getResults(const double minA, const double maxA,
                      const double minB, const double maxB,
                      const uint64_t step, const int numIterations);