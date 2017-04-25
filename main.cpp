#define WINVER 0x0500
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <shellapi.h>
#include <tlhelp32.h>

using namespace std;

LPCTSTR gEX;
LPCTSTR gWN;
LPCTSTR oID;
HANDLE GetHndl(LPCTSTR pName);
int Start(LPCTSTR pName, LPCTSTR pArgs);
bool IsRunning(LPCTSTR pName);
void GoTo(LPCTSTR pName);
void Kill(LPCTSTR pName);
void PressPlay();

int main(int argc, char **argv)
{
    FreeConsole();
    //Return error if no parameter is specified
    if (argv[1]==NULL){
        MessageBox(0, "Specify which game to launch: F16 or F17", "Error", MB_OK);
        return 1;}
    if (_stricmp(argv[1], "F15") == 0) {
        gEX = "fifa15.exe";
        gWN = "FIFA 15";
        oID = "origin://launchgame/Origin.Origin.OFR.50.0000462";//fifa 16
        }
    else if (_stricmp(argv[1], "F16") == 0) {
        gEX = "fifa16.exe";
        gWN = "FIFA 16";
        oID = "origin://launchgame/Origin.OFR.50.0000781";//fifa 16
        }
    else if (_stricmp(argv[1], "F17") == 0) {
        gEX = "FIFA17.exe";
        gWN = "FIFA 17";
        oID = "origin://launchgame/Origin.OFR.50.0001057";//fifa 17
    }
    else {
        MessageBox(0, "Please specify a valid parameter: F15, F16, or F17", "Error", MB_OK);
        return 1;
        }
    //determine origin directory
    LPCTSTR oDir;
    if (argv[2]!=NULL){
        oDir = argv[2];}
    else oDir = "C:/PROGRA~2/Origin/Origin.exe";
    //check if origin is running
    if (IsRunning("Origin.exe")){
        Kill("Origin.exe");}
    //start origin as child program
    if (Start(oDir,"-StartClientMinimized") <= 32){
        MessageBox(0, "Failed to launch Origin.", "Error", MB_OK);
        return 1;}
    while (!IsRunning("Origin.exe")){
        Sleep(1000);}
    //wait for origin to go idle
    Sleep(8000);
    //launch game
    Start(oID, NULL);
    while (!IsRunning("fifaconfig.exe")){
        Sleep(500);}
    //go to fifaconfig.exe window and press play
    GoTo(gWN);
    Sleep(100);
    PressPlay();
    //exit if game does not start after 40s
    for (int i = 1; i<41; i++){
        Sleep (1000);}
    if (!IsRunning(gEX)){
        MessageBox(0, "Game doesn't appear to be running. Closing launcher.", "Error", MB_OK);
        Kill("Origin.exe");
        return 1;}
    //close fifaconfig window
    Kill("fifaconfig.exe");
    //wait for game to close
    while (IsRunning(gEX)){
        Sleep(1000);}
    //give some time for cloud sync
    Sleep(8000);
    //Close Origin
    Kill("Origin.exe");
    return 0;
}

//--------------- F U N C T I O N S ---------------//

HANDLE GetHndl(LPCTSTR pName){
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE){
        while (Process32Next(snapshot, &entry) == TRUE){
            if (_stricmp(entry.szExeFile, pName ) == 0){
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
            CloseHandle(snapshot);
            return hProcess;}}
    }return 0;
}

int Start(LPCTSTR pName, LPCTSTR pArgs){
    HINSTANCE h = ShellExecute(NULL,"open",pName,pArgs,NULL,SW_SHOW);
    int r = (int) h;
    return r;
}

bool IsRunning(LPCTSTR pName){
    if (GetHndl(pName)==0)
        return 0;
    return 1;
}
void GoTo(LPCTSTR pName){
    HWND hwnd = FindWindow(NULL,pName);
    if(hwnd){SetForegroundWindow(hwnd);}
}

void PressPlay(){
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = 0x0D;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

void Kill(LPCTSTR pName){
    HANDLE hProcess = GetHndl(pName);
    TerminateProcess(hProcess, 0 );
}

