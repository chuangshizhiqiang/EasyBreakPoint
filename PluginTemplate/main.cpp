/***********************************************************************
    头文件
***********************************************************************/
// 系统文件
#include "interface.h"
#include <fstream>
#include <string>
#include <map>
#include <list>

// 本程序文件
#include "debug.h"
#include "config.h"
#include "entryPoint.h"
#include "settingInterface.h"

/***********************************************************************
    枚举配置
***********************************************************************/
enum
{
    MENU_SETTING,
    MENU_SETALL,
    MENU_CLEARALL,
    MENU_DELIMITER,
};

/***********************************************************************
    全局变量
***********************************************************************/
std::map<std::string, std::list<std::string>*> g_FileFunc;              // 文件对应全部 func
std::map<std::string, std::list<std::string>*> g_NeedSetFileFunc;       // 文件对应需要设置的 func
//std::map<std::string, std::list<std::string>*> g_SetedFileFunc;         // 文件对应已经设置的 func

/***********************************************************************
    示例
***********************************************************************/
//static void Adler32Menu(GUISELECTIONTYPE hWindow)
//{
//    if(!DbgIsDebugging())
//    {
//        dputs("You need to be debugging to use this command");
//        return;
//    }
//    SELECTIONDATA sel;
//    GuiSelectionGet(hWindow, &sel);
//    duint len = sel.end - sel.start + 1;
//    unsigned char* data = new unsigned char[len];
//    if(DbgMemRead(sel.start, data, len))
//    {
//        DWORD a = 1, b = 0;
//        for(duint index = 0; index < len; ++index)
//        {
//            a = (a + data[index]) % 65521;
//            b = (b + a) % 65521;
//        }
//        delete[] data;
//        DWORD checksum = (b << 16) | a;
//        dprintf("Adler32 of %p[%X] is: %08X\n", sel.start, len, checksum);
//    }
//    else
//        dputs("DbgMemRead failed...");
//}
//
//static bool cbTestCommand(int argc, char* argv[])
//{
//    dputs("Test command!");
//    char line[GUI_MAX_LINE_SIZE] = "";
//    if(!GuiGetLineWindow("test", line))
//        dputs("Cancel pressed!");
//    else
//        dprintf("Line: \"%s\"\n", line);
//    return true;
//}
//
//static duint exprZero(int argc, duint* argv, void* userdata)
//{
//    return 0;
//}


/***********************************************************************
    程序逻辑
***********************************************************************/
bool getBreakPointInfo() {
    CHAR pathBuf[MAX_PATH] = { 0 };
    std::string path;
    WIN32_FIND_DATAA wfd;
    HANDLE hFile = 0;

    GetCurrentDirectoryA(MAX_PATH, pathBuf);
    path = pathBuf;
    path += "\\" PLUGIN_DATADIRECTORY "\\";
    hFile = FindFirstFileA((path + "*").c_str(), &wfd);

    DBGPRINT("Search path is: %s\r\n", path.c_str());

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("[ERROR]FindFirstFile failed (%d)\n", GetLastError());
        return 0;
    }

    do
    {
        // 不处理文件夹
        if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            std::ifstream file(path + wfd.cFileName);
            std::string readBuf;
            std::list<std::string>* filebuf = new std::list<std::string>;

            if (!file.is_open()) {
                printf("[ERROR] open file %s error, code = %#x\r\n", (path + wfd.cFileName).c_str(), GetLastError());

                return 0;
            }

            // 读取所有函数
            while (std::getline(file, readBuf))
            {
                filebuf->push_back(readBuf);
                printf("[DEBUG] getFunc %s\r\n", readBuf.c_str());

                return 0;
            }
            // 存到全量表
            g_FileFunc.insert(std::pair<std::string, std::list<std::string>*>(wfd.cFileName, filebuf));
        }
    } while (FindNextFileA(hFile, &wfd));

    CloseHandle(hFile);

    return 1;
}


/***********************************************************************
    系统事件
***********************************************************************/
PLUG_EXPORT void CBLOADDLL(CBTYPE cbType, PLUG_CB_LOADDLL* info) {

    for (auto x = g_NeedSetFileFunc.begin(); x != g_NeedSetFileFunc.end(); x++) {
        auto unSuccessBp = new std::list<std::string>;

        *unSuccessBp = settingInterface::getInstance()->setArray(*x->second);

        
        x->second->clear();

        if (unSuccessBp->empty()) {
            delete unSuccessBp;
            unSuccessBp = 0;
            continue;
        }

        for (auto y = unSuccessBp->begin(); y != unSuccessBp->end(); y++) {
            x->second->push_back(*y);
        }

        delete unSuccessBp;

    }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_SETTING:
        //MessageBoxA(hwndDlg, "Test Menu Entry Clicked!", PLUGIN_NAME, MB_ICONINFORMATION);
        break;

    case MENU_SETALL:

        for (auto x = g_FileFunc.begin(); x != g_FileFunc.end(); x++) {
            auto unSuccessBp = new std::list<std::string>;

            //printf("[DEBUG]file is %s\r\n", x->first.c_str());
            //for (auto y = (*x->second).begin(); y != (*x->second).end(); y++) {
            //    printf("[DEBUG] func %s\r\n", y->c_str());
            //}

            *unSuccessBp = settingInterface::getInstance()->setArray(*x->second);

            if (unSuccessBp->empty()) {
                continue;
            }

            // 留着 DLL 加载事件再做一遍
            g_NeedSetFileFunc.insert(std::pair<std::string, std::list<std::string>*>(x->first, unSuccessBp));
        }

        break;

    case MENU_CLEARALL:
        for (auto x = g_FileFunc.begin(); x != g_FileFunc.end(); x++) {

            //printf("[DEBUG]file is %s\r\n", x->first.c_str());
            //for (auto y = (*x->second).begin(); y != (*x->second).end(); y++) {
            //    printf("[DEBUG] func %s\r\n", y->c_str());
            //}

            settingInterface::getInstance()->clearArray(*x->second);

            for (auto y = g_NeedSetFileFunc.begin(); y != g_NeedSetFileFunc.end(); y++) {
                delete y->second;
                y->second = 0;
            }
            g_NeedSetFileFunc.clear();

        }
        break;

    default:
        break;
    }
}


/***********************************************************************
    默认函数
***********************************************************************/
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    // 注册命令行
    //_plugin_registercommand(pluginHandle, PLUGIN_NAME, cbTestCommand, false);
    //_plugin_registerexprfunction(pluginHandle, PLUGIN_NAME ".zero", 0, exprZero, nullptr);

    //if (AllocConsole()) {
    //    FILE* pCout;
    //    freopen_s(&pCout, "conout$", "w", stdout);
    //}

    getBreakPointInfo();

    return true; //Return false to cancel loading the plugin.
}

void pluginStop()
{
}

void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU_SETTING, "Setting");
    _plugin_menuaddentry(hMenu, MENU_SETALL, "SetAll");
    _plugin_menuaddentry(hMenu, MENU_CLEARALL, "ClearAll");
    
}
