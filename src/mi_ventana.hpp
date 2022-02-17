// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// para 64bits GWLP_HINSTANCE
// compilar: cl ejecutate.cpp /EHsc /DUNICODE /link /subsystem:windows
// rc nombre-archivo-recursos.rc
#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <tchar.h>
#include <WinUser.h>
#include <sstream>
#include <shellapi.h>
#include <Commctrl.h>
#include <Wingdi.h>
#include "recursos.h"

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")

// ventana principal
#define ANCHO_VENTANA 378
#define ALTO_VENTANA 171

// boton si
#define LEFT_BOTON_SI 170
#define TOP_BOTON_SI 96
#define ANCHO_BOTON_SI 88
#define ALTO_BOTON_SI 26

// boton no
#define LEFT_BOTON_NO 266
#define TOP_BOTON_NO 96
#define ANCHO_BOTON_NO 88
#define ALTO_BOTON_NO 26

//texto
#define LEFT_BOTON_TEXT 65
#define TOP_BOTON_TEXT 26
#define ANCHO_BOTON_TEXT 261
#define ALTO_BOTON_TEXT 32

// icono de error
#define LEFT_ICON 25
#define TOP_ICON 26
#define WEIGHT_ICON 32
#define HEIGHT_ICON 32

#define ID_BUTTON_YES 99
#define ID_TEXTO 100
#define ID_GRIS 101

class CustomWindow
{
public:
	CustomWindow()
	{

		WNDCLASSEX wincl = {};
		//ZeroMemory(&wincl, sizeof(wincl));
		wincl.hInstance = instance;
		wincl.lpszClassName = TEXT("WindowsApp");
		wincl.lpfnWndProc = WindowProcedure;
		wincl.style = CS_HREDRAW | CS_VREDRAW;
		wincl.cbSize = sizeof(wincl);
		wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // fondo blanco
		wincl.hIcon = LoadIcon(instance, MAKEINTRESOURCE(VBOXICON16X16)); // 32x32
		wincl.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(VBOXICON32X32)); // 16x16
		wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
		InitCommonControls();
		RegisterClassEx(&wincl);
	}

	bool Center(HWND hwndParent) const// posicionar una ventana en el centro de su ventana padre
	{
		HWND parent = hwndParent;
		RECT rectWindow, rectParent;

		// make the window relative to its parent
		if (parent != NULL)
		{
			GetWindowRect(main_hwnd, &rectWindow);
			GetWindowRect(parent, &rectParent);

			int nWidth = rectWindow.right - rectWindow.left;
			int nHeight = rectWindow.bottom - rectWindow.top;

			int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
			int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

			int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			// make sure that the dialog box never moves outside of the screen
			if (nX < 0)
				nX = 0;
			if (nY < 0)
				nY = 0;
			if (nX + nWidth > nScreenWidth)
				nX = nScreenWidth - nWidth;
			if (nY + nHeight > nScreenHeight)
				nY = nScreenHeight - nHeight;

			MoveWindow(main_hwnd, nX, nY, nWidth, nHeight, FALSE);

			return true;
		}

		return false;
	}
	// TODO: instancia como parametro
	bool CreateNewWindow(HWND hwndParent, const char* title, const char* text)
	{
		response = 0;
		parent = hwndParent;
		/*
			WS_OVERLAPPED ->
			WS_CAPTION -> borde
			WS_SYSMENU -> icono del titulo
			WS_THICKFRAME -> podemos estirar la ventana pero yo no quiero esto...
		*/
		main_hwnd = CreateWindow(
			TEXT("WindowsApp"),
			title,
			WS_SYSMENU | WS_CAPTION | WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			ANCHO_VENTANA, ALTO_VENTANA,
			hwndParent, NULL,
			instance, NULL
		);
		if (main_hwnd == NULL) {
			return false;
		}
		HWND fondo_gris = CreateWindow(TEXT("STATIC"),
										nullptr,
										WS_VISIBLE | WS_CHILD,
										0, 86, // cuanto menos sea mas ancho es
										ANCHO_VENTANA, ALTO_VENTANA,
										main_hwnd, (HMENU)ID_GRIS,
										GetModuleHandle(NULL), NULL);
		// BS_DEFPUSHBUTTON para que sea el boton que destaque
		HWND SiButtom = CreateWindow(TEXT("BUTTON"),
			TEXT("&Sí"),
			BS_DEFPUSHBUTTON | WS_VISIBLE | WS_CHILD,
			LEFT_BOTON_SI, TOP_BOTON_SI,
			ANCHO_BOTON_SI, ALTO_BOTON_SI,
			main_hwnd, (HMENU)ID_BUTTON_YES,
			instance, NULL); // instance 64bits casteamos a LONG_PTR

		HWND NoButtom = CreateWindow(TEXT("BUTTON"),
			TEXT("&No"),
			WS_VISIBLE | WS_CHILD,
			LEFT_BOTON_NO, TOP_BOTON_NO,
			ANCHO_BOTON_NO, ALTO_BOTON_NO,
			main_hwnd, NULL,
			instance, NULL);

		// EL TEXTO
		HWND texto = CreateWindow(TEXT("STATIC"),
			text,
			WS_VISIBLE | WS_CHILD,
			LEFT_BOTON_TEXT, TOP_BOTON_TEXT,
			ANCHO_BOTON_TEXT, ALTO_BOTON_TEXT,
			main_hwnd, (HMENU)ID_TEXTO,
			instance, NULL);

		// icono error de windows
		HWND image = CreateWindow(TEXT("STATIC"),
			nullptr,
			SS_ICON | WS_VISIBLE | WS_CHILD,
			LEFT_ICON, TOP_ICON,
			WEIGHT_ICON, HEIGHT_ICON,
			main_hwnd, NULL,
			instance, NULL);

		HFONT hFont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Segoe UI"));
		if (!hFont)
		{
			return false;
		}
		HICON icon = LoadIconA(0, (LPCSTR)IDI_ERROR);
		SendMessage(image, STM_SETICON, (LPARAM)icon, FALSE);
		Button_SetElevationRequiredState(SiButtom, TRUE);

		SendMessage(texto, WM_SETFONT, (LPARAM)hFont, FALSE);
		SendMessage(NoButtom, WM_SETFONT, (LPARAM)hFont, FALSE);
		SendMessage(SiButtom, WM_SETFONT, (LPARAM)hFont, FALSE);

		return true;
	}
	void Show()
	{

		/* Make the window visible on the screen */

		ShowWindow(main_hwnd, 1);

		UpdateWindow(main_hwnd);

		/* Run the message loop. It will run until GetMessage() returns 0 */
		while (GetMessage(&messages, NULL, 0, 0) > 0)
		{
			/* Translate virtual-key messages into character messages */
			TranslateMessage(&messages);
			DispatchMessage(&messages);
		}
		SetForegroundWindow(parent); // para que me mantenga virtualbox en el foco, es decir, el padre
	}
	WORD GetAnswer() const {
		return response;
	}
	~CustomWindow() {
		//DeleteObject(hBrush);
		DeleteObject(hFont);
		EnableWindow(parent, true);

	}

private:
	static HWND parent;
	static WORD response;
	HFONT hFont;
	HWND main_hwnd;
	MSG messages = {};
	DWORD err;
	HINSTANCE instance = GetModuleHandle(NULL);
	
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		
		switch (message)
		{
		case WM_CREATE:
			EnableWindow(parent, false); // desactivamos el padre; ademas si arrastramos la ventana padre lentamente la ventana hija se centra igualmente 
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
			case BN_CLICKED:
				if (LOWORD(wParam) == ID_BUTTON_YES) {
					response = IDYES;
				}
				break;
			}
			DestroyWindow(hwnd);
			break;
		case WM_CTLCOLORSTATIC:
		{
			DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
			if (CtrlID == ID_GRIS) //If desired control
			{				
				return (INT_PTR)CreateSolidBrush(RGB(240,240,240)); // gris claro
				
			}
			else {
				return (INT_PTR)CreateSolidBrush(RGB(255,255,255));

			}
			break;

		}

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		return 0;
	}
};

HWND CustomWindow::parent = NULL;
WORD CustomWindow::response = 0;