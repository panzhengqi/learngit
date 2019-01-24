#include "commonconfig.h"

CommonConfig::CommonConfig()
{
    //ctor
}
CommonConfig::~CommonConfig()
{
    //dtor
}

//获取当前程序目录
int  CommonConfig::GetCurrentPath(char buf[],char *pFileName)
{
    char pidfile[64];
    int bytes =0;
    int fd;
    sprintf(pidfile, "/proc/%d/cmdline", getpid());
    printf("%s\n",pidfile);
    fd = open(pidfile, O_RDONLY, 0);
    if(fd ==0)
        return -1;
    bytes = read(fd, buf, 256);
    close(fd);
    buf[MAX_PATH] = '\0';

    char * p = &buf[strlen(buf)];
    while( '/' != *p )
    {
        *p = '\0';
         p--;
    }
    p++;
    //配置文件目录
    //int len = strlen(buf);
    memcpy(p,pFileName,strlen(pFileName));
    return 0;
}

char * CommonConfig::GetIniKeyString(char *title, char *key, char *filename)
{
     FILE *fp;
     char szLine[1024];
     static char tmpstr[1024];
     int rtnval;
     int i = 0;
     int flag = 0;
     char *tmp;

     if ((fp = fopen(filename, "r")) == NULL)
     {
        printf("Have no such file [%s]", filename);
        return "Have no such file";
     }

     while (!feof(fp))
     {
          rtnval = fgetc(fp);

          if (rtnval == EOF)
          {
                break;
          }
          else
          {
             szLine[i++] = rtnval;
          }

          if (rtnval == '\n')
          {
               szLine[--i] = '\0';
               i = 0;
               tmp = strchr(szLine, '=');

                if (tmp != NULL && flag == 1)
                {
                    if (strstr(szLine, key) != NULL)
                    {
                        if ('#' == szLine[0] || ('\/' == szLine[0] && '\/' == szLine[1]))
                        {

                        }
                        else
                        {
                            strcpy(tmpstr, tmp + 1);
                            fclose(fp);
                            //     allTrim(tmpstr);
                            return tmpstr;
                        }
                    }
                }
                else
                {
                    strcpy(tmpstr, "[");
                    strcat(tmpstr, title);
                    strcat(tmpstr, "]");

                    if (strncmp(tmpstr, szLine, strlen(tmpstr)) == 0)
                    {
                    flag = 1;
                    }
                }
          }
     }
     fclose(fp);
     return "";
}

int CommonConfig::GetIniKeyInt(char *title, char *key, char *filename)
{
 return atoi(GetIniKeyString(title, key, filename));
}



