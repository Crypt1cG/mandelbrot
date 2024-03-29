// #include <cstdint>
#include "ComplexNum.hpp"
#include <iostream>
#pragma once

struct WindowInfo
{
    uint64_t step;
    double minA, maxA;
    double minB, maxB;
    double rangeA, rangeB;
    int pixelWidth, pixelHeight, numIterations;
    WindowInfo(double minA, double maxA, double minB, double maxB, uint64_t step, int numIterations);
    WindowInfo();
    void zoom(double zoomFactor, ComplexNum target, double& magnification);
    friend std::ostream& operator<<(std::ostream& os, const WindowInfo& wi);
};
