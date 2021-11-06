#include <boost/numeric/odeint.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
typedef boost::multiprecision::cpp_dec_float_50 float50;
float50 balls()
{
    float50 thing(10.0);
    return thing;
}

int main()
{
    std::cout << "eh" << std::endl;
}