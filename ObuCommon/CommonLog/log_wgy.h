#ifndef LOG_WGY_H
#define LOG_WGY_H

/*************************************************************************
 * COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD              *
 *************************************************************************
 * File:       log.h
 * Brief:      none
 * Author      weigaorong
 * Version:    1.0.0
 * Data:       2017-07-6
 * Record:
 ************************************************************************/

#include<string>

using namespace std;


#define FL __FILE__ ":" TO_STRING(__LINE__)
#define TO_STRING(X)    TO__STRING(X)
#define TO__STRING(X)   #X

#define MAX_PATH        260
#define MAX_LOG_LINES   1500000

class DTLog
{
public:
    DTLog(const string strLogDir);
    ~DTLog();
    void create_new_log_file(string Log_Path);
    void close_old_log_file();
    void LogRecord(string context, const char *location = FL);
    int get_log_line_num();
    void clear_log_line_num();
    void inc_log_line_num();
    pthread_mutex_t  m_pMutex;     //used for log record synchronization
    pthread_mutex_t  ndis_pMutex;

private:
    DTLog();
    DTLog(DTLog &);
    DTLog& operator=(const DTLog &);
private:
    string           log_path;
    string           m_strFileFullname;
    FILE            *m_pFile;
    char             m_szBuf[MAX_PATH];
    int              current_line_num;
};


#endif // LOG_WGY_H
