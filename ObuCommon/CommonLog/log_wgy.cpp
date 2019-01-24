
/*************************************************************************
 * COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD              *
 *************************************************************************
 * File:       log.cpp
 * Brief:      Record log information to file
 * Author      weigaorong
 * Version:    1.0.0
 * Data:       2017-06-28
 * Record:
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include<log_wgy.h>

/************************************************************************
 * Method:     DTLog
 * FullName:   DTLog::DTLog
 * Function:   Create log file
 * Acces:      none
 * Return:     none
 * Qualifier:  none
 * Parameter:  strLogDir   the save path of log
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
DTLog::DTLog(const string strLogDir) :
    log_path(strLogDir),
    m_pFile(NULL)
{
    pthread_mutex_init(&m_pMutex, NULL);
    pthread_mutex_init(&ndis_pMutex, NULL);
    current_line_num = 0;
    //Get time and format time to string
    struct timeval tv;
    char szNow[50] = "";
    gettimeofday(&tv, NULL);
    strftime(szNow, sizeof(szNow), "Log_%Y-%m-%d_%T.log", localtime(&tv.tv_sec));
    m_strFileFullname = log_path + szNow;

    //make file, create file header
    m_pFile = fopen(m_strFileFullname.data(), "w+");
    string strFileHeader = "File fullname: " + m_strFileFullname + "\n";
    fputs(strFileHeader.data(), m_pFile);
    strFileHeader = "\n<: enter function\n>: exit function, default true\n\n";
    fputs(strFileHeader.data(), m_pFile);
    strFileHeader = "/***********Begin**********/\n";
    fputs(strFileHeader.data(), m_pFile);
}

/*************************************************************************
 * Method:     ~DTLog
 * FullName:   DTLog::~DTLog
 * Function:   Close log file
 * Acces:      none
 * Return:     none
 * Qualifier:  none
 * Parameter:  none
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
DTLog::~DTLog()
{
    string strEndFlag("/***********End**********/\n");

    pthread_mutex_lock(&m_pMutex);
    if(NULL != m_pFile)
    {
        fputs(strEndFlag.data(), m_pFile);
        fclose(m_pFile);
        m_pFile = NULL;
    }
    pthread_mutex_unlock(&m_pMutex);
    pthread_mutex_destroy(&m_pMutex);
    pthread_mutex_destroy(&ndis_pMutex);
}


void DTLog::create_new_log_file(string Log_Path)
{
    struct timeval tv;
    char szNow[50] = "";
    gettimeofday(&tv, NULL);
    strftime(szNow, sizeof(szNow), "Log_%Y-%m-%d_%T.log", localtime(&tv.tv_sec));
    m_strFileFullname = Log_Path + szNow;
//    pthread_mutex_lock(&m_pMutex);
    close_old_log_file();
    //make file, create file header
    m_pFile = fopen(m_strFileFullname.data(), "w+");
    string strFileHeader = "File fullname: " + m_strFileFullname + "\n";
    fputs(strFileHeader.data(), m_pFile);
    strFileHeader = "\n<: enter function\n>: exit function, default true\n\n";
    fputs(strFileHeader.data(), m_pFile);
    strFileHeader = "/***********Begin**********/\n";
    fputs(strFileHeader.data(), m_pFile);
//    pthread_mutex_unlock(&m_pMutex);
}


void DTLog::close_old_log_file()
{
    if(NULL != m_pFile)
    {
        string strEndFlag("/***********End**********/\n");
        fputs(strEndFlag.data(), m_pFile);
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

/*************************************************************************
 * Method:     LogRecord
 * FullName:   DTLog::LogRecord
 * Function:   Write log info to file
 * Acces:      none
 * Return:     none
 * Qualifier:  none
 * Parameter:  none
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
void DTLog::LogRecord(string context, const char *location)
{
    //Add '\n' for file line
    if('\n' != *(context.data() + context.size() - 1))
    {
            context += '\n';
    }
    pthread_mutex_lock(&m_pMutex);
    if(MAX_LOG_LINES <= get_log_line_num())
    {
        create_new_log_file(log_path);
        clear_log_line_num();
    }
    inc_log_line_num();
    //Format string for easy reading
    sprintf(m_szBuf, "%18s ", location);
    context = m_szBuf + context;
    fputs(context.data(), m_pFile);
    fflush(m_pFile);
    pthread_mutex_unlock(&m_pMutex);
    return;
}


int DTLog::get_log_line_num()
{
//    int line_num = 0;
//    pthread_mutex_lock(&m_pMutex);
//    line_num = current_line_num;
//    pthread_mutex_unlock(&m_pMutex);
    return current_line_num;
}


void DTLog::clear_log_line_num()
{
//    pthread_mutex_lock(&m_pMutex);
    current_line_num = 0;
//    pthread_mutex_unlock(&m_pMutex);
}


void DTLog::inc_log_line_num()
{
//    pthread_mutex_lock(&m_pMutex);
    current_line_num++;
//    pthread_mutex_unlock(&m_pMutex);
}




