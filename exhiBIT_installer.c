#include <windows.h>

#include <objidl.h>
#include <shlobj.h>

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "exhiBIT_installer_data.h"

const char g_szClassName[] = "wClass";
char desktop_folder_path[MAX_PATH] = {0}; 
char desktop_subfolder_path[MAX_PATH] = {0}; 
char startup_path[MAX_PATH] = {0};

void get_startup_path(HWND hwnd, char *path) {

	SHGetFolderPath(hwnd, CSIDL_STARTUP, NULL, 0, path);
	printf("%s\n", path);
}

void get_desktop_folder_path(HWND hwnd, char *path) {
	
	SHGetFolderPath(hwnd, CSIDL_DESKTOP, NULL, 0, path);
	strcat(path, "\\exhiBIT");

	// ensure that the directory is there
	CreateDirectory(path, NULL);
}

void get_desktop_subfolder_path(HWND hwnd, char *path) {
	
	SHGetFolderPath(hwnd, CSIDL_DESKTOP, NULL, 0, path);
	strcat(path, "\\exhiBIT\\files");

	// ensure that the directory is there
	CreateDirectory(path, NULL);
}

int instalar(void) {
	FILE *fp;
	char *file;
	char output_name[MAX_PATH] = {0};
	char input_name[MAX_PATH] = {0};
	int path_size = 0;

	sprintf(output_name, "%s\\exhiBIT.exe", startup_path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData1[0], SIZE_FILE_1, 1, fp);
	fclose(fp);

	sprintf(output_name, "%s\\exhiBIT_uninstaller.exe", desktop_subfolder_path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData2[0], SIZE_FILE_2, 1, fp);
	fclose(fp);

	sprintf(output_name, "%s\\exhiBIT_README.txt", desktop_subfolder_path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData3[0], SIZE_FILE_3, 1, fp);
	fclose(fp);

	sprintf(output_name, "%s\\0xF0A7C.txt", desktop_subfolder_path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData4[0], SIZE_FILE_4, 1, fp);
	fclose(fp);



	path_size = GetCurrentDirectory(0, NULL);
	GetCurrentDirectory(path_size, &input_name[0]);
	strcat(input_name, "\\exhiBIT_installer.exe");
	sprintf(output_name, "%s\\exhiBIT_installer.exe", desktop_subfolder_path);
	CopyFile(input_name, output_name, FALSE);
	return TRUE;
}


int rodar(char *program, char *cmdline) {
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);	

	CreateProcess(
		program,
		cmdline,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startup_info,
		&process_info
	);

	CloseHandle(process_info.hProcess);
	CloseHandle(process_info.hThread);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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
	char exhiBIT_path[MAX_PATH] = {0};
	char readme_path[MAX_PATH] = {0};

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
	get_desktop_subfolder_path(hwnd, &desktop_subfolder_path[0]);

	instalar();
	sprintf(exhiBIT_path, "%s\\exhiBIT.exe", startup_path);
	rodar(exhiBIT_path, NULL);
	sprintf(readme_path, "C:\\Windows\\notepad.exe %s\\exhiBIT_README.txt", desktop_subfolder_path);
	rodar(NULL, readme_path);
	return 0;
}
