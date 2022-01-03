#include "include/calculations.hpp"
#include <cmath>
#ifdef OPENCL
    #include "include/openclStuff.h"
#endif
#include <vector>

const unsigned int NUM_CPUS = std::thread::hardware_concurrency();
// double numIterations = 30;
// extern "C" double iterate(ComplexNum* z, const ComplexNum* c, int itLeft);

double iterate(ComplexNum& z, const ComplexNum& c, int itLeft)
{
    // if (itLeft == 0) // we have done all iterations and the number is still in the mandelbrot set
    //     return 0;
    // z.square();
    // z += c;
    // // if (new_z.squaredModulus() >= 4) return totalIterations - (itLeft - 1);
    // // if (new_z.abs() >= 1000) return totalIterations - (itLeft - 1);
    // // if (new_z.abs() >= 2) return (totalIterations - itLeft) + 1 - log(log2(new_z.abs()));
    // double R = z.squaredModulus();
    // if (R >= 9) 
    // {
    //     double V = log(R) / pow(2, numIterations - itLeft);
    //     return V;
    // }
    // return iterate(z, c, itLeft - 1);
    int numIterations = itLeft;
    while (itLeft > 0)
    {
        z.square();
        z += c;
        double R = z.squaredModulus();
        if (R >= 9)
        {
            double V = log(R) / pow(2, numIterations - itLeft);
            return V;
        }
        itLeft--;
    }
    return 0;
}

int iterateNotNormalized(ComplexNum& z, const ComplexNum& c, int itLeft, int numIterations)
{
    if (itLeft == 0)
        return 0;
    z.square();
    z += c;
    if (z.squaredModulus() >= 4) return numIterations - (itLeft - 1);
    return iterateNotNormalized(z, c, itLeft - 1, numIterations);
}

double* calculate(const WindowInfo& info)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    double* results = new double[info.pixelWidth * info.pixelHeight];
    int index = 0;

    ComplexNum z = ComplexNum(0, 0);
    ComplexNum c = ComplexNum(0, 0);
    double actualStep = 1.0 / info.step;
    for (double b = info.minB; b <= info.maxB; b += actualStep) // loop from min to max incrementing by step
    {
        for (double a = info.minA; a <= info.maxA; a += actualStep) // loop from min to max incrementing by step
        {
            double sum = 0;
            // super sampling stuff - use 3x3 grid of sub-pixels (centered on the
            // main pixel) and average them to get the value for the pixel - this
            // is super sampling, it helps reduce graininess
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    c.a = a + i * (1 / (info.step * 3.0));
                    c.b = b + j * (1 / (info.step * 3.0));
                    // c.a = a;
                    // c.b = b;
                    // reset z to zero
                    z.a = 0;
                    z.b = 0;

                    // sum += iterateNotNormalized(z, c, numIterations);
                    sum += iterate(z, c, info.numIterations);
                }
            }

            results[index] = sum / 9;
            // c.a = a;
            // c.b = b;
            // z.a = 0;
            // z.b = 0;
            // results[index] = iterate(z, c, info.numIterations);
            index++;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << duration.count() << " milliseconds" << std::endl;
    return results;
}

double* getResults(const WindowInfo& info)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    double* results;
#ifdef OPENCL
    results = cl_getResults(info.minA, info.maxA, info.minB, info.maxB, info.step, info.numIterations);
#else
    if (NUM_CPUS == 2)
    {
        WindowInfo half1 = WindowInfo(info.minA, info.maxA, info.minB, 
                                    info.minB + info.rangeB / 2, info.step,
                                    info.numIterations);
        WindowInfo half2 = WindowInfo(info.minA, info.maxA, info.minB + 
                                    info.rangeB / 2 + 1.0 / info.step, info.maxB, 
                                    info.step, info.numIterations);
        // results = calculate(info, numIterations);
        std::future<double*> ret = std::async(calculate, half1);
        std::future<double*> ret2 = std::async(calculate, half2);
        double* results2 = ret.get();
        results = new double[info.pixelWidth * info.pixelHeight];
        std::copy(results2, results2 + half1.pixelHeight * half1.pixelWidth, results);
        delete [] results2;
        results2 = ret2.get();
        std::copy(results2, results2 + half2.pixelWidth * half2.pixelHeight, results + half1.pixelHeight * half1.pixelWidth);
    }
    else
    {
        results = new double[info.pixelWidth * info.pixelHeight];

        int index = 0;
        for (int i = 0; i < 16 / NUM_CPUS; i++) // do 4 sections of 4 (16 pieces, 4 at a time)
        {
            WindowInfo* pieces = new WindowInfo[NUM_CPUS];
            for (int j = i * NUM_CPUS; j < (i + 1) * NUM_CPUS; j++) // j is actual "piece #"
            {
                WindowInfo piece;
                if (j == 0) // special case, include the first and last of section (1/16 + 1)
                {
                    piece = WindowInfo(info.minA, info.maxA,
                                       info.minB, info.minB + info.rangeB / 16,
                                       info.step, info.numIterations);
                }
                else
                {
                    piece = WindowInfo(info.minA, info.maxA,
                                       info.minB + j * (info.rangeB / 16) + 1.0 / info.step,
                                       info.minB + (j + 1) * (info.rangeB / 16),
                                       info.step, info.numIterations);
                }
                pieces[j % NUM_CPUS] = piece;
            }
            std::vector<std::future<double*>> returns;
            returns.reserve(NUM_CPUS);
            for (int n = 0; n < NUM_CPUS; n++)
            {
                returns.push_back(std::async(calculate, pieces[n]));
            }
            // std::future<double*> ret1 = std::async(calculate, pieces[0], numIterations);
            // std::future<double*> ret2 = std::async(calculate, pieces[1], numIterations);
            // std::future<double*> ret3 = std::async(calculate, pieces[2], numIterations);
            // std::future<double*> ret4 = std::async(calculate, pieces[3], numIterations);

            // array of double arrays (double*), each double* is the results from one of the sections
            double** allResults = new double*[NUM_CPUS];
            for (int n = 0; n < NUM_CPUS; n++)
            {
                allResults[n] = returns[n].get();
            }
            // double* allResults[4] = {ret1.get(), ret2.get(), ret3.get(), ret4.get()};
            for (int k = 0; k < NUM_CPUS; k++)
            {
                double* result = allResults[k];
                WindowInfo inf = pieces[k];
                std::copy(result, result + inf.pixelHeight * inf.pixelWidth,
                          results + index);
                index += inf.pixelWidth * inf.pixelHeight;
                delete [] result;
                // delete [] allResults[k];
            }
            std::cout << std::endl;
            delete [] pieces;
            delete [] allResults;
        }

    //     unsigned int NUM_PIECES = 16;
    //     int index = 0;
    //     WindowInfo pieces[NUM_PIECES] = {};
    //     for (int i = 0; i < NUM_PIECES; i++)
    //     {
    //         WindowInfo piece;
    //             if (i == 0) // special case, include the first and last of section (1/16 + 1)
    //             {
    //                 piece = WindowInfo(info.minA, info.maxA,
    //                                    info.minB, info.minB + info.rangeB / NUM_PIECES,
    //                                    info.step, info.numIterations);
    //             }
    //             else
    //             {
    //                 piece = WindowInfo(info.minA, info.maxA,
    //                                    info.minB + i * (info.rangeB / NUM_PIECES) + 1.0 / info.step,
    //                                    info.minB + (i + 1) * (info.rangeB / NUM_PIECES),
    //                                    info.step, info.numIterations);
    //             }
    //             pieces[i] = piece;
    //     }

    //     std::vector<std::future<double*>> returns;
    //     returns.reserve(NUM_PIECES);
    //     for (int n = 0; n < NUM_PIECES; n++)
    //     {
    //         returns.push_back(std::async(calculate, pieces[n]));
    //     }

    //     double* allResults[NUM_PIECES] = {};
    //     for (int n = 0; n < NUM_PIECES; n++)
    //     {
    //         allResults[n] = returns[n].get();
    //     }
    //     for (int k = 0; k < NUM_PIECES; k++)
    //     {
    //         double* result = allResults[k];
    //         WindowInfo inf = pieces[k];
    //         // std::cout << "got the things " << "height: " << inf.pixelHeight << " width: " << info.pixelWidth << std::endl;
    //         std::copy(result, result + inf.pixelHeight * inf.pixelWidth,
    //                     results + index);
    //         index += inf.pixelWidth * inf.pixelHeight;
    //         delete [] result;
    //         // std::cout << "finished number " << k << std::endl;
    //         // delete [] allResults[k];
    //     }
    }
#endif
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Total: " << duration.count() << " milliseconds" << std::endl;
    return results;
}

std::vector<double> getCoefficients(int n, double z_n)
{
	std::vector<double> coefficients = {1, 0, 0, 0, 0}; 
	for (int i = 0; i < n; i++)
	{
		coefficients[0] = 2 * z_n * coefficients[0] + 1;
	}
}
