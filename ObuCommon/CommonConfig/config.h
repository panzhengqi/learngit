#ifndef CONFIG_H
#define CONFIG_H

#include<string>

using namespace std;

class DTConfig
{
public:
    DTConfig(string strFileFullname, bool IsRead);
    ~DTConfig();

    bool OpenFile();
    void CloseFile();

    bool GetValue(const string& strkey, string& strValue);
    bool SetValue(const string& strkey, const string& strValue);

private:
    DTConfig();
    DTConfig(DTConfig&);
    DTConfig& operator=(const DTConfig&);
private:
    string     m_strFileFullname;    //the full path of the config file
    bool       m_IsRead;             //open mode
    FILE      *m_pFile;              //handle of the config file
};

#endif // CONFIG_H
