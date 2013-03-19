#include <windows.h>

//#include <objbase.h>
//#include <shellapi.h>
//#include <oleauto.h>

#include <objidl.h>
#include <shlobj.h>

//#include <shlguid.h>
#include <tchar.h>
#include <wctype.h>
#include <dbt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>


#define MIN_TIME 30
#define MAX_TIME 150
#define MAX_FILE_SIZE 100000000 // 100 MBytes
#define MIN_FILE_SIZE 10

char desktop_folder_path[MAX_PATH] = {0}; 
char recent_path[MAX_PATH] = {0};
int timer_id = 0;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

const char g_szClassName[] = "wClass";

struct bmpfile_magic {
  unsigned char magic[2];
};

struct bmpfile_header {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
};

typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} BITMAPINFOHEADERR;

int exhiBIT(char *file_name) {

	FILE *fp;
	struct stat  file_stat;
	int size;
	char *file;
	struct bmpfile_magic magicnum = {{0x42, 0x4d}};
	struct bmpfile_header header;
	BITMAPINFOHEADERR dib_header;
	int w, h;


	time_t tt;
	struct tm *tmp;
	char timestamp[50] = {0};
	char *base_file_name;
	char *output_name;

	stat(file_name, &file_stat);
	size = file_stat.st_size;

	file = malloc(size + 1);
	if(file == NULL) {
		printf("Faltou memoria p/ criar a imagem!\n");
		return FALSE;
	}

	fp = fopen(file_name, "rb");
	fread(file, size, 1, fp);
	file[size] = 0;
	fclose(fp);

	header.filesz = size + 54;
	header.creator1 = 0;
	header.creator2 = 0;
	header.bmp_offset = 54;


	h = w = (int) round(sqrt(size/3) - 1);
	dib_header.header_sz = 40;
	dib_header.width = w;
	dib_header.height = h;
	dib_header.nplanes = 1;
	dib_header.bitspp = 24;
	dib_header.compress_type = 0;
	dib_header.bmp_bytesz = size;
	dib_header.hres = 0;
	dib_header.vres = 0;
	dib_header.ncolors = 0;
	dib_header.nimpcolors = 0;
	//printf("%s %d %d\n", file_name, dib_header.width, dib_header.height);


	base_file_name = basename(file_name);
	tt = time(NULL);
	tmp = localtime(&tt);
	strftime(timestamp, 50, "%Y_%m_%d_%Hh%Mm%Ss", tmp);
	output_name = malloc(MAX_PATH);
	sprintf(output_name, "%s\\%s_%s_exhiBITed.bmp", desktop_folder_path, base_file_name, timestamp);

	fp = fopen(output_name, "wb");
	fwrite(&magicnum, sizeof(struct bmpfile_magic), 1, fp);
	fwrite(&header, sizeof(struct bmpfile_header), 1, fp);
	fwrite(&dib_header, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(file, size, 1, fp);
	fclose(fp);
	free(file);

	printf("Imagem criada em\n%s\ntamanho: %d\n", output_name, size);
	free(output_name);
	return TRUE;

}


char *ResolveIt(HWND hwnd, LPCSTR lpszLinkFile) { 
    HRESULT hres; 
    IShellLink* psl; 
    WCHAR szGotPath[MAX_PATH]; 
    WCHAR szDescription[MAX_PATH]; 
    WIN32_FIND_DATA wfd; 
    LPSTR target_path;
 
	char *extension = NULL;
	extension = strrchr(lpszLinkFile, '.');
	if(extension != NULL) {
		if(strcmp(extension, ".lnk")) {
			target_path = malloc(MAX_PATH);
			sprintf(target_path, "%s", lpszLinkFile);
			return target_path;
		}
	} else {
		target_path = malloc(MAX_PATH);
		sprintf(target_path, "%s", lpszLinkFile);
		return target_path;
	}

    CoInitialize(NULL);
    // Get a pointer to the IShellLink interface.
    hres = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
        hres = psl->lpVtbl->QueryInterface(psl,&IID_IPersistFile,(void**)&ppf); 
		
        if (SUCCEEDED(hres)) { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH); 
 
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Load the shortcut. 
            hres = ppf->lpVtbl->Load(ppf, wsz, STGM_READ); 
            if (SUCCEEDED(hres)) { 

	        // Resolve the link. 
                hres = psl->lpVtbl->Resolve(psl, hwnd, SLR_NO_UI); 

                if (SUCCEEDED(hres)) { 
                    // Get the path to the link target. 
                    hres = psl->lpVtbl->GetPath(psl, (LPSTR)szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_UNCPRIORITY); 
                    //hres = psl->lpVtbl->GetPath(psl, (LPSTR)szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH); 

                    if (SUCCEEDED(hres)) { 
			//printf("TARGET: %s\n" , szGotPath);
			//wprintf(L"TARGET: %s\n" , szGotPath);
                        //hres = swprintf(lpszPath, L"%s", szGotPath);
			target_path = malloc(MAX_PATH);
			sprintf(target_path, "%s", szGotPath);
                        if (!SUCCEEDED(hres)) {
			    printf("Erro ao resolver link!\n");
        		    ppf->lpVtbl->Release(ppf); 
			    psl->lpVtbl->Release(psl); 
			    return NULL;
		        }
                    }
                } 
            } 

            // Release the pointer to the IPersistFile interface. 
            ppf->lpVtbl->Release(ppf); 
        } 

        // Release the pointer to the IShellLink interface. 
        psl->lpVtbl->Release(psl); 
    } 
    return target_path;
}


void get_recent_path(HWND hwnd, char *path) {

	SHGetFolderPath(hwnd, CSIDL_RECENT, NULL, 0, path);
}

void get_desktop_folder_path(HWND hwnd, char *path) {
	
	SHGetFolderPath(hwnd, CSIDL_DESKTOP, NULL, 0, path);
	strcat(path, "\\exhiBIT");

	// ensure that the directory is there
	CreateDirectory(path, NULL);
}

int count_files(char *find_path) {

	WIN32_FIND_DATA find_data;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int error_code;
	int count = 0;

	hFind = FindFirstFile(find_path, &find_data);

	if (hFind == INVALID_HANDLE_VALUE) {
		return -1;
	} 

	if(strcmp(find_data.cFileName, ".") &&
	   strcmp(find_data.cFileName, "..")) {
		count++;
	}


	while (FindNextFile(hFind, &find_data) != 0) {
		count++;
	}

	error_code = GetLastError();
	FindClose(hFind);
	if (error_code != ERROR_NO_MORE_FILES) {
		return -error_code;
	}
	return count;
}


// return a random file within a folder
char *pick(HWND hwnd, char *path) {

	WIN32_FIND_DATA find_data;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int error_code;
	char *find_path = NULL;
	char *link_path = NULL;
	char *target_path = NULL;
	char *next_target_path = NULL;
	int attr;
	int count;
	int file, i;


	find_path = malloc(MAX_PATH);
	sprintf(find_path, "%s\\*", path);

	count = count_files(find_path);


	hFind = FindFirstFile(find_path, &find_data);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf ("Invalid file handle. Error is %u.\n", GetLastError());
		free(find_path);
		return NULL;
	} 

	//printf ("First file name is %s.\n", find_data.cFileName);

	link_path = malloc(MAX_PATH);
	
	file = rand() % count;
	i = 0;
	//printf("i=%d file=%d count=%d\n", i, file, count);
	if(strcmp(find_data.cFileName, ".") &&
	   strcmp(find_data.cFileName, "..")) {

		if(i == file) {
			sprintf(link_path, "%s\\%s", path, find_data.cFileName);
			//puts(link_path);
			target_path = ResolveIt(hwnd, link_path);
			//printf("target: %s\n", target_path);

			if((attr = GetFileAttributes(target_path))
			   != INVALID_FILE_ATTRIBUTES) {
				if(attr & FILE_ATTRIBUTE_DIRECTORY) {
					FindClose(hFind);
					free(find_path);
					free(link_path);
					next_target_path = pick(hwnd, target_path);
					free(target_path);
					return next_target_path;
				} else/* if(attr & FILE_ATTRIBUTE_NORMAL) */{

					FindClose(hFind);
					free(find_path);
					free(link_path);
					return target_path;
				}
			} else {
				FindClose(hFind);
				free(find_path);
				free(link_path);
				free(target_path);
				return NULL;
			}
		}
	}

	i++;

	while (FindNextFile(hFind, &find_data) != 0 && i < count) {
	//	printf("i=%d file=%d count=%d\n", i, file, count);
		if(strcmp(find_data.cFileName, ".") &&
		   strcmp(find_data.cFileName, "..")) {

			if(i == file) {
				sprintf(link_path, "%s\\%s", path, find_data.cFileName);
				//puts(link_path);
				target_path = ResolveIt(hwnd, link_path);
				//printf("target: %s\n", target_path);

				if((attr = GetFileAttributes(target_path))
				   != INVALID_FILE_ATTRIBUTES) {
					if(attr & FILE_ATTRIBUTE_DIRECTORY) {
						FindClose(hFind);
						free(find_path);
						free(link_path);
						next_target_path = pick(hwnd, target_path);
						free(target_path);
						return next_target_path;
					} else/* if(attr & FILE_ATTRIBUTE_NORMAL)*/ {
						FindClose(hFind);
						free(find_path);
						free(link_path);
						return target_path;
					}
				} else {
					FindClose(hFind);
					free(find_path);
					free(link_path);
					free(target_path);
					return NULL;
				}
			}
		}
		i++;
	}

	if (GetLastError() != ERROR_NO_MORE_FILES)
		printf ("FindNextFile error. Error is %u.\n", error_code);
	FindClose(hFind);
	free(find_path);
	free(link_path);
	free(target_path);
	return NULL;
}


void book_next_exhiBIT(HWND hwnd) {
	static int second = 1000;
	static int minute = 60;
	int chaos =  MIN_TIME + (rand() % (MAX_TIME-MIN_TIME));
	timer_id = SetTimer(hwnd,timer_id, chaos * minute * second,(TIMERPROC)NULL);
	printf("Proximo exhiBIT em %d segundos\n", chaos);
}

int check_file_size(char *file_path) {
	struct stat file_stat;
	stat(file_path, &file_stat);
	if(file_stat.st_size <= MAX_FILE_SIZE &&
	   file_stat.st_size >= MIN_FILE_SIZE)
		return TRUE;
	return FALSE;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	char *file = NULL;

	switch(msg) {
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_TIMER:
	
			do {
				file = pick(hwnd, recent_path);
				if(file)
					if(check_file_size(file))
						break;
			} while(TRUE);
			printf("exhiBITing: %s\n", file);
			exhiBIT(file);
			book_next_exhiBIT(hwnd);
			free(file);
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

	// Summon the Chaos Entities
	srand(time(NULL));

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

	// this is where we should find all our twisted material >;-)
	get_recent_path(hwnd, &recent_path[0]);

	// and this is where the exhiBITed images will live
	get_desktop_folder_path(hwnd, &desktop_folder_path[0]);

	// We don't want any windows showing around
	// Double-comment to ensure evilness 

/*
	// ShowWindow(hwnd, nCmdShow);
	// UpdateWindow(hwnd);
*/

	// how about some exhiBITing?
	book_next_exhiBIT(hwnd);

	// spinning round and round, forever and ever
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

