#include "include/WindowInfo.hpp"

WindowInfo::WindowInfo(double minA, double maxA, double minB, double maxB, uint64_t step, int numIterations)
{
    this->minA = minA;
    this->maxA = maxA;
    this->minB = minB;
    this->maxB = maxB;
    this->step = step;
    this->numIterations = numIterations;

    rangeA = maxA - minA;
    rangeB = maxB - minB;
    pixelWidth = rangeA * step + 1;
    pixelHeight = rangeB * step + 1;
}

WindowInfo::WindowInfo() : WindowInfo(0, 0, 0, 0, 0, 0) {};

void WindowInfo::zoom(double zoomFactor, ComplexNum target, double& magnification)
{
    if (zoomFactor > 1)
        numIterations *= zoomFactor / 2;
    else numIterations *= zoomFactor * 2;
    rangeA = rangeA / zoomFactor;
    rangeB = rangeB / zoomFactor;

    minA = target.a - rangeA / 2;
    maxA = target.a + rangeA / 2;
    minB = target.b - rangeB / 2;
    maxB = target.b + rangeB / 2;

    step *= zoomFactor;

    pixelWidth = rangeA * step + 1;
    pixelHeight = rangeB * step + 1;
    magnification *= zoomFactor;
}

std::ostream& operator<<(std::ostream& os, const WindowInfo& wi)
{
    std::cout.precision(17);
    os << "\t min\t\t| max\n\t----------------+------\nA:\t "
        << wi.minA << "\t\t| " << wi.maxA << "\t(range: " << wi.rangeA << ")"
        << "\nB:\t " << wi.minB << "\t\t| " << wi.maxB << "\t(range: " << wi.rangeB << ")"
        << "\nStep: " << wi.step << "\nWidth (pixels): " << wi.pixelWidth
        << "\nHeight (pixels): " << wi.pixelHeight;
    return os;
}
