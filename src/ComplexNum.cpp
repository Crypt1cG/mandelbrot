#include "include/ComplexNum.hpp"
#include <math.h>

ComplexNum::ComplexNum(double a, double b)
{
    this->a = a;
    this->b = b;
}

void ComplexNum::add(ComplexNum num)
{
    a += num.a;
    b += num.b;
}

double ComplexNum::abs()
{
    double res = sqrt(a * a + b * b);
    return res;
}

void ComplexNum::square()
{
    double old_a = a;
    a = (a + b) * (a - b);
    b = 2 * old_a * b;
    // return ComplexNum((a + b) * (a - b), 2 * a * b);
}

double ComplexNum::squaredModulus()
{
    return a * a + b * b;
}

ComplexNum ComplexNum::operator+=(const ComplexNum& other)
{
    this->a += other.a;
    this->b += other.b;
    return *this;
}
