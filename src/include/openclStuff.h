#pragma once

void setup();

double* cl_getResults(const double minA, const double maxA,
                      const double minB, const double maxB,
                      const int step, const int numIterations);