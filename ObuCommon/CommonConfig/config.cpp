/*************************************************************************
 * COPYRIGHT DaTang Mobile Communications Equipment CO.,LTD              *
 *************************************************************************
 * File:       config.cpp
 * Brief:      Read the config info and complete config work
 * Author      weigaorong
 * Version:    1.0.0
 * Data:       2017-06-28
 * Record:
 ************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <QStringList>
#include <qqueue.h>

#define MAX_PATH   260

/************************************************************************
 * Method:     DTConfig
 * FullName:   DTConfig::DTConfig
 * Function:
 * Access:
 * Return:     none
 * Qualifier:  none
 * Parameter:
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
DTConfig::DTConfig(string strFileFullname, bool IsRead)
    : m_strFileFullname(strFileFullname)
    , m_IsRead(IsRead)
    , m_pFile(NULL)
{
  //  Log("DTConfig::DTConfig()<");
    //Log("DTConfig::DTConfig()>");
}

/************************************************************************
 * Method:     ~DTConfig
 * FullName:   DTConfig::~DTConfig
 * Function:
 * Access:
 * Return:     none
 * Qualifier:  none
 * Parameter:
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
DTConfig::~DTConfig()
{
   // Log("DTConfig::~DTConfig()<");
    CloseFile();
   // Log("DTConfig::~DTConfig()>");
}

/************************************************************************
 * Method:     OpenFile
 * FullName:   DTConfig::OpenFile
 * Function:   Open the configuration file
 * Access:
 * Return:     Return true if open configuration file succussfully, or false
 * Qualifier:  none
 * Parameter:
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
bool DTConfig::OpenFile()
{
   // Log("DTConfig::OpenFile()<");
    if(NULL != m_pFile)
    {
        CloseFile();
    }
    if(true == m_IsRead)
    {
        m_pFile = fopen(m_strFileFullname.c_str(), "r");
    }
    else
    {
        if(NULL == (m_pFile = fopen(m_strFileFullname.c_str(), "r+")))
        {
            m_pFile = fopen(m_strFileFullname.c_str(), "w+");
        }
    }
   // Log("DTConfig::OpenFile()>");
    return (NULL != m_pFile);
}


/************************************************************************
 * Method:     CloseFile
 * FullName:   DTConfig::CloseFile
 * Function:   Close the configuration file
 * Access:
 * Return:     none
 * Qualifier:  none
 * Parameter:
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
void DTConfig::CloseFile()
{
   //  Log("DTConfig::CloseFile()<");
     if(NULL != m_pFile)
     {
         fclose(m_pFile);
         m_pFile = NULL;
     }
    // Log("DTConfig::CloseFile()>");
}

/************************************************************************
 * Method:     GetValue
 * FullName:   DTConfig::GetValue
 * Function:   Get a certain value according to the "strkey" provided
 * Access:
 * Return:     return true if success, or false
 * Qualifier:  none
 * Parameter:  strKey: the key word provided; strValue: store the strKey's value
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
bool DTConfig::GetValue(const string &strkey, string &strValue)
{
   // Log("DTConfig::GetValue()<");

    if(!OpenFile())
    {
      //  Log("DTConfig::GetValue()>-false in OpenFile()");
        return false;
    }

    //Get the line include the strKey
    char szBuf[MAX_PATH];
    string strTemp;
    int nPose;
    while(fgets(szBuf, MAX_PATH-1, m_pFile))
    {
        int i = 0;
        while(MAX_PATH > i)
        {
            if('\n' == szBuf[i++])
            {
                szBuf[--i] = '\0';
                break;
            }
        }
        strTemp = szBuf;
        nPose = strTemp.find(strkey);
        if(-1 == nPose)
        {
            continue;
        }
        else
            break;
    }

    //No line include the strKey
    if(-1 == nPose)
    {
        CloseFile();
        strValue.clear();
        //Log("DTConfig::GetValue()>-false-no strkey find in file");
        return false;
    }

    //strKey exists, delete the blank after strKey
    nPose += strkey.size();
    strTemp = strTemp.substr(nPose, strTemp.size() - nPose);
    string::iterator it  = strTemp.begin();
    while(strTemp.end() != it)
    {
        if(' ' == *it)
        {
            strTemp.erase(it);
        }
        else
            ++it;
    }

    //Delete the "=" between the strKey and strValue
    if('=' == *(strTemp.begin()))
    {
        strTemp.erase(strTemp.begin());
    }

    strValue = strTemp;
    CloseFile();

  //  Log("DTConfig::GetValue()>");
    return true;
}


/************************************************************************
 * Method:     SetValue
 * FullName:   DTConfig::SetValue
 * Function:   Set the value of key
 * Access:
 * Return:     return true if success, or false
 * Qualifier:  none
 * Parameter:  strKey: the key word; strValue: store the strKey's value
 * Record:     2017-06-28  weigaorong    creation
 ************************************************************************/
bool DTConfig::SetValue(const string &strkey, const string &strValue)
{
 //   Log("DTConfig::SetValue()<");
    if(0 == strkey.size())
    {
        //Log("DTConfig::SetValue()>-strKey is NULLL");
        return true;
    }
    string strValueR;
    bool bRet = GetValue(strkey, strValueR);

    //If the existing value of strKey is same with strValue,do nothing
    if(0 == strValue.compare(strValueR))
    {
       // Log("DTConfig::SetValue()>-Value is same, do nothing!");
        return true;
    }

    //If no aim line include strKey, add a line to preserve.
    if((0 == strValue.size()) || (!bRet))
    {
        string strWriteLine = strkey + "=" + strValue + "\n";
        OpenFile();
        fseek(m_pFile, 0L, SEEK_END);
        fputs(strWriteLine.data(), m_pFile);
        CloseFile();
       // Log("DTConfig::SetValue()>-Value is NULL, add a new line.");
    }

    if(!OpenFile())
    {
      //  Log("DTConfig::SetValue()>-OpenFile false");
        return false;
    }

    //Get all lines of the file.
    QStringList strQStringList;
    char szBuf[MAX_PATH];
    string strTemp;
    int nPose;
    while(fgets(szBuf, MAX_PATH-1, m_pFile))
    {
        int i = 0;
        while(MAX_PATH > i)
        {
            if('\n' == szBuf[i++])
            {
                szBuf[i] = '\0';
                break;
            }
        }
        strTemp = szBuf;
        nPose = strTemp.find(strkey);
        if(-1 == nPose)
        {
            strQStringList.append(strTemp.data());
        }
        else
        {
            //Add the arm line
            string strWriteLine = strkey + "=" + strValue + "\n";
            strQStringList.append(strWriteLine.data());
        }
    }
    fclose(m_pFile);
    m_pFile = fopen(m_strFileFullname.data(), "w+");
    while(!strQStringList.isEmpty())
    {
        QString pstring = strQStringList.at(0);
        string StringTemp;
        StringTemp = (const char*)pstring.toLocal8Bit();
        fputs(StringTemp.data(), m_pFile);
        strQStringList.removeAt(0);
    }
    CloseFile();
   // Log("DTConfig::SetValue()>");
    return true;
}


