#include <math.h>
//��������������� ���������
double CalculateExpression(double x)
{
    return 2*x+cos(x);
}
/*
��������� ������������ ������������ ��������
a - ������ ������
b - ������� ������
result - ���������
iterations - ���-�� �������� ��� ���������� �������� ��������
*/
struct DefiniteIntegral
{
    double result;
    int iterations = 0;
    double a;
    double b;
};
/*
��������� ����������� ������������ ���������
integral - ��������
q - ������ ����� ���������
*/
struct Equation
{
    DefiniteIntegral integral;
    double q;
};
/*
Comparator ��� ��������� ������ ���������
��������� ������� ������� �����, ���� �� �������� ������ ��������� E
*/
struct RootComparator {
    RootComparator(double eps = 0) :e(eps) {}

    bool operator()(double lhs, double rhs) const {
        return lhs - rhs > e;
    }
private:
    const double e;
};
//������� ���������� ��������� ������� ����� ���������������
DefiniteIntegral CalculateIntegral(DefiniteIntegral integral) {
    double x, h;
    double sum = 0.0;
    double fx;
    if (integral.iterations < 1)integral.iterations = 1;
    //��������� ��� ���������
    h = (integral.b - integral.a) / integral.iterations;
    DefiniteIntegral result = integral;
    int i = 0;
    //��������� �������� ������� ����� ���������������
    for (i = 0; i < integral.iterations; i++) {
        x = integral.a + i * h;
        fx = CalculateExpression(x);
        sum += fx;
    }
    result.result = sum * h;
    return result;
}
//������� ���������� ��������� � �������� ��������� �
DefiniteIntegral CalculateIntegralWithPrecision(DefiniteIntegral Integral, double eps)
{
    Integral.iterations = 1;
    unsigned long n = 1;
    double sum1, sum2;
    DefiniteIntegral r1, r2;
    //��������� �������� � 1 �����������
    r1 = CalculateIntegral(Integral);
    sum1 = r1.result;
    //��������� �������� � �������� ���������. 
    //������� ��������� - ������ �������� ���� ����������� ������ ��������� �
    do
    {
        //� ������ ��������� ����������� ��������� � 2 ����
        n *= 2;
        r1.iterations = n;
        sum2 = sum1;
        r2 = r1;
        r1 = CalculateIntegral(r1);
        sum1 = r1.result;
    } while (fabs(sum1 - sum2) > eps);
    return r1;
}
//������� ���������� ����� c ��������� � ��� �������� ������������ x0 � x1
double CalculateRoot(Equation eq, double eps, double x0, double x1)
{
    double raz, ss1, temp, xR = x1;
    raz = xR - x1;
    DefiniteIntegral r1, r2;
    r1 = eq.integral;
    r2 = eq.integral;
    r1.b = x1;
    r2.b = x0;
    //��������� �������� ����� ����� ��������� ��� 2 ��������� ����������� �����
    r1 = CalculateIntegral(r1);
    r2 = CalculateIntegral(r2);
    //������� ������������ �������� ����� ������� �������
    //������� ��������� - ������ �������� �������� � ����������� ������ ������ ��������� �
    do
    {
        //��������� ����� ����������� �����
        xR = xR - (xR - x0) / ((r1.result - eq.q) - (r2.result - eq.q)) * (r1.result - eq.q);
        raz = xR - x1;
        x0 = x1;
        x1 = xR;
        r1.b = x1;
        r2.b = x0;
        //��������� �������� ����� ����� ��������� ��� �������� � ����������� �����������
        r1 = CalculateIntegral(r1);
        r2 = CalculateIntegral(r2);
    } while (fabs(raz) > eps);
    return xR;
}
//������� ��������� ���� ������ ��������� �� ������� [a;b] � ��������� �
void CalculateRoots(DefiniteIntegral integral, double q, double eps, set<double>& result)
{
    set<double, RootComparator> X{ RootComparator { eps } };
    Equation eq;
    eq.integral = integral;
    eq.q = q;
    DefiniteIntegral x0;
    double root;
    //��������� ��� ����� �� ������� [a;b], �������� ������� �� ������ �����
    for (int i = eq.integral.a; i < eq.integral.b; i++)
    {
        root = CalculateRoot(eq, eps, i, i + 1);
        if(root>=eq.integral.a-0.01 && root <= integral.b+0.01) X.insert(root);
    }
    result.insert(X.begin(), X.end());
}