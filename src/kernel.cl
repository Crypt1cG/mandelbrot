// #pragma OPENCL EXTENSION cl_amd_printf : enable
__kernel void iterate(__global double* results, const double minA, const double minB, 
                      const int sizeA, const int sizeB, const int step, int numIterations)
{
    int id = get_global_id(0);
    // results[id] = 1;
    double c_a = minA + fmod((float)id, sizeA) * (1.0 / step);
    double c_b = minB + (id / sizeB) * (1.0 / step);
    double z_a = 0, z_b = 0;
    int initialNumIterations = numIterations;
    while (numIterations > 0)
    {
        // square
        double old_a = z_a;
        z_a = (z_a + z_b) * (z_a - z_b);
        z_b = 2 * old_a * z_b;

        // z += c
        z_a += c_a;
        z_b += c_b;
        double R = z_a * z_a + z_b * z_b; // squared modulus
        if (R >= 9)
        {
            double V = log((float)R) / pow(2.f, initialNumIterations - numIterations); // cast to float bc of compiler problems
            results[id] = V;
            return;
        }
        numIterations--;
    }
    results[id] = 0;
}