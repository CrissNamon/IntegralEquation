#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include "resource.h"
#include <sstream>
#include "IntegralLib.h"
#include <set>
#include <algorithm>
#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif
#define MAX_LOADSTRING 100
using namespace std;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWnd;
HWND dialog;
HWND hEditA, hEditB, hEditQ, hEditEps;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MainDialog(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EQUATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EQUATION));
    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EQUATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EQUATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 700, 250, nullptr, nullptr, hInstance, nullptr);
   dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, MainDialog);
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
string GetEquationRoots(double a, double b, double q, double eps)
{
    string result = "";
    DefiniteIntegral Integral;
    Integral.a = a;
    Integral.b = b;
    Integral.iterations = 1;
    unsigned long n = 1;
    double sum1, sum2;
    DefiniteIntegral r1, r2;
    r1 = CalculateIntegral(Integral);
    sum1 = r1.result;
    do
    {
        n *= 2;
        r1.iterations = n;
        sum2 = sum1;
        r2 = r1;
        r1 = CalculateIntegral(r1);
        sum1 = r1.result;
    } while (fabs(sum1 - sum2) > eps);
    set<double, RootComparator> X{ RootComparator { eps } };
    Equation eq;
    DefiniteIntegral x0;
    int found = 1;
    eq.integral = r1;
    eq.q = q;
    for (int i = a; i < b; i++)
    {
        X.insert(CalculateRoot(eq, eps, i, i + 1));
    }
    set<double>::iterator i;
    for (i = X.begin(); i != X.end(); ++i)
    {
        r1.b = *i;
        x0 = CalculateIntegral(r1);
        if (fabs(x0.result - eq.q) < eps)
        {
            result.append("X"+to_string(found)+"="+to_string(*i)+"\r\n");
            found++;
        }
    }
    if (found == 1)result = "Метод не сходится. Невозожно найти корни";
    return result;
    
}

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
INT_PTR CALLBACK MainDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_BUTTON1:
            TCHAR buff[1024];
            double a, b, q, eps, dValue;
            LPTSTR endPtr;
            GetDlgItemText(hDlg, IDC_EDIT1, buff, 255);
            dValue = _tcstod(buff, &endPtr);
            a = dValue;
            GetDlgItemText(hDlg, IDC_EDIT2, buff, 255);
            dValue = _tcstod(buff, &endPtr);
            b = dValue;
            GetDlgItemText(hDlg, IDC_EDIT3, buff, 255);
            dValue = _tcstod(buff, &endPtr);
            q = dValue;
            GetDlgItemText(hDlg, IDC_EDIT4, buff, 255);
            dValue = _tcstod(buff, &endPtr);
            eps = dValue;
            string result = GetEquationRoots(a, b, q, eps);
            wstring tmp = s2ws(result);
            SetDlgItemText(hDlg, IDC_EDIT6, tmp.c_str());
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
