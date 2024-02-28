
// this program implements a mini editor based on windows and event-messages
// it also has a console component for inspecting the actual software execution and 
// for controlling it via stdin


#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

#define READ_BUFFER 4096

#define AUTO_KILLER

#define WITH_TEXT
#define WITH_BUTTON

#define VERBOSE if(0)

char text[READ_BUFFER];
HANDLE file;

char *command1 = (char*)"command1";
char *command2 = (char*)"command2";
char *term = (char*)"term";

UINT command1_type = 0;
UINT command2_type = 0;
UINT term_type = 0;
UINT msg_type = 0;


HWND hWindow;
HWND hwndButtonA;
HWND hwndButtonB;
HWND hEdit;


LRESULT CALLBACK WndProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam) {

        DWORD size;

        if (message == WM_CREATE) {
                printf("window creation ok\n");
                fflush(stdout);
                return 0;
        }

        if (message == command1_type) {
                printf("requested command 1\n");
                fflush(stdout);
                return 1;
        }

        if (message == command2_type) {
                printf("requested command 2\n");
                fflush(stdout);
                return 2;
        }

        if (message == term_type) {
                printf("requested termination\n");
                fflush(stdout);
                PostQuitMessage(0);
                return 3;
        }

        if (message == WM_COMMAND) {
                VERBOSE printf("process asked to run some menu/command - param is %d\n", wParam);
                if (wParam == BN_CLICKED) {
                        printf("button pressed\n");
                        if ((HWND)lParam == hwndButtonA) {
                                printf("button identified - load text\n");
                                SendMessage(hEdit, WM_SETTEXT, READ_BUFFER, (LPARAM)text);
                                fflush(stdout);
                        }
                        if ((HWND)lParam == hwndButtonB) {
                                printf("button identified - store text\n");
                                SendMessage(hEdit, WM_GETTEXT, READ_BUFFER, (LPARAM)text);
                                printf("text window got message:\n%s\n", text);

                                SetFilePointer(file, 0, NULL, FILE_BEGIN);
                                SetEndOfFile(file);
                                if (WriteFile(file, text, strlen(text), &size, NULL) == 0) {
                                        printf("Cannot write to file\n");
                                        fflush(stdout);
                                }

                                fflush(stdout);
                        }
                }
                fflush(stdout);

#ifdef WITH_TEXT
                if (wParam == 128) {

                        SendMessage(hEdit, WM_GETTEXT, READ_BUFFER, (LPARAM)text);
                        printf("mineditor got request to quit message - text box had this content:\n%s\n", text);
                        printf("Exiting process\n");
                        fflush(stdout);
                        ExitProcess(0);
                }
#endif
        }

        if (message == WM_CLOSE) {
                printf("process asked to terminate for windows closure\n");
                fflush(stdout);
                //ExitProcess(0);
                return 4;
        }

        //if (message == WM_PAINT) printf("got WM_PAINT\n");
        VERBOSE printf("going for default  treatment\n");
        return (DefWindowProc(hWindow, message, wParam, lParam));

}


DWORD WINAPI Killer(void * nothing) {

        int ret;
        UINT msg_type;
        char buff[READ_BUFFER];

        while (1) {
                scanf("%s", buff);

                printf("trying to kill with '%s' event-message\n", buff);
                fflush(stdout);

                msg_type = RegisterWindowMessage(buff);
                if (!msg_type) {
                        printf("Can't create '%s' event-message for error %u\n", buff, GetLastError());
                        fflush(stdout);
                        ExitProcess(1);
                }
                else {
                        printf("event-message '%s' correctly registered - code is %u\n", buff, msg_type);
                        fflush(stdout);
                }

                ret = PostMessage(HWND_BROADCAST, msg_type, 0, 0);
                printf("event-message post returned %d\n", ret);
                fflush(stdout);
        }

}



void main(int argc, char *argv[]){

        struct _thread_info * thread_info = NULL;

        WNDCLASS wndclass;
        char nome_applicazione[] = "test";
        int ret;
        DWORD size;
        MSG msg;
        HMENU hMenu;
        HMENU MenuList;
        HMENU BMenu;


        file = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file == INVALID_HANDLE_VALUE) {
                printf("Cannot open file to edit\n");
                fflush(stdout);
                return;
        }

        if (ReadFile(file, text, READ_BUFFER, &size, NULL) == 0) {
                printf("Cannot read from file\n");
                fflush(stdout);
                return;
        }

        text[size] = '\0';

        term_type = RegisterWindowMessage(term);
        if (!term_type) {
                printf("Can't create term message for error %d\n", GetLastError());
                fflush(stdout);
                ExitProcess(1);
        }
        else {
                printf("term event-message correctly registered - code is %u\n", term_type);
                fflush(stdout);
        }

        command1_type = RegisterWindowMessage(command1);
        if (!command1_type) {
                printf("Can't create command1 message for error %d\n", GetLastError());
                fflush(stdout);
                ExitProcess(1);
        }
        else {
                printf("command1 event-message correctly registered - code is %u\n", command1_type);
        }

        command2_type = RegisterWindowMessage(command2);
        if (!command1_type) {
                printf("Can't create command2 message for error %d\n", GetLastError());
                fflush(stdout);
                ExitProcess(1);
        }
        else {
                printf("command2 event-message correctly registered - code is %u\n", command2_type);
                fflush(stdout);
        }

        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = NULL;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, (LPTSTR)IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = nome_applicazione;

        if (!RegisterClass(&wndclass)) {
                printf("Can't register class"); fflush(stdout);
                ExitProcess(1);
        }


        //managing the mneu
        hMenu = CreateMenu();

        AppendMenu(hMenu, MF_STRING, 128, "Quit");


        MenuList = CreateMenu();

        AppendMenu(MenuList, MF_POPUP, (UINT_PTR)hMenu, "menu");

        hWindow = CreateWindow(nome_applicazione,
                TEXT("Windows mini editor"),
                WS_OVERLAPPEDWINDOW,
                10, 20, 650, 600,
                NULL,
                MenuList,
                NULL,
                NULL);

        if (hWindow == INVALID_HANDLE_VALUE) {
                printf("Can't create window for error %d\n", GetLastError());
                fflush(stdout);
                ExitProcess(-1);
        }


#ifdef WITH_BUTTON
        hwndButtonA = CreateWindow(
                "BUTTON",  // Predefined class
                "Load text",      // Button text
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                10,         // x position
                20,         // y position
                100,        // Button width
                60,        // Button height
                hWindow,     // Parent window
                NULL,       // No menu.
                (HINSTANCE)GetWindowLong(hWindow, GWL_HINSTANCE),
                NULL);      // Pointer not needed.

        hwndButtonB = CreateWindow(
                "BUTTON",  // Predefined class
                "Store text",      // Button text
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                10,         // x position
                80,         // y position
                100,        // Button width
                60,        // Button height
                hWindow,     // Parent window
                NULL,       // No menu.
                (HINSTANCE)GetWindowLong(hWindow, GWL_HINSTANCE),
                NULL);      // Pointer not needed.

#endif

#ifdef WITH_TEXT
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                120,
                20,
                500,
                500,
                hWindow,
                NULL,
                GetModuleHandle(NULL),
                NULL);
#endif


        ShowWindow(hWindow, SW_SHOWNORMAL);


#ifdef AUTO_KILLER
        if ((CreateThread(NULL, 0, Killer, NULL, 0, NULL)) == INVALID_HANDLE_VALUE) {
                printf("Can't start event-message listening thread\n");
                fflush(stdout);
                ExitProcess(-1);
        }
#endif

        printf("start polling on main thread\n");
        while (ret = GetMessage(&msg, NULL, 0, 0)) {
                if (ret == -1) {
                        printf("event-message poll error\n");
                }
                else {
                        VERBOSE printf("got event-message while polling - handle is %u - type is %u\n", msg.hwnd, msg.message);
                        fflush(stdout);
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                }
        }

        ExitProcess(0);


}



