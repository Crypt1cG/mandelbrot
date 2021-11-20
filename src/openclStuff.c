#include <OpenCL/opencl.h>
#include <stdio.h>
#include <include/openclStuff.h>

static cl_context context;
static cl_command_queue queue;

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
    cl_device_id device = devices[1];

    char* deviceName = (char*)malloc(sizeof(char) * 50);
    cl_uint* numComputeUnits = (cl_uint*)malloc(sizeof(cl_uint));
    cl_uint* clockSpeed = (cl_uint*)malloc(sizeof(cl_uint));
    size_t* workItemsPerGroup = (size_t*)malloc(sizeof(size_t));

    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), numComputeUnits, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), clockSpeed, NULL);
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(char) * 50, deviceName, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), workItemsPerGroup, NULL);

    printf("Device Name: %s\n# of Compute Units: %u\n# Work Items per Group: %u\nMax Clock Speed %uMhz\n",
            deviceName, *numComputeUnits, *workItemsPerGroup, *clockSpeed);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);
}