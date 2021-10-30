class ComplexNum
{
public:
    double a, b;
    ComplexNum(double, double);
    void add(ComplexNum);
    double abs();
    void square();
    /**
     * I think this is basically just the abs ^2
     * abs = √a^2 + b^2 = modulus
     * squared modulus = a^2 + b^2
     */
    double squaredModulus();
};