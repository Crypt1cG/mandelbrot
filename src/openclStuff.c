#ifdef __linux__
    #include <CL/opencl.h>
#elif __MACH__ // for mac os
    #include <OpenCL/opencl.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "include/openclStuff.h"

static cl_context context;
static cl_command_queue queue;
static cl_program program;
static cl_kernel kern;

void setup()
{
    cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * 3);
    clGetPlatformIDs(3, platforms, NULL);
    char* info = (char*)malloc(sizeof(char) * 50);
    char* version = (char*)malloc(sizeof(char) * 50);
    clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME, 50, info, NULL);
    clGetPlatformInfo(platforms[0], CL_PLATFORM_VERSION, 50, version, NULL);
    printf("%s (%s)\n", info, version);
    
    cl_device_id* devices = (cl_device_id*)malloc(sizeof(cl_device_id) * 2);
    clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 2, devices, NULL);
    //! IMPORTANT - CHANGE THE ARRAY INDEX TO CHANGE DEVICE
    cl_device_id device = devices[0];

    char* deviceName = (char*)malloc(sizeof(char) * 50);
    cl_uint* numComputeUnits = (cl_uint*)malloc(sizeof(cl_uint));
    cl_uint* clockSpeed = (cl_uint*)malloc(sizeof(cl_uint));
    size_t* workItemsPerGroup = (size_t*)malloc(sizeof(size_t));
    size_t* maxWorkItemSizes = (size_t*)malloc(sizeof(size_t) * 3);

    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), numComputeUnits, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), clockSpeed, NULL);
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(char) * 50, deviceName, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), workItemsPerGroup, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, maxWorkItemSizes, NULL);

    printf("Device Name: %s\n# of Compute Units: %u\n# Work Items per Group: %u\nMax"
           "Clock Speed %uMhz\nMax Work Item Sizes: %u x %u x %u\n",
            deviceName, *numComputeUnits, *workItemsPerGroup, *clockSpeed,
            maxWorkItemSizes[0], maxWorkItemSizes[1], maxWorkItemSizes[2]);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
#ifdef __linux__
    queue = clCreateCommandQueueWithProperties(context, device, 0, NULL);
#elif __MACH__
    queue = clCreateCommandQueue(context, device, 0, NULL);
#endif

    // open and read opencl kernel file
    FILE* fp = fopen("src/kernel.cl", "r");
    char* source = (char*)malloc(2000);
    size_t sourceSize = fread(source, 1, 2000, fp);
    fclose(fp);

    cl_int err = 0;
    program = clCreateProgramWithSource(context, 1, (const char**)&source, &sourceSize, &err);
    assert(err == CL_SUCCESS);

    // const char* options = "-cl-std=CL3.0";
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err == CL_INVALID_PROGRAM)
        printf("program is not a valid program object\n");
    else if (err == CL_INVALID_VALUE)
        printf("device_list is NULL and num_devices is greater than 0, or device_list is not NULL and num_devices is 0\n");
    else if (err == CL_INVALID_DEVICE)
        printf("any device in device_list is not in the list of devices associated with program\n");
    else if (err == CL_INVALID_BUILD_OPTIONS)
        printf("build options specified are invalid\n");
    else if (err == CL_COMPILER_NOT_AVAILABLE)
        printf("Compiler not available\n");
    else if (err == CL_BUILD_PROGRAM_FAILURE)
    {
        printf("build failure\n");
        //https://stackoverflow.com/questions/9464190/error-code-11-what-are-all-possible-reasons-of-getting-error-cl-build-prog
        // determine size of log
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        // allocate memory for the log
        char* log = (char*)malloc(log_size);

        // get the log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("%s\n", log);
    }
    else if (err == CL_INVALID_OPERATION)
        printf("idk invalid operation ig\n");
    else if (err == CL_OUT_OF_RESOURCES)
        printf("out of resources\n");
    else if (err == CL_OUT_OF_HOST_MEMORY)
        printf("out of host memory\n");
    else if (err == CL_INVALID_BUILD_OPTIONS)
        printf("invalid build options\n");
    
    // assert(err == CL_SUCCESS);

    // err = clCompileProgram(program, 1, &device, NULL, 0, NULL, NULL, NULL, NULL);
    // assert(err == CL_SUCCESS);

    // program = clLinkProgram(context, 1, &device, NULL, 1, &program, NULL, NULL, &err);
    // assert(err == CL_SUCCESS);

    kern = clCreateKernel(program, "iterate", &err);
    assert(err == CL_SUCCESS);
    printf("finished setup\n");
}

double* cl_getResults(const double minA, const double maxA,
                      const double minB, const double maxB,
                      const uint64_t step, const int numIterations)
{
    int sizeA = (maxA - minA) * step + 1;
    int sizeB = (maxB - minB) * step + 1;
    printf("size a: %d, size b: %d\n", sizeA, sizeB);
    int totalSize = sizeA * sizeB;
    printf("minA: %.17f, minB: %f, step: %llu, totalSize: %d\n", minA, minB, step, totalSize);

    double* results = (double*)malloc(sizeof(double) * totalSize);
    cl_mem d_results = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL);

    // create arrays with a and b values for a given index (don't have to calculate in kernel)
    //NOTE: this probably sucks, should prob just do calculation in kernel
    // double* a_vals = (double*)malloc(totalSize);
    // double* b_vals = (double*)malloc(totalSize);
    // for (int i = 0; i < totalSize; i++)
    // {
    //     a_vals[i] = minA + (i % sizeA) * (1.0 / step);
    //     b_vals[i] = minB + (i / sizeA) * (1.0 / step);
 
    // }
    // // create device versions of these arrays
    // cl_mem d_a_vals = clCreateBuffer(queue, CL_MEM_READ_ONLY, sizeof(double) * totalSize, NULL, NULL);
    // cl_mem d_b_vals = clCreateBuffer(queue, CL_MEM_READ_ONLY, sizeof(double) * totalSize, NULL, NULL);
    double* results_arr[9] = {(double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize),
                              (double*)malloc(sizeof(double) * totalSize)};
    cl_mem d_results_arr[9] = {clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL),
                           clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(double) * totalSize, NULL, NULL)};

    cl_int err;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            double new_minA = minA + i * (1.0 / (step * 3.0));
            double new_minB = minB + j * (1.0 / (step * 3.0));
            err = clSetKernelArg(kern, 0, sizeof(cl_mem), &d_results_arr[(i + 1) * 3 + j + 1]);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 1, sizeof(double), &new_minA);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 2, sizeof(double), &new_minB);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 3, sizeof(int), &sizeA);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 4, sizeof(int), &sizeB);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 5, sizeof(uint64_t), &step);
            assert(err == CL_SUCCESS);
            err = clSetKernelArg(kern, 6, sizeof(int), &numIterations);
            assert(err == CL_SUCCESS);

            size_t global_work_size = totalSize - 1; // ok so i think this is the total # of work items
            size_t local_work_size = 128; // and I think this is how many work items per work group
            err = clEnqueueNDRangeKernel(queue, kern, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

            err = clFinish(queue);
            assert(err == CL_SUCCESS);
            clEnqueueReadBuffer(queue, d_results_arr[(i + 1) * 3 + j + 1], CL_TRUE, 0, sizeof(double) * totalSize,
                                results_arr[(i + 1) * 3 + j + 1], 0, NULL, NULL);
        }
    }

    // // set args for kernel
    // float f_minA = (float)minA; // used for devices with no double precision support
    // float f_minB = (float)minB; // likewise
    // err = clSetKernelArg(kern, 0, sizeof(cl_mem), &d_results);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 1, sizeof(double), &minA);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 2, sizeof(double), &minB);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 3, sizeof(int), &sizeA);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 4, sizeof(int), &sizeB);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 5, sizeof(uint64_t), &step);
    // assert(err == CL_SUCCESS);
    // err = clSetKernelArg(kern, 6, sizeof(int), &numIterations);
    // assert(err == CL_SUCCESS);
    
    // size_t global_work_size = totalSize - 1; // ok so i think this is the total # of work items
    // size_t local_work_size = 128; // and I think this is how many work items per work group
    // err = clEnqueueNDRangeKernel(queue, kern, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
    // if (err == CL_INVALID_WORK_GROUP_SIZE)
    // {
    //     printf("invalid work group size\n");
    //     printf("global size: %d local size: %d\n", global_work_size, local_work_size);
    // }
    // else if (err == CL_INVALID_WORK_DIMENSION)
    //     printf("work_dim is not valid\n");
    // else if (err == CL_INVALID_GLOBAL_WORK_SIZE)
    //     printf("global_work_size is not valid\n");
    // else if (err == CL_INVALID_WORK_ITEM_SIZE)
    //     printf("the number of work-items specified by in any of local_work_size[0],"
    //            "... local_work_size[work_dim - 1] is greated than the corresponding"
    //            "values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], ..., "
    //            "CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim - 1]\n");
    // else if (err == CL_INVALID_KERNEL_ARGS)
    //     printf("invalid kernel args\n");
    // // printf("%d\n", err);
    
    // assert(err == CL_SUCCESS);
    // err = clFinish(queue);
    // assert(err == CL_SUCCESS);
    // clEnqueueReadBuffer(queue, d_results, CL_TRUE, 0, sizeof(double) * totalSize, results, 0, NULL, NULL);
    for (int i = 0; i < totalSize; i++)
    {
        double sum = 0;
        for (int j = 0; j < 9; j++)
        {
            sum += results_arr[j][i];
        }
        results[i] = sum / 9;
    }
    return results;
}