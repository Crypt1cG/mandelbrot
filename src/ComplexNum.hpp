// #include <gmp.h>

class ComplexNum
{
public:
    double a, b;

    /**
     * Constructor - makes a complex number from the a and b coefficients
     * in the form a + bi
     * 
     * @param a the a coefficient
     * @param b the b coefficient
     */
    ComplexNum(double a, double b);

    /**
     * Adds another complex number to this complex number
     * 
     * @param num the other complex num to add to this one
     */
    void add(ComplexNum num);

    /**
     * returns the absolute value (distance to origin)
     * of the complex number (√(a^2 + b^2))
     * 
     * @return the absolute value
     */
    double abs();

    /**
     * Squares the complex number
     */
    void square();
    
    /**
     * I think this is basically just the abs ^2
     * abs = √a^2 + b^2 = modulus
     * squared modulus = a^2 + b^2
     */
    double squaredModulus();

    /**
     * Adds another complex number to this one
     */
    ComplexNum operator+=(const ComplexNum& other);
};