#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include "resource.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <stdexcept>
using namespace std;
#include "IntegralLib.h"


#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWnd;                                      // дескриптор окна
HWND dialog;                                    // дескриптор диалогового окна  
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MainDialog(HWND, UINT, WPARAM, LPARAM);
//Главная функция
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
//Функция для получения и форматирования результата вычислений
string GetEquationRoots(double a, double b, double q, double eps)
{
    string result = "";
    int found = 0;
    DefiniteIntegral integral;
    integral.a = a;
    integral.b = b;
    integral = CalculateIntegralWithPrecision(integral, eps);
    set<double> X;
    CalculateRoots(integral, q, eps, X);
    set<double>::iterator i;
    for (i = X.begin(); i != X.end(); ++i)
    {
        result.append("X"+to_string(found+1)+" = "+to_string(*i)+"\r\n");
        integral.b = *i;
        integral = CalculateIntegral(integral);
        result.append("Интеграл (X=" + to_string(*i) + ") = " + to_string(integral.result) + "\r\n");
        result.append("\r\n");
        found++;
    }
    if (found == 0)result = "Метод не сходится. Невозожно найти корни";
    return result;  
}
//Функция для перевода переменной типа STRING в тип WSTRING
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

//Функция для перевода переменной типа TCHAR в тип STRING
std::string tchar_to_string(TCHAR* a)
{
   string s;
#ifndef UNICODE
    s = a;
#else
    std::wstring wStr = a;
    s = std::string(wStr.begin(), wStr.end());
#endif
    return s;
}

//Функция для получения значения типа DOUBLE из поля для ввода
//Если введенное значение не является числом, выбрасывается исключение
double GetDlgItemDouble (HWND hDlg, int id) 
{
    TCHAR buff[256];
    double dValue;
    LPTSTR endPtr;
    GetDlgItemText(hDlg, id, buff, 255);
    string strValue = tchar_to_string(buff);
    return stod(strValue);
}
//Обработчик
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
            SetDlgItemText(hDlg, IDC_BUTTON1, TEXT("Вычисление корней..."));
            double a, b, q, eps;
            try
            {
                //Получаем значения из формы
                a = GetDlgItemDouble(hDlg, IDC_EDIT1);
                b = GetDlgItemDouble(hDlg, IDC_EDIT2);
                q = GetDlgItemDouble(hDlg, IDC_EDIT3);
                eps = GetDlgItemDouble(hDlg, IDC_EDIT4);
                //Получаем результат
                string result = GetEquationRoots(a, b, q, eps);
                wstring tmp = s2ws(result);
                //Выводим результат
                SetDlgItemText(hDlg, IDC_EDIT6, tmp.c_str());
                SetDlgItemText(hDlg, IDC_BUTTON1, TEXT("Вычислить"));
            }
            catch (std::exception& e)
            {
                //В случае ошибки показываем соответствующее сообщение
                MessageBox(hDlg, TEXT("Один из параметров не является числом!"), TEXT("Ошибка"), NULL);
                SetDlgItemText(hDlg, IDC_BUTTON1, TEXT("Вычислить"));
            } 
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
