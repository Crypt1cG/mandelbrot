__kernel void iterate(__global double* results, __global const double* a_vals, __global const double* b_vals, int numIterations)
{
    int id = get_global_id(0);
    double c_a = a_vals[id];
    double c_b = a_vals[id];
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
    }
    results[id] = 0;
}