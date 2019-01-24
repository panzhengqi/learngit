#ifndef COMMONCONFIG_H
#define COMMONCONFIG_H
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define  MAX_PATH 260


class CommonConfig
{
    public:
        CommonConfig();
        virtual ~CommonConfig();
    //获取当前程序目录
    int GetCurrentPath(char  buf[],char *pFileName);

   //获取 字符串的结果
    char *GetIniKeyString(char *title, char *key, char *filename);
    //获取 INT  型的 结果
    int GetIniKeyInt(char *title, char *key, char *filename);

    protected:

    private:
};

#endif // COMMONCONFIG_H
