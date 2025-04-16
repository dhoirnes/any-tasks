#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <complex>
#include <iomanip>

using namespace std;

// Глобальные переменные:
HINSTANCE hInst;
HWND hEditOutputResult;
HWND hEditOutputAllResults;
HWND hPlotWindow;
vector<double> numberArray;
vector<complex<double>> DFTResult;

// Прототипы функций:
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ResultWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AllResultsWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PlotWndProc(HWND, UINT, WPARAM, LPARAM);
void GenerateArray(int type);
void ShowArray();
void ClearArray();
void ComputeDFT();
void ComputeReDFT();
void ComputeImDFT();
void ComputeAmplitudeSpectrum();
void ComputePhaseSpectrum();
void ComputeIDFT();
void ShowAllResults();
void PlotArray(HWND hWnd, const vector<double>& data);
void PlotReArray(HWND hWnd, const vector<double>& data);
void PlotImArray(HWND hWnd, const vector<double>& data);
void PlotAArray(HWND hWnd, const vector<double>& data);
void PlotFiArray(HWND hWnd, const vector<double>& data);
void PlotIDTFArray(HWND hWnd, const vector<double>& data);
void PlotRealDFT(HWND hWnd);
void PlotImagDFT(HWND hWnd);
void PlotAmplitudeSpectrum(HWND hWnd);
void PlotPhaseSpectrum(HWND hWnd);
void PlotIDFT(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // Регистрация класса для главного окна
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"MainWndClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    // Регистрация класса для окна результатов
    wcex.lpfnWndProc = ResultWndProc;
    wcex.lpszClassName = L"ResultWndClass";
    RegisterClassEx(&wcex);

    // Регистрация класса для окна всех результатов
    wcex.lpfnWndProc = AllResultsWndProc;
    wcex.lpszClassName = L"AllResultsWndClass";
    RegisterClassEx(&wcex);

    // Регистрация класса для окна графиков
    wcex.lpfnWndProc = PlotWndProc;
    wcex.lpszClassName = L"PlotWndClass";
    RegisterClassEx(&wcex);

    hInst = hInstance;

    // Создание главного окна
    HWND hWndMain = CreateWindow(L"MainWndClass", L"Управление Массивом", WS_OVERLAPPEDWINDOW,
        0, 30, 800, 350, nullptr, nullptr, hInstance, nullptr);

    // Создание окна для вывода результатов
    HWND hWndResult = CreateWindow(L"ResultWndClass", L"Результат", WS_OVERLAPPEDWINDOW,
        820, 20, 500, 400, nullptr, nullptr, hInstance, nullptr);

    // Создание окна для вывода всех результатов
    HWND hWndAllResults = CreateWindow(L"AllResultsWndClass", L"Все Результаты", WS_OVERLAPPEDWINDOW,
        1340, 20, 500, 400, nullptr, nullptr, hInstance, nullptr);

    // Создание окна для вывода графиков
    hPlotWindow = CreateWindow(L"PlotWndClass", L"Графики", WS_OVERLAPPEDWINDOW,
        20, 500, 1900, 500, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWndMain, nCmdShow);
    ShowWindow(hWndResult, nCmdShow);
    ShowWindow(hWndAllResults, nCmdShow);
    ShowWindow(hPlotWindow, nCmdShow);
    UpdateWindow(hWndMain);
    UpdateWindow(hWndResult);
    UpdateWindow(hWndAllResults);
    UpdateWindow(hPlotWindow);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Обработка окна с кнопками для работы с массивом
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        CreateWindow(L"BUTTON", L"Синусоида", WS_CHILD | WS_VISIBLE,
            10, 10, 240, 30, hWnd, (HMENU)1, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Косинусоида", WS_CHILD | WS_VISIBLE,
            260, 10, 240, 30, hWnd, (HMENU)2, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Синусоида + косинусоида", WS_CHILD | WS_VISIBLE,
            510, 10, 240, 30, hWnd, (HMENU)3, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Синусоида * Косинусоида", WS_CHILD | WS_VISIBLE,
            10, 50, 240, 30, hWnd, (HMENU)4, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Синусоида 2", WS_CHILD | WS_VISIBLE,
            260, 50, 240, 30, hWnd, (HMENU)5, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Показать массив", WS_CHILD | WS_VISIBLE,
            510, 50, 240, 30, hWnd, (HMENU)6, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Очистить массив", WS_CHILD | WS_VISIBLE,
            10, 90, 240, 30, hWnd, (HMENU)7, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Вычислить ДПФ", WS_CHILD | WS_VISIBLE,
            260, 90, 240, 30, hWnd, (HMENU)8, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Вычислить Re ДПФ", WS_CHILD | WS_VISIBLE,
            510, 90, 240, 30, hWnd, (HMENU)9, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Вычислить Im ДПФ", WS_CHILD | WS_VISIBLE,
            10, 130, 240, 30, hWnd, (HMENU)10, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Апмлитудный спектр", WS_CHILD | WS_VISIBLE,
            260, 130, 240, 30, hWnd, (HMENU)11, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Фазовый спектр", WS_CHILD | WS_VISIBLE,
            510, 130, 240, 30, hWnd, (HMENU)12, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Вычислить ОДПФ", WS_CHILD | WS_VISIBLE,
            10, 170, 240, 30, hWnd, (HMENU)13, hInst, nullptr);
        CreateWindow(L"BUTTON", L"Показать все результаты", WS_CHILD | WS_VISIBLE,
            260, 170, 240, 30, hWnd, (HMENU)14, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График массива", WS_CHILD | WS_VISIBLE,
            510, 170, 240, 30, hWnd, (HMENU)15, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График Re ДПФ", WS_CHILD | WS_VISIBLE,
            10, 210, 240, 30, hWnd, (HMENU)16, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График Im ДПФ", WS_CHILD | WS_VISIBLE,
            260, 210, 240, 30, hWnd, (HMENU)17, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График Амплитудного спектра", WS_CHILD | WS_VISIBLE,
            510, 210, 240, 30, hWnd, (HMENU)18, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График Фазового спектра", WS_CHILD | WS_VISIBLE,
            10, 250, 240, 30, hWnd, (HMENU)19, hInst, nullptr);
        CreateWindow(L"BUTTON", L"График ОДПФ", WS_CHILD | WS_VISIBLE,
            260, 250, 240, 30, hWnd, (HMENU)20, hInst, nullptr);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            GenerateArray(LOWORD(wParam));
            break;
        case 6:
            ShowArray();
            break;
        case 7:
            ClearArray();
            break;
        case 8:
            ComputeDFT();
            break;
        case 9:
            ComputeReDFT();
            break;
        case 10:
            ComputeImDFT();
            break;
        case 11:
            ComputeAmplitudeSpectrum();
            break;
        case 12:
            ComputePhaseSpectrum();
            break;
        case 13:
            ComputeIDFT();
            break;
        case 14:
            ShowAllResults();
            break;
        case 15:
            InvalidateRect(hPlotWindow, NULL, TRUE);  
            SendMessage(hPlotWindow, WM_COMMAND, 15, 0);
            break;
        case 16:
            InvalidateRect(hPlotWindow, NULL, TRUE);  
            SendMessage(hPlotWindow, WM_COMMAND, 16, 0);
            break;
        case 17:
            InvalidateRect(hPlotWindow, NULL, TRUE); 
            SendMessage(hPlotWindow, WM_COMMAND, 17, 0);
            break;
        case 18:
            InvalidateRect(hPlotWindow, NULL, TRUE);  
            SendMessage(hPlotWindow, WM_COMMAND, 18, 0);
            break;
        case 19:
            InvalidateRect(hPlotWindow, NULL, TRUE);  
            SendMessage(hPlotWindow, WM_COMMAND, 19, 0);
            break;
        case 20:
            InvalidateRect(hPlotWindow, NULL, TRUE);  
            SendMessage(hPlotWindow, WM_COMMAND, 20, 0);
            break;
        default:
            break;
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

// Обработка окна вывода промежуточных результатов
LRESULT CALLBACK ResultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hEditOutputResult = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
            10, 10, 760, 340, hWnd, nullptr, hInst, nullptr);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработка окна вывода всех результатов
LRESULT CALLBACK AllResultsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hEditOutputAllResults = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
            10, 10, 760, 340, hWnd, nullptr, hInst, nullptr);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработка окна работы с графиками
LRESULT CALLBACK PlotWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_COMMAND:
        switch (wParam)
        {
        case 15:
            PlotArray(hWnd, numberArray);
            break;
        case 16:
            PlotRealDFT(hWnd);
            break;
        case 17:
            PlotImagDFT(hWnd);
            break;
        case 18:
            PlotAmplitudeSpectrum(hWnd);
            break;
        case 19:
            PlotPhaseSpectrum(hWnd);
            break;
        case 20:
            PlotIDFT(hWnd);
            break;
        default:
            break;
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

void GenerateArray(int type) // Создание массивов
{
    const int N = 64;
    numberArray.clear();
    double amplitude = 1.0;
    double frequency = 1.0;
    double phase = 0.0;

    switch (type)
    {
    case 1: // Синусоида
        for (int i = 0; i < N; ++i)
        {
            numberArray.push_back(amplitude * sin(2 * M_PI * frequency * i / N + phase));
        }
        break;
    case 2: // Косинусоида
        for (int i = 0; i < N; ++i)
        {
            numberArray.push_back(amplitude * cos(2 * M_PI * frequency * i / N + phase));
        }
        break;
    case 3: // Синусоида + Косинусоида
        for (int i = 0; i < N; ++i)
        {
            numberArray.push_back(amplitude * sin(2 * M_PI * frequency * i / N + phase) + amplitude * cos(2 * M_PI * frequency * i / N + phase));
        }
        break;
    case 4: // Синусоида * Косинусоида
        for (int i = 0; i < N; ++i)
        {
            numberArray.push_back(amplitude * sin(2 * M_PI * frequency * i / N + phase) * amplitude * cos(2 * M_PI * frequency * i / N + phase));
        }
        break;
    case 5: // Синусоида 2
        frequency = 5.0;
        for (int i = 0; i < N; ++i)
        {
            numberArray.push_back(amplitude * sin(2 * M_PI * frequency * i / N + phase));
        }
        break;
    }
}

void ShowArray() // Вывод массива
{
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < numberArray.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << numberArray[i] << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}



void ClearArray()   // Очистка массива и окон
{
    numberArray.clear();
    if (hEditOutputResult)
    {
        SetWindowText(hEditOutputResult, L"");
    }
    if (hEditOutputAllResults)
    {
        SetWindowText(hEditOutputAllResults, L"");
    }
}

void ComputeDFT()// Вычисление ДПФ
{
    const int N = numberArray.size();
    DFTResult.clear();
    for (int k = 0; k < N; ++k)
    {
        complex<double> sum(0.0, 0.0);
        for (int n = 0; n < N; ++n)
        {
            double angle = 2 * M_PI * k * n / N;
            sum += numberArray[n] * complex<double>(cos(angle), -sin(angle));
        }
        DFTResult.push_back(sum);
    }
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << i << L": " << fixed << setw(7) << setprecision(5) << DFTResult[i] << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ComputeReDFT() // Вычисление Re ДПФ
{
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << i << L": " << fixed << setprecision(5) << DFTResult[i].real() << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ComputeImDFT() // Вычисление Im ДПФ
{
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << DFTResult[i].imag() << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ComputeAmplitudeSpectrum() // Вычисление Амплитудного спектра
{
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << abs(DFTResult[i]) << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ComputePhaseSpectrum() // Вычисление Фазового спектра
{
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << arg(DFTResult[i]) << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ComputeIDFT()  // Вычисление ОДПФ
{
    const int N = DFTResult.size();
    vector<complex<double>> idftResult(N);
    for (int k = 0; k < N; ++k)
    {
        complex<double> sum(0.0, 0.0);
        for (int n = 0; n < N; ++n)
        {
            double angle = 2 * M_PI * k * n / N;
            sum += DFTResult[n] * complex<double>(cos(angle), sin(angle));
        }
        idftResult[k] = sum / static_cast<double>(N);
    }
    if (hEditOutputResult)
    {
        wstringstream ss;
        for (size_t i = 0; i < idftResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << idftResult[i].real() << L"\r\n";
        }
        SetWindowText(hEditOutputResult, ss.str().c_str());
    }
}

void ShowAllResults() {
    if (hEditOutputAllResults)
    {
        wstringstream ss;
        ss << L"Оригинальный массив:\r\n";  // Вывод данных об исходном массиве
        for (size_t i = 0; i < numberArray.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << numberArray[i] << L"\r\n";
        }
        ss << L"\r\nДПФ:\r\n";  // Вывод ДПФ
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << DFTResult[i] << L"\r\n";
        }
        ss << L"\r\nRe ДПФ:\r\n";   // Вывод Re ДПФ
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << DFTResult[i].real() << L"\r\n";
        }
        ss << L"\r\nIm ДПФ:\r\n";   // Вывод Im ДПФ
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << DFTResult[i].imag() << L"\r\n";
        }
        ss << L"\r\nАмплитудный спектр:\r\n";   // Вывод Амплитудного спектра ДПФ
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << abs(DFTResult[i]) << L"\r\n";
        }
        ss << L"\r\nФазовый спектр:\r\n";   // Вывод Фазового спектра ДПФ
        for (size_t i = 0; i < DFTResult.size(); ++i)
        {
            ss << setw(2) << setfill(L' ') << i << L": " << fixed << setprecision(5) << arg(DFTResult[i]) << L"\r\n";
        }
        SetWindowText(hEditOutputAllResults, ss.str().c_str());
    }
}

void ShowErrorMessage(HWND hWnd)
{
    MessageBox(hWnd, L"Массив пуст!", L"Ошибка", MB_OK | MB_ICONERROR);
}

void ShowArrayExistsErrorMessage(HWND hWnd)
{
    MessageBox(hWnd, L"Массив уже существует! Очистите массив перед созданием нового.", L"Ошибка", MB_OK | MB_ICONERROR);
}

void PlotArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr); 
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr); 
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr); 
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"k", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"X(k)", 4);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 0.2)
    double scaleY = 0.2; // Шаг равный 0.2
    int numTicksY = 10; // Количество делений по оси Y (5 вверх и 5 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -1.0 && valueY <= 1.0) 
        { // Ограничиваем значения по Y от -1 до 1
            int yPos = centerY - static_cast<int>(valueY * height / 2) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0) 
            {
                wchar_t labelY[10];
                if (valueY > 0) 
                {
                    swprintf(labelY, 10, L"+%.1f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else {
                    swprintf(labelY, 10, L"%.1f", valueY); // Выводим значение с одним знаком после точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY)); 
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
}

void PlotReArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr);
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr);
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr);
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"u", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"Re(u)", 5);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 10)
    double scaleY = 10.0; // Шаг равный 10
    int numTicksY = 4; // Количество делений по оси Y (4 вверх и 4 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -40.0 && valueY <= 40.0)
        { // Ограничиваем значения по Y от -40 до 40
            int yPos = centerY - static_cast<int>(valueY * height / 80) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0)
            {
                wchar_t labelY[10];
                if (valueY > 0)
                {
                    swprintf(labelY, 10, L"+%.0f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else
                {
                    swprintf(labelY, 10, L"%.0f", valueY); // Выводим значение без десятичной точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY));
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        MoveToEx(hdc, margin + i * step * (width / numTicksX), centerY + 10, nullptr);
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);

}

void PlotImArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr);
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr);
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr);
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"u", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"Re(u)", 5);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 10)
    double scaleY = 10.0; // Шаг равный 10
    int numTicksY = 4; // Количество делений по оси Y (4 вверх и 4 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -40.0 && valueY <= 40.0)
        { // Ограничиваем значения по Y от -40 до 40
            int yPos = centerY - static_cast<int>(valueY * height / 80) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0)
            {
                wchar_t labelY[10];
                if (valueY > 0)
                {
                    swprintf(labelY, 10, L"+%.0f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else
                {
                    swprintf(labelY, 10, L"%.0f", valueY); // Выводим значение без десятичной точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY));
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        MoveToEx(hdc, margin + i * step * (width / numTicksX), centerY + 10, nullptr);
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
}

void PlotAArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr);
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr);
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr);
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"u", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"Re(u)", 5);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 10)
    double scaleY = 10.0; // Шаг равный 10
    int numTicksY = 4; // Количество делений по оси Y (4 вверх и 4 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -40.0 && valueY <= 40.0)
        { // Ограничиваем значения по Y от -40 до 40
            int yPos = centerY - static_cast<int>(valueY * height / 80) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0)
            {
                wchar_t labelY[10];
                if (valueY > 0)
                {
                    swprintf(labelY, 10, L"+%.0f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else
                {
                    swprintf(labelY, 10, L"%.0f", valueY); // Выводим значение без десятичной точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY));
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        MoveToEx(hdc, margin + i * step * (width / numTicksX), centerY + 10, nullptr);
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
}


void PlotFiArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr);
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr);
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr);
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"X", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"Y", 1);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 1)
    double scaleY = 1.0; // Шаг равный 1
    int numTicksY = 5; // Количество делений по оси Y (5 вверх и 5 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -5.0 && valueY <= 5.0)
        { // Ограничиваем значения по Y от -5 до 5
            int yPos = centerY - static_cast<int>(valueY * height / 10) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0)
            {
                wchar_t labelY[10];
                if (valueY > 0)
                {
                    swprintf(labelY, 10, L"+%.0f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else
                {
                    swprintf(labelY, 10, L"%.0f", valueY); // Выводим значение без десятичной точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY));
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        MoveToEx(hdc, margin + i * step * (width / numTicksX), centerY + 10, nullptr);
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
}


void PlotIDTFArray(HWND hWnd, const vector<double>& data)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int width = 1800 - 2 * 40; // Ширина окна - учет отступов
    int height = 400 - 2 * 40; // Высота окна - учет отступов
    int margin = 60; // Отступ слева
    int step = 1; // Шаг по оси X задаем равным 1

    // Центр по X
    int centerX = margin + width / 2;
    int centerY = margin + height / 2;

    // Отрисовка осей
    MoveToEx(hdc, margin, centerY + 10, nullptr);
    LineTo(hdc, width + margin, centerY + 10);

    // Ось Y 
    MoveToEx(hdc, margin, centerY + height / 2 + 10, nullptr);
    LineTo(hdc, margin, centerY - height / 2 + 10);

    // Стрелки на осях
    // Стрелка на оси X
    MoveToEx(hdc, width + margin - 10, centerY + 10 - 5, nullptr);
    LineTo(hdc, width + margin, centerY + 10);
    LineTo(hdc, width + margin - 10, centerY + 10 + 5);

    // Стрелка на оси Y 
    MoveToEx(hdc, margin - 5, margin + 10, nullptr);
    LineTo(hdc, margin, margin);
    LineTo(hdc, margin + 5, margin + 10);

    // Подписи к осям
    // Ось X
    TextOut(hdc, width + margin - 15, centerY + 10 + 10, L"k", 1);
    // Ось Y
    TextOut(hdc, margin - 25, margin - 15, L"X(k)", 4);

    // Масштаб по осям
    // Масштаб по оси X (шаг равный 1)
    int numTicksX = data.size();
    for (int i = 0; i < numTicksX; ++i)
    {
        int xPos = margin + i * step * (width / numTicksX);
        MoveToEx(hdc, xPos, centerY + 10 - 5, nullptr);
        LineTo(hdc, xPos, centerY + 10 + 5);
        // Подписи чисел на оси X
        wchar_t labelX[10];
        swprintf(labelX, 10, L"%d", i); // Выводим номер точки на оси X
        TextOut(hdc, xPos - 10, centerY + 10 + 10, labelX, wcslen(labelX));
    }

    // Масштаб по оси Y (шаг равный 0.2)
    double scaleY = 0.2; // Шаг равный 0.2
    int numTicksY = 10; // Количество делений по оси Y (5 вверх и 5 вниз)
    for (int i = -numTicksY; i <= numTicksY; ++i)
    {
        double valueY = i * scaleY;
        if (valueY >= -1.0 && valueY <= 1.0)
        { // Ограничиваем значения по Y от -1 до 1
            int yPos = centerY - static_cast<int>(valueY * height / 2) + 10;

            MoveToEx(hdc, margin - 5, yPos, nullptr);
            LineTo(hdc, margin + 5, yPos);

            // Подписи чисел на оси Y
            if (valueY != 0.0)
            {
                wchar_t labelY[10];
                if (valueY > 0)
                {
                    swprintf(labelY, 10, L"+%.1f", valueY); // Добавляем знак "+" перед положительными значениями
                }
                else {
                    swprintf(labelY, 10, L"%.1f", valueY); // Выводим значение с одним знаком после точки
                }
                TextOut(hdc, margin - 40, yPos - 10, labelY, wcslen(labelY));
            }
        }
    }

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, margin, centerY + 10 - data[0] * height / 2, nullptr);
    for (size_t i = 1; i < data.size(); ++i)
    {
        LineTo(hdc, margin + i * step * (width / numTicksX), centerY + 10 - data[i] * height / 2);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    EndPaint(hWnd, &ps);
}


void PlotRealDFT(HWND hWnd)
{
    vector<double> ReDFT;
    for (const auto& val : DFTResult)
    {
        ReDFT.push_back(val.real());
    }
    PlotReArray(hWnd, ReDFT);
}

void PlotImagDFT(HWND hWnd)
{
    vector<double> ImDFT;
    for (const auto& val : DFTResult)
    {
        ImDFT.push_back(val.imag());
    }
    PlotImArray(hWnd, ImDFT);
}

void PlotAmplitudeSpectrum(HWND hWnd)
{
    vector<double> amplitudes;
    for (const auto& val : DFTResult)
    {
        amplitudes.push_back(abs(val));
    }
    PlotAArray(hWnd, amplitudes);
}

void PlotPhaseSpectrum(HWND hWnd)
{
    vector<double> phases;
    for (const auto& val : DFTResult)
    {
        phases.push_back(arg(val));
    }
    PlotFiArray(hWnd, phases);
}

void PlotIDFT(HWND hWnd)
{
    const int N = DFTResult.size();
    vector<double> idftResult(N);
    for (int k = 0; k < N; ++k)
    {
        double sum = 0.0;
        for (int n = 0; n < N; ++n)
        {
            double angle = 2 * M_PI * k * n / N;
            sum += DFTResult[n].real() * cos(angle) - DFTResult[n].imag() * sin(angle);
        }
        idftResult[k] = sum / static_cast<double>(N);
    }
    PlotIDTFArray(hWnd, idftResult);
}