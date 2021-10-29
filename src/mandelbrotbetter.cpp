#include <math.h>
#include <graphx.h>
#include <tice.h>
#include <debug.h>
#define NUM_ITERATIONS 12
#define SCALE 1

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

uint8_t iterate(ComplexNum& z, ComplexNum& c, int itLeft, int totalIterations)
{
    if (itLeft == 0)
        return 0;
    ComplexNum new_z = z.square();
    new_z.a += c.a;
    new_z.b += c.b;
    // dbg_sprintf(dbgout, "%f, %f became %f, %f (C: %f, %f)\n", z.a, z.b, new_z.a, new_z.b, c.a, c.b);
    if (new_z.abs() >= 2) return totalIterations - (itLeft - 1);
    return iterate(new_z, c, itLeft - 1, totalIterations);
}

uint8_t* calculate(float startA, float endA, float startB, float endB, int step, int numIterations)
{
    startA *= step;
    endA *= step;

    startB *= step;
    endB *= step;

    int sizeA = endA - startA + 1;
    int sizeB = endB - startB + 1;

    uint8_t* results = new uint8_t[sizeA * sizeB]; // allocate on heap bc stack not big enough
    int index = 0;

    ComplexNum z = ComplexNum(0, 0);
    ComplexNum c = ComplexNum(0, 0);
    uint8_t res;
    for (float b = startB; b <= endB; b += 1)
    {
        for (float a = startA; a <= endA; a += 1)
        {
            c.a = a / step;
            c.b = b / step;
            z.a = 0;
            z.b = 0;
            // dbg_sprintf(dbgout, "C: %f, %f\n", c.a, c.b);
            res = iterate(z, c, numIterations, numIterations);
            // dbg_sprintf(dbgout, "abs: %f, index: %d\n", res, index);
            results[index] = res;
            //     dbg_sprintf(dbgout, "set a 1\n");
            // }
            index++;
        }
    }
    return results;
}
/**
 * remember to call gfx_Begin() before this
 */
void display(uint8_t* results, int sizeA, int sizeB)
{
    // BTW SCREEN IS 320 x 240
    gfx_FillScreen(255);
    int scale = SCALE;
    uint8_t color_map_thing[] = {0, 2, 3, 4, 5, 6, 7, 103, 135, 167, 199, 231, 230, 229, 228, 227, 226};
    int startY = 120 - (sizeB * scale) / 2; // 120 is middle, subtract half of height
    int startX = 160 - (sizeA * scale) / 2; // 160 is middle, subtract half of width

    for (int i = 0; i < sizeB; i++)
    {
        for (int j = 0; j < sizeA; j++)
        {
            gfx_SetColor(color_map_thing[results[i * sizeA + j]]);
            for (int x = 0; x < scale; x++)
            {
                for (int y = 0; y < scale; y++)
                {
                    gfx_SetPixel(startX + j * scale + x, startY + i * scale + y);
                }
            }
            // gfx_SetPixel(startX + j * scale, startY + i * scale);
        }
    }
}

void drawTarget(ComplexNum target, ComplexNum oldTarget, int& prevColor, int step, int originX, int originY)
{
    // cover up old target
    gfx_SetColor(prevColor);
    gfx_SetPixel(originX + oldTarget.a * step, originY + oldTarget.b * step);
    // draw new target
    prevColor = gfx_GetPixel(originX + target.a * step, originY + target.b * step); // save old color
    gfx_SetColor(31); // some random blue
    gfx_SetPixel(originX + target.a * step, originY + target.b * step);
    dbg_sprintf(dbgout, "Target: %f + %fi\n", target.a, target.b);
}

int main()
{
    int step = 64; // actual step is 1 / step

    float actualAStart = -2;
    float actualAEnd = 1;

    float actualBStart = -1.5;
    float actualBEnd = 1.5;
    // int step = 64; // actual step is 1 / step

    // float actualAStart = -1;
    // float actualAEnd = 0;

    // float actualBStart = -1;
    // float actualBEnd = 0;

    float startA = actualAStart * step;
    float endA = actualAEnd * step;

    float startB = actualBStart * step;
    float endB = actualBEnd * step;
    
    int sizeA = endA - startA + 1;
    int sizeB = endB - startB + 1;

    uint8_t* results = calculate(actualAStart, actualAEnd, actualBStart, actualBEnd, step, NUM_ITERATIONS);
    gfx_Begin();
    display(results, sizeA, sizeB);
    delete [] results;

    ComplexNum target = ComplexNum(0, 0);
    ComplexNum prevTarget = ComplexNum(0, 0);
    int prevColor = 0;
    // midx is avg of min and max
    // midx is at half the screen width (160)
    float midX = (actualAStart + actualAEnd) / 2;
    int originX = 160 - midX * step;

    float midY = (actualBStart + actualBEnd) / 2;
    int originY = 120 - midY * step;
    dbg_sprintf(dbgout, "Origin: %d, %d MidX: %f\n", originX, originY, midX);
    while (true)
    {
        
        auto thing = os_GetCSC();
        if (thing == sk_Enter) 
        {
            float oldRangeA = actualAEnd - actualAStart;
            float oldRangeB = actualBEnd - actualBStart;
            float newRangeA = oldRangeA / 3;
            float newRangeB = oldRangeB / 3;
            actualAStart = target.a - newRangeA / 2;
            actualAEnd = target.a + newRangeA / 2;
            actualBStart = target.b - newRangeB / 2;
            actualBEnd = target.b + newRangeB / 2;
            step *= 3;
            float newSizeA = (actualAEnd * step) - (actualAStart * step) + 1;
            float newSizeB = (actualBEnd * step) - (actualBStart * step) + 1;
            dbg_sprintf(dbgout, "Target: %f + %fi\n", target.a, target.b);
            dbg_sprintf(dbgout, "dimensions: b: %f, b: %f\n", newSizeA, newSizeB);
            dbg_sprintf(dbgout, "new A start: %f, new A end: %f, new B start: %f, new B end: %f, new step: %d\n", newAStart, newAEnd, newBStart, newBEnd, (int)(step));
            uint8_t* new_results = calculate(actualAStart, actualAEnd, actualBStart, actualBEnd, step, NUM_ITERATIONS + 5);
            display(new_results, newSizeA, newSizeB);
            delete [] new_results;
        }
        else if (thing == sk_Up)
        {
            // float newStart = -1.0;
            // float newEnd = 0;
            // // float newActualStart = newStart * step;
            // // float newActualEnd = newEnd * step;
            // float newStep = step * 2;
            // int newSize = (newEnd * newStep) - (newStart * newStep) + 1;
            // uint8_t* new_results = calculate(newStart, newEnd, newStart, newEnd, newStep, NUM_ITERATIONS + 5);
            // display(new_results, newSize, newSize);
            // delete[] new_results;
            prevTarget = target;
            target.b -= 1.0 / step;
            drawTarget(target, prevTarget, prevColor, step, originX, originY);
        }
        else if (thing == sk_Down)
        {
            prevTarget = target;
            target.b += 1.0 / step;
            drawTarget(target, prevTarget, prevColor, step, originX, originY);
        }
        else if (thing == sk_Left)
        {
            prevTarget = target;
            target.a -= 1.0 / step;
            drawTarget(target, prevTarget, prevColor, step, originX, originY);
        }
        else if (thing == sk_Right)
        {
            prevTarget = target;
            target.a += 1.0 / step;
            drawTarget(target, prevTarget, prevColor, step, originX, originY);
        }
        else if (thing)
        {
            break;
        }
    }
    gfx_End();
    return 0;
}