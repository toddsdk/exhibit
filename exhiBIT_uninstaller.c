#include <windows.h>

#include <objidl.h>
#include <shlobj.h>

#include <tlhelp32.h>

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>



const char g_szClassName[] = "wClass";

char desktop_folder_path[MAX_PATH] = {0}; 
char startup_path[MAX_PATH] = {0};

void get_startup_path(HWND hwnd, char *path) {

	SHGetFolderPath(hwnd, CSIDL_STARTUP, NULL, 0, path);
}

void get_desktop_folder_path(HWND hwnd, char *path) {
	
	SHGetFolderPath(hwnd, CSIDL_DESKTOP, NULL, 0, path);
	strcat(path, "\\exhiBIT");
}

int desinstalar(void) {
	char file_name[MAX_PATH] = {0};

	sprintf(file_name, "%s\\exhiBIT.exe", startup_path);
	DeleteFile(file_name);

//	sprintf(file_name, "%s\\exhiBIT_uninstaller.exe", desktop_folder_path);
//	DeleteFile(file_name);

	return TRUE;

}

HANDLE GetProcessHandle(LPSTR szExeName) {
	PROCESSENTRY32 Pc = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if(Process32First(hSnapshot, &Pc)){
		do{
			if(!strcmp(Pc.szExeFile, szExeName)) {
				return OpenProcess(PROCESS_ALL_ACCESS,
							TRUE,
							Pc.th32ProcessID);
			}
		}while(Process32Next(hSnapshot, &Pc));
	}
	return NULL;
}

int parar(char *program) {
	HANDLE hProcess = GetProcessHandle(program);
	DWORD fdwExit = 0;
	GetExitCodeProcess(hProcess, &fdwExit);
	TerminateProcess(hProcess, fdwExit);
	CloseHandle(hProcess);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	desinstalar();
	parar("exhiBIT.exe");
	PostQuitMessage(0);
		
	switch(msg) {
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	// some jibber-jabber about window's classes
	wc.cbSize	 = sizeof(WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon	 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm	 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		printf("Can't register window.\n");
		return 0;
	}

	// this is the forever-hidden window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		printf("Can't create window.\n");
		return 0;
	}

	get_startup_path(hwnd, &startup_path[0]);
	get_desktop_folder_path(hwnd, &desktop_folder_path[0]);

	parar("exhiBIT.exe");
	desinstalar();
	return 0;
	// spinning round and round, forever and ever
/*	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
*/
}

