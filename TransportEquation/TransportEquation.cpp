// TransportEquation.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TransportEquation.h"
#include <string>

using namespace std;

const double max_x = 1.0;
const double max_t = 1.0;
const int xRange = 20; //количество точек для x координаты
const int tRange = 300; //количество точек для времени
const int currTime = 100; //текущий момент времени
double u[tRange][xRange]; //Решение уравнения теплопроводности

inline double initCondition(double x) {
	if (x >= 0)
		return max_x;
	return 0;
}

inline double boundaryСondition(double xmin, double t) {
	if (xmin - t >= 0)
		return max_t;
	return 0;
}

//Решиние уравнения переноса методом конечных разностей
void solveTransportEquation(double xmin, double xmax, double tmax, int N, double u[][xRange]) {
	double ht = tmax / N; //шаг по времени
	double hx = (xmax - xmin) / xRange; //шаг по x

	//вычисление функции U в начальный момент времени
	for (int i = 0; i < xRange; i++) {
		u[0][i] = initCondition(xmin + i * hx);
	}

	//решение уравнения du/dt = -du/dx
	for (int j = 0; j < N-1; j++) {
		//вычисление функции U при нулевой координате
		u[j + 1][0] = boundaryСondition(xmin, (j + 1) * ht);
		
		//вычисление U в остальных случаях
		for (int i = 1; i < xRange; i++) {
			double dudx = (u[j][i] - u[j][i - 1]) / hx;
			u[j + 1][i] = -dudx * ht + u[j][i];
		}
	}
}

// Функция рисования графика
void drawGraph(HDC hdc, RECT rectClient,
	double x[][xRange], // массив данных
	int n, // количество точек
	int currTime) // текущий момент времени
{
	double OffsetY, OffsetX;
	double MAX_X, MAX_Y;
	double ScaleX, ScaleY;
	double min, max;
	int height, width;
	int X, Y; // координаты в окне (в px)
	HPEN hpen;
	
	height = rectClient.bottom - rectClient.top;
	width = rectClient.right - rectClient.left;
	
	// Область допустимых значений X
	min = x[currTime][0];
	max = x[currTime][0];
	for (int i = 0; i < n; i++)
	{
		if (x[currTime][i] < min) min = x[currTime][i];
		if (x[currTime][i] > max) max = x[currTime][i];
	}

	MAX_X = n;
	OffsetX = 0.1 * width / MAX_X; // смещение X
	ScaleX = (double)width / MAX_X; // масштабный коэффициент X
	MAX_Y = max - min;
	OffsetY = 0.5 * max * height / (MAX_Y); // смещение Y
	ScaleY = (double)height / MAX_Y * 0.5; // масштабный коэффициент Y
	
	SetTextColor(hdc, 0x00FF0000); // синий цвет букв
	string s = to_string(MAX_Y);
	std::wstring ws;
	ws.assign(s.begin(), s.end());
	TextOut(hdc, width - 100, MAX_Y + 10, ws.c_str(), 8);
	TextOut(hdc, 10, MAX_Y + 10, L"U(x, t)", 8);
	TextOut(hdc, width * 0.3, OffsetY + 20, L"Решение уравнения переноса методом конечных разностей", 60);
	TextOut(hdc, width * 0.45, OffsetY + 40, L"t = 100", 7);
	TextOut(hdc, width - 40, OffsetY + 10, L"X", 1);

	// Отрисовка осей координат
	hpen = CreatePen(PS_SOLID, 0, 0); // черное перо 1px
	SelectObject(hdc, hpen);
	MoveToEx(hdc, 0, OffsetY, 0); // перемещение в точку (0;OffsetY)
	LineTo(hdc, width, OffsetY); // рисование горизонтальной оси
	MoveToEx(hdc, OffsetX, 0, 0); // перемещение в точку (OffsetX;0)
	LineTo(hdc, OffsetX, height); // рисование вертикальной оси (не видна)
	DeleteObject(hpen); // удаление черного пера
	
	// Отрисовка графика функции
	int color = 0xFF; // красное перо для первого ряда данных

	hpen = CreatePen(PS_SOLID, 2, color); // формирование пера 2px
	SelectObject(hdc, hpen);
	X = (int)(OffsetX + 0 * ScaleX); // начальная точка графика
	Y = (int)(OffsetY - x[currTime][0] * ScaleY);
	MoveToEx(hdc, X, Y, 0); // перемещение в начальную точку
	for (int i = 0; i < n; i++)
	{
		X = OffsetX + i* ScaleX;//x[0][i] * ScaleX;
		Y = OffsetY - x[currTime][i] * ScaleY;
		LineTo(hdc, X, Y);
	}
	color = color << 8; // изменение цвета пера для следующего ряда
	DeleteObject(hpen); // удаление текущего пера
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRANSPORTEQUATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	solveTransportEquation(0, 1, 1, tRange, u);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRANSPORTEQUATION));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRANSPORTEQUATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRANSPORTEQUATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			drawGraph(hdc, ps.rcPaint, u, xRange, currTime); // построение графика

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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}