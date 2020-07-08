#include <math.h>
double CalculateExpression(double x)
{
    return 2 * x - cos(x);
}

struct DefiniteIntegral
{
    double result;
    int iterations;
    double a;
    double b;
};

struct Equation
{
    DefiniteIntegral integral;
    double q;
};

DefiniteIntegral CalculateIntegral(DefiniteIntegral integral) {
    double x, h;
    double sum = 0.0;
    double fx;
    h = (integral.b - integral.a) / integral.iterations;
    DefiniteIntegral result = integral;
    int i = 0;
    for (i = 0; i < integral.iterations; i++) {
        x = integral.a + i * h;
        fx = CalculateExpression(x);
        sum += fx;
    }
    result.result = sum * h;
    return result;
}

struct RootComparator {
    RootComparator(double eps = 0) :e(eps) {}

    bool operator()(double lhs, double rhs) const {
        return lhs - rhs > e;
    }
private:
    const double e;
};

double CalculateRoot(Equation eq, double eps, double x0, double x1)
{
    double raz, ss1, temp, xR = x1;
    raz = xR - x1;
    DefiniteIntegral r1, r2;
    r1 = eq.integral;
    r2 = eq.integral;
    r1.b = x1;
    r2.b = x0;
    r1 = CalculateIntegral(r1);
    r2 = CalculateIntegral(r2);
    do
    {
        xR = xR - (xR - x0) / ((r1.result - eq.q) - (r2.result - eq.q)) * (r1.result - eq.q);
        raz = xR - x1;
        x0 = x1;
        x1 = xR;
        r1.b = x1;
        r2.b = x0;
        r1 = CalculateIntegral(r1);
        r2 = CalculateIntegral(r2);
    } while (fabs(raz) > eps);
    return xR;
}