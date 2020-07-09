#include <math.h>
//Функция y=f(t)
double CalculateExpression(double x)
{
    return 2*x-cos(x);
}
/*
Структура описывающаяя определенный интеграл
a - нижний предел
b - верхний предел
result - результат
iterations - кол-во итераций для вычисления заданной точности
*/
struct DefiniteIntegral
{
    double result;
    int iterations;
    double a;
    double b;
};
/*
Структура описывающая интегральное уравнение
integral - интеграл
q - правая часть уравнения
*/
struct Equation
{
    DefiniteIntegral integral;
    double q;
};
/*
Comparator для сравнения корней уравнения
Позволяет убирать близкие корни, если их разность меньше заданного E
*/
struct RootComparator {
    RootComparator(double eps = 0) :e(eps) {}

    bool operator()(double lhs, double rhs) const {
        return lhs - rhs > e;
    }
private:
    const double e;
};
//Функция вычисления интеграла методом левых прямоугольников
DefiniteIntegral CalculateIntegral(DefiniteIntegral integral) {
    double x, h;
    double sum = 0.0;
    double fx;
    if (integral.iterations < 1)integral.iterations = 1;
    //Вычисляем шаг разбиения
    h = (integral.b - integral.a) / integral.iterations;
    DefiniteIntegral result = integral;
    int i = 0;
    //Вычисляем интеграл методом левых прямоугольников
    for (i = 0; i < integral.iterations; i++) {
        x = integral.a + i * h;
        fx = CalculateExpression(x);
        sum += fx;
    }
    result.result = sum * h;
    return result;
}
//Функция вычисления интеграла с заданной точностью Е
DefiniteIntegral CalculateIntegralWithPrecision(DefiniteIntegral Integral, double eps)
{
    Integral.iterations = 1;
    unsigned long n = 1;
    double sum1, sum2;
    DefiniteIntegral r1, r2;
    //Вычисляем интеграл в 1 приближении
    r1 = CalculateIntegral(Integral);
    sum1 = r1.result;
    //Вычисляем интеграл с заданной точностью 
    //Условие остановки - модуль разности двух приближений меньше заданного Е
    do
    {
        //С каждой итерацией увеличиваем разбиение в 2 раза
        n *= 2;
        r1.iterations = n;
        sum2 = sum1;
        r2 = r1;
        r1 = CalculateIntegral(r1);
        sum1 = r1.result;
    } while (fabs(sum1 - sum2) > eps);
    return r1;
}
//Функция вычисления корня c точностью Е при заданных приближениях x0 и x1
double CalculateRoot(Equation eq, double eps, double x0, double x1)
{
    double raz, ss1, temp, xR = x1;
    raz = xR - x1;
    DefiniteIntegral r1, r2;
    r1 = eq.integral;
    r2 = eq.integral;
    r1.b = x1;
    r2.b = x0;
    //Вычисляем значение левой части уравнения для 2 начальных приближений корня
    r1 = CalculateIntegral(r1);
    r2 = CalculateIntegral(r2);
    //Находим приближенное значение корня методом секущих
    //Условие остановки - модуль разности текущего и предыдущего корней меньше заданного Е
    do
    {
        //Вычисляем новое приближение корня
        xR = xR - (xR - x0) / ((r1.result - eq.q) - (r2.result - eq.q)) * (r1.result - eq.q);
        raz = xR - x1;
        x0 = x1;
        x1 = xR;
        r1.b = x1;
        r2.b = x0;
        //Вычисляем значение левой части уравнения для текущего и предыдущего приближений
        r1 = CalculateIntegral(r1);
        r2 = CalculateIntegral(r2);
    } while (fabs(raz) > eps);
    return xR;
}
//Функция получения всех корней уравнения на отрезке [a;b] с точностью Е
void CalculateRoots(DefiniteIntegral integral, double q, double eps, set<double>& result)
{
    set<double, RootComparator> X{ RootComparator { eps } };
    Equation eq;
    eq.integral = integral;
    eq.q = q;
    DefiniteIntegral x0;
    //Вычисляем все корни на отрезке [a;b], разбивая отрезов на равные части
    for (int i = eq.integral.a; i < eq.integral.b; i++)
    {
        X.insert(CalculateRoot(eq, eps, i, i + 1));
    }
    //Убираем одинаковые корни и корни, которые отличаются меньше чем на E
    set<double>::iterator i;
    for (i = X.begin(); i != X.end(); ++i)
    {
        eq.integral.b = *i;
        x0 = CalculateIntegral(eq.integral);
        if (fabs(x0.result - eq.q) < eps)
        {
            result.insert(*i);
        }
    }
}