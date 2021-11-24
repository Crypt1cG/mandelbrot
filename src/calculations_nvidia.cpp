#include "include/calculations.hpp"
#include <CL/opencl.hpp>
#include <fstream>

unsigned int NUM_CPUS;
double numIterations = 30;

double iterate(ComplexNum& z, const ComplexNum& c, int itLeft);

int iterateNotNormalized(ComplexNum&z, const ComplexNum& c, int itLeft);

double* calculate(const WindowInfo& info, int numIterations);

double* getResults(const WindowInfo& info);

void setup()
{
    std::vector<cl::Platform> allPlatforms;
    cl::Platform::get(&allPlatforms);
    if (allPlatforms.size() == 0)
    {
        std::cout << "No OpenCL platforms found. Check OpenCL installation." << std::endl;
        exit(1);
    }
    cl::Platform defaultPlatform = allPlatforms[0];
    std::cout << "Using platofrm: " << defaultPlatform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    cl::Context context(CL_DEVICE_TYPE_DEFAULT);

    std::ifstream in("src/kernels.cl");

}