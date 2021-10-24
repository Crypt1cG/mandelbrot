#include <math.h>
#include <graphx.h>
#include <tice.h>
#include <debug.h>

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
    if (itLeft == 0)
        return z;
    ComplexNum new_z = z.square();
    new_z.a += c.a;
    new_z.b += c.b;
    dbg_sprintf(dbgout, "%f, %f became %f, %f (C: %f, %f)\n", z.a, z.b, new_z.a, new_z.b, c.a, c.b);
    if (new_z.abs() >= 2) return new_z;
    return iterate(new_z, c, itLeft - 1);
}

int main()
{
    dbg_sprintf(dbgout, "started program");
    int step = 64; // actual step is 1 / step

    float actualAStart = -2;
    float actualAEnd = 1;

    float actualBStart = -1.5;
    float actualBEnd = 1.5;

    float startA = actualAStart * step;
    float endA = actualAEnd * step;

    float startB = actualBStart * step;
    float endB = actualBEnd * step;
    
    int sizeA = endA - startA + 1;
    int sizeB = endB - startB + 1;

    bool* results = new bool[sizeA * sizeB]; // allocate on heap bc stack not big enough
    int numIterations = 12;
    int index = 0;

    ComplexNum z = ComplexNum(0, 0);
    ComplexNum c = ComplexNum(0, 0);
    ComplexNum z_n = ComplexNum(0, 0);
    float res;
    for (float b = startB; b <= endB; b += 1)
    {
        for (float a = startA; a <= endA; a += 1)
        {
            c.a = a / step;
            c.b = b / step;
            z.a = 0;
            z.b = 0;
            // dbg_sprintf(dbgout, "C: %f, %f\n", c.a, c.b);
            z_n = iterate(z, c, numIterations);
            res = z_n.abs();
            dbg_sprintf(dbgout, "abs: %f, index: %d\n", res, index);
            if (res >= 2)
                results[index] = 0;
            else
                results[index] = 1;
            //     dbg_sprintf(dbgout, "set a 1\n");
            // }
            index++;
        }
    }
    dbg_sprintf(dbgout, "finished calculating stuff\n");
    gfx_Begin();
    gfx_FillScreen(255);
    gfx_SetColor(7);
    // BTW SCREEN IS 320 x 240
    int scale = 1;
    int startY = 120 - (sizeB * scale) / 2; // 120 is middle, subtract half of height
    int startX = 160 - (sizeA * scale) / 2; // 160 is middle, subtract half of width

    for (int i = 0; i < sizeB; i++)
    {
        for (int j = 0; j < sizeA; j++)
        {
            if (results[i * sizeA + j]) 
            {
                gfx_SetColor(7);
                for (int x = 0; x < scale; x++)
                {
                    for (int y = 0; y < scale; y++)
                    {
                        gfx_SetPixel(startX + j * scale + x, startY + i * scale + y);
                    }
                }
                // gfx_SetPixel(startX + j * scale, startY + i * scale);
            }
            // else is helpful for debugging sometimes
            // else
            // {
            //     gfx_SetColor(0);
            //     gfx_SetPixel(startX + j * scale, startY + i * scale);
            // }
        }
    }

    while (!os_GetCSC());
    gfx_End();
    delete [] results;
    return 0;
}