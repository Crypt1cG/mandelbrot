#include <math.h>
#include <iostream>
class ComplexNum
{
public:
    ComplexNum(float a, float b)
    {
        this->a = a;
        this->b = b;
    }
    void add(ComplexNum num)
    {
        a += num.a;
        b += num.b;
    }
    float abs()
    {
        float res = sqrt(a * a + b * b);
        return res;
    }
    ComplexNum square()
    {
        // float new_a = a * a - b * b;
        // float new_b = 2 * a * b;
        // ComplexNum z = ComplexNum(new_a, new_b);
        // return z;
        return ComplexNum(a * a - b * b, 2 * a * b);
    }
    // ComplexNum cube()
    // {
    //     return ComplexNum(a * a * a + b * b * b, -3 * a * a * b + 3 * a * b * b);
    // }

    float a, b;
};

ComplexNum iterate(ComplexNum& z, ComplexNum& c, int itLeft)
{
    if (itLeft == 0 || z.abs() >= 2)
        return z;
    ComplexNum new_z = z.square();
    new_z.a += c.a;
    new_z.b += c.b;
    return iterate(new_z, c, itLeft - 1);
}

int main()
{
    float step = 14; // actual step is 1 / step
    float actualStart = -2;
    float actualEnd = 2;
    float start = actualStart * step;
    float end = actualEnd * step;
    int numIterations = 10;
    int size = end - start + 1;
    bool results[size * size];
    int index = 0;
    ComplexNum z = ComplexNum(0, 0);
    ComplexNum c = ComplexNum(0, 0);
    ComplexNum z_n = ComplexNum(0, 0);
    float res;
    for (float b = start; b <= end; b += 1)
    {
        for (float a = end; a >= start; a -= 1)
        {
            c.a = a / step;
            c.b = b / step;
            z.a = 0;
            z.b = 0;
            z_n = iterate(z, c, numIterations);
            res = z_n.abs();
            if (res >= 2) 
                results[index] = 0;
            else
            {
                results[index] = 1;
            }
            index++;
        }
    }
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (results[i * size + j]) 
            {
                std::cout << "x ";
            }
            else
            {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }

    return 0;
}