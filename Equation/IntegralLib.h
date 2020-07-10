#include <math.h>
//ѕодынтегральное выражение
double CalculateExpression(double x)
{
    return 2*x+cos(x);
}
/*
—труктура описывающа€€ определенный интеграл
a - нижний предел
b - верхний предел
result - результат
iterations - кол-во итераций дл€ вычислени€ заданной точности
*/
struct DefiniteIntegral
{
    double result;
    int iterations = 0;
    double a;
    double b;
};
/*
—труктура описывающа€ интегральное уравнение
integral - интеграл
q - права€ часть уравнени€
*/
struct Equation
{
    DefiniteIntegral integral;
    double q;
};
/*
Comparator дл€ сравнени€ корней уравнени€
ѕозвол€ет убирать близкие корни, если их разность меньше заданного E
*/
struct RootComparator {
    RootComparator(double eps = 0) :e(eps) {}

    bool operator()(double lhs, double rhs) const {
        return lhs - rhs > e;
    }
private:
    const double e;
};
//‘ункци€ вычислени€ интеграла методом левых пр€моугольников
DefiniteIntegral CalculateIntegral(DefiniteIntegral integral) {
    double x, h;
    double sum = 0.0;
    double fx;
    if (integral.iterations < 1)integral.iterations = 1;
    //¬ычисл€ем шаг разбиени€
    h = (integral.b - integral.a) / integral.iterations;
    DefiniteIntegral result = integral;
    int i = 0;
    //¬ычисл€ем интеграл методом левых пр€моугольников
    for (i = 0; i < integral.iterations; i++) {
        x = integral.a + i * h;
        fx = CalculateExpression(x);
        sum += fx;
    }
    result.result = sum * h;
    return result;
}
//‘ункци€ вычислени€ интеграла с заданной точностью ≈
DefiniteIntegral CalculateIntegralWithPrecision(DefiniteIntegral Integral, double eps)
{
    Integral.iterations = 1;
    unsigned long n = 1;
    double sum1, sum2;
    DefiniteIntegral r1, r2;
    //¬ычисл€ем интеграл в 1 приближении
    r1 = CalculateIntegral(Integral);
    sum1 = r1.result;
    //¬ычисл€ем интеграл с заданной точностью. 
    //”словие остановки - модуль разности двух приближений меньше заданного ≈
    do
    {
        //— каждой итерацией увеличиваем разбиение в 2 раза
        n *= 2;
        r1.iterations = n;
        sum2 = sum1;
        r2 = r1;
        r1 = CalculateIntegral(r1);
        sum1 = r1.result;
    } while (fabs(sum1 - sum2) > eps);
    return r1;
}
//‘ункци€ вычислени€ корн€ c точностью ≈ при заданных приближени€х x0 и x1
double CalculateRoot(Equation eq, double eps, double x0, double x1)
{
    double raz, ss1, temp, xR = x1;
    raz = xR - x1;
    DefiniteIntegral r1, r2;
    r1 = eq.integral;
    r2 = eq.integral;
    r1.b = x1;
    r2.b = x0;
    //¬ычисл€ем значение левой части уравнени€ дл€ 2 начальных приближений корн€
    r1 = CalculateIntegral(r1);
    r2 = CalculateIntegral(r2);
    //Ќаходим приближенное значение корн€ методом секущих
    //”словие остановки - модуль разности текущего и предыдущего корней меньше заданного ≈
    do
    {
        //¬ычисл€ем новое приближение корн€
        xR = xR - (xR - x0) / ((r1.result - eq.q) - (r2.result - eq.q)) * (r1.result - eq.q);
        raz = xR - x1;
        x0 = x1;
        x1 = xR;
        r1.b = x1;
        r2.b = x0;
        //¬ычисл€ем значение левой части уравнени€ дл€ текущего и предыдущего приближений
        r1 = CalculateIntegral(r1);
        r2 = CalculateIntegral(r2);
    } while (fabs(raz) > eps);
    return xR;
}
//‘ункци€ получени€ всех корней уравнени€ на отрезке [a;b] с точностью ≈
void CalculateRoots(DefiniteIntegral integral, double q, double eps, set<double>& result)
{
    set<double, RootComparator> X{ RootComparator { eps } };
    Equation eq;
    eq.integral = integral;
    eq.q = q;
    DefiniteIntegral x0;
    double root;
    //¬ычисл€ем все корни на отрезке [a;b], разбива€ отрезов на равные части
    for (int i = eq.integral.a; i < eq.integral.b; i++)
    {
        root = CalculateRoot(eq, eps, i, i + 1);
        if(root>=eq.integral.a-0.01 && root <= integral.b+0.01) X.insert(root);
    }
    result.insert(X.begin(), X.end());
}