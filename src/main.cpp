#include <iostream>
#include "json.hpp"
#include <stdlib.h>
#include <fstream>
#include <map>
#include <mysql/mysql.h>

using json = nlohmann::json;

#define SELECT 1
#define UPDATE 2
#define INSERT 3

static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

int getfile(const std::string& path, std::string& result)
{
    result.clear();
    char buffer[256];
    std::ifstream in(path);
    if (!in.is_open())
    {
        std::cout << "Error opening file";
        exit(1);
    }
    while (!in.eof())
    {
        in.getline(buffer, 10000);
        result = result + buffer;
    }
    return true;
}

struct parse
{
    std::string type;
    std::string name;
};

struct p2
{
    std::vector<parse> parses;
    std::vector<p2> ps;
};

bool check(const std::string& result, std::vector<std::string>& outWords)
{
    std::string normal;
    for (int i = 0; i < result.length(); i++)
    {
        if (result[i] != ' ' && result[i] != ';' && result[i] != '{'
                && result[i] != '}')
        {
            normal = normal + result[i];
        } else if (result[i] == ' ')
        {
            if (result[i - 1] != ' ' && result[i + 1] != ' ')
            {
                normal = normal + result[i];
            }
        } else if (result[i] == ';')
        {
            normal = normal + result[i];
        } else if (result[i] == '{')
        {
            normal = normal + result[i];
        } else if (result[i] == '}')
        {
            normal = normal + result[i];
        }
    }
    std::cout << normal << std::endl;
    std::vector<std::string> words;
    std::string word;
    for (int i = 0; i < normal.size(); i++)
    {
        //std::cout<<normal[i]<<std::endl;
        if (normal[i] != ' ')
        {
            if (normal[i] == ';')
            {
                words.push_back(word);
                word.clear();
//                word.push_back(normal[i]);
//                words.push_back(word);
//                word.clear();
            } else if (normal[i] == '{' || normal[i] == '}')
            {
                words.push_back(word);
                word.clear();
                word.push_back(normal[i]);
                words.push_back(word);
                word.clear();
            } else
            {
                word.push_back(normal[i]);
            }
        } else
        {
            if (word != " ")
            {
                words.push_back(word);
                word.clear();
            }
        }
    }

    std::vector<std::string> tmpWords;
    for (auto w : words)
    {
        if (w != " " && w != "")
        {
            tmpWords.push_back(w);
        }
    }

    for (auto tmp : tmpWords)
    {
        //std::cout<<tmp<<std::endl;
    }

    bool start = false;
    int left = 0;
    int right = 0;
    for (auto tmp : tmpWords)
    {
        if (tmp == "struct")
        {
            start = true;
            outWords.push_back(tmp);
        } else if (start)
        {
            if (tmp == "{")
            {
                left++;
                outWords.push_back(tmp);
            } else if (tmp == "}")
            {
                right++;
                outWords.push_back(tmp);
            } else if (left - right == 0 && left > 0 && right > 0)
            {
                break;
            } else
            {
                outWords.push_back(tmp);
            }
        }
    }

    if (left - right != 0)
    {
        std::cout << "file is error" << std::endl;
        return false;
    }
    return true;
}

bool add(const std::vector<std::string>& words, p2& p)
{
    int structs = 0;
    for (int i = 0; i < words.size(); i++)
    {
        if (words[i] == "struct")
        {
            structs++;
            if (structs == 1)
            {
                std::cout << "first structs" << std::endl;
            } else
            {
                std::cout << "include structs" << std::endl;
            }
        }
    }
    return true;
}

typedef struct dataType_s
{
    std::string type;
    int length;
} dataType_t;

typedef struct sqlParse_s
{
    int type;
    std::vector<std::string> selectField;
    std::vector<std::string> updateField;
    std::vector<std::string> insertField;
    std::string table;
    std::vector<std::string> conditions;
} sqlParse_t;

int parseSelect(const std::string& result, sqlParse_t& sqlparse)
{
    std::cout << "start parse select" << std::endl;

    auto start = result.find("select");
    start = start + 6;
    auto from = result.find("from");
    std::cout<<"from:"<<from<<std::endl;
    std::string fds = result.substr(start, from-start);
    std::cout<<fds<<std::endl;
    int p = 0;
    int fp = 0;
    int end = 0;
    while(!fds.empty()){
        fp = -1;
        fp = fds.find(",");
        if(fp == -1)
        {
            end = 1;
        }
        auto f = fds.substr(0, fp);
        std::cout<<"f is "<<f<<std::endl;
        p = fp + 1;
        fds = fds.substr(p, fds.length()- p );
        sqlparse.selectField.push_back(trim(f));
        if(end == 1)
        {
            break;
        }
        //std::cout<<"fds is "<<fds<<std::endl;
    }

    return 0;
}

int parseUpdate(const std::string& result, sqlParse_t& sqlparse)
{
    std::cout << "start parse update" << std::endl;
    return 0;
}

int parseInsert(const std::string& result, sqlParse_t& sqlparse)
{
    std::cout << "start parse insert" << std::endl;
    return 0;
}

int main()
{
    std::map<std::string, dataType_t> dataFields;
    sqlParse_t sqlparse;
    char * s = getenv("filePath");
    int ret = 0;
    std::string result;
    if (s == nullptr)
    {
        std::cout << "getenv failed" << std::endl;
    }
    std::cout << "path is " << s << std::endl;
    getfile(s, result);
    std::cout << "result is " << result << std::endl;
    json j = json::parse(result);
    std::string includePath = j["includepath"];
    std::string ip = j["dbip"];
    std::string port = j["dbport"];
    std::string user = j["dbusrname"];
    std::string pwd = j["dbpasswd"];
    std::string sqlpath = j["sqlpath"];
    std::string src = j["src"];
    std::string dst = j["dst"];
    std::string dbName = j["dbname"];
    std::string tableName = j["tablename"];

    std::string sql;
    std::string fd, fdType, fdLength, nativeStr;

    std::cout << "includePath:" << includePath << std::endl;
    std::cout << "ip:" << ip << std::endl;
    std::cout << "port:" << port << std::endl;
    std::cout << "usr:" << user << std::endl;
    std::cout << "pwd:" << pwd << std::endl;
    //std::cout<<"sql:"<<sql<<std::endl;
    std::cout << "src:" << src << std::endl;
    std::cout << "dst:" << dst << std::endl;
    std::cout << "dbname:" << dbName << std::endl;
    std::cout << "tablename:" << tableName << std::endl;
    std::cout << "sqlpath:" << sqlpath << std::endl;

    MYSQL_RES *my_res;    //查询结果
    MYSQL_FIELD *my_field;  //结果中字段信息
    MYSQL_ROW my_row;    //结果中数据信息
    int cols = 0;
    unsigned long *lengths;

    sql = "desc " + tableName;

    MYSQL mysql;
    MYSQL * connect = NULL;
    connect = mysql_init(&mysql);
    if (connect == NULL)
    {
        ret = mysql_errno(&mysql);
        printf("mysql_init error, %s\n", mysql_error(&mysql));
        return ret;
    }
    connect = mysql_real_connect(connect, ip.c_str(), user.c_str(), pwd.c_str(),
            dbName.c_str(), 0, NULL, 0);
    if (connect == NULL)
    {
        ret = mysql_errno(&mysql);
        printf("mysql_real_connect error, err is: %s\n", mysql_error(&mysql));
        return ret;
    }

    if (mysql_query(connect, sql.c_str()))
    {
        printf("mysql_query[%d] [%s]!\n", mysql_errno(connect),
                mysql_error(connect));
        return -1;
    }
    my_res = mysql_store_result(&mysql);
    my_field = mysql_fetch_fields(my_res);
    cols = mysql_num_fields(my_res);

    while (my_row = mysql_fetch_row(my_res))
    {
        //printf("name is %s,len is %s",my_row[0],my_row[1]);
        nativeStr = my_row[1];
        fd = my_row[0];
        auto pos = nativeStr.find("(");
        fdType = nativeStr.substr(0, pos);
        fdLength = nativeStr.substr(pos + 1, nativeStr.length() - 1);
        //std::cout<<"fd:"<<fd<<" fdType:"<<fdType<<" fdLength:"<<fdLength<<std::endl;
        dataFields[fd]=
        {   fdType,atoi(fdLength.c_str())};
    }
//        for(auto iter:dataFields)
//        {
//            std::cout<<iter.first<<" "<<iter.second.type<<" "<<iter.second.length<<std::endl;
//        }

    getfile(sqlpath, result);
    std::cout << result << std::endl;
    int pos = -1;
    pos = result.find("select");
    if (pos == -1)
    {
        pos = result.find("insert");
        if (pos == -1)
        {
            pos = result.find("update");
            if (pos == -1)
            {
                std::cout << "type error" << std::endl;
                return 0;
            } else
            {
                sqlparse.type = UPDATE;
                std::cout << "type is update" << std::endl;
            }
        } else
        {
            sqlparse.type = INSERT;
            std::cout << "type is insert" << std::endl;
        }
    } else
    {
        sqlparse.type = SELECT;
        std::cout << "type is select" << std::endl;
    }

    switch (sqlparse.type)
    {
    case UPDATE:
        parseUpdate(result,sqlparse);
        break;
    case INSERT:
        parseInsert(result,sqlparse);
        break;
    case SELECT:
        parseSelect(result,sqlparse);
        for(auto iter:sqlparse.selectField)
        {
            std::cout<<iter<<"."<<std::endl;
        }
        break;
    default:
        std::cout << "error" << std::endl;
        return 0;
    }

    return 0;
}
