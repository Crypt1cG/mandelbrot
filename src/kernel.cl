// #pragma OPENCL EXTENSION cl_amd_printf : enable
__kernel void iterate(__global double* results, const double minA, const double minB, 
                      const int sizeA, const int sizeB, const unsigned long step, int numIterations)
{
    int id = get_global_id(0);
    // results[id] = 1;
    double c_a = minA + id % sizeA * (1.0 / step);
    double c_b = minB + (id / sizeB) * (1.0 / step);
    double z_a = 0, z_b = 0;
    unsigned long initialNumIterations = numIterations;
    // printf("id: %d, sizeA: %d, sizeB: %d, minA: %f, c: %f + %fi, z: %f + %fi, numIterations: %d\n", id, sizeA, sizeB, minA, c_a, c_b, z_a, z_b, numIterations);
    int period = 0;
    double z_a_old = 0;
    double z_b_old = 0;
    while (numIterations > 0)
    {
        // square
        double old_a = z_a;
        z_a = (z_a + z_b) * (z_a - z_b);
        z_b = (old_a + old_a) * z_b;

        // z += c
        z_a += c_a;
        z_b += c_b;
        double R = z_a * z_a + z_b * z_b; // squared modulus
        if (R >= 9)
        {
            double V = log((double)R) / pow((double)2.0, initialNumIterations - numIterations); // cast to float bc of compiler problems
            results[id] = V;
            return;
        }
        numIterations--;
        // if (z_a == z_a_old && z_b == z_b_old)
        // {
        //     results[id] = 0;
        //     return;
        // }
        // period++;
        // if (period > 20)
        // {
        //     period = 0;
        //     z_a_old = z_a;
        //     z_b_old = z_b;
        // }
    }
    results[id] = 0;
}