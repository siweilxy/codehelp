#include <iostream>
#include "json.hpp"
#include <stdlib.h>
#include <fstream>
#include <map>

using json = nlohmann::json;

int getfile(const std::string& path,std::string& result)
{
    char buffer[256];
    std::ifstream in(path);
    if (!in.is_open())
    { std::cout << "Error opening file"; exit (1); }
    while (!in.eof() )
    {
        in.getline (buffer,10000);
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
    for(int i = 0;i<result.length();i++)
    {
        if(result[i] != ' '&&result[i] != ';'&&result[i] != '{'&&result[i] != '}')
        {
            normal = normal +result[i];
        }
        else if(result[i] == ' ')
        {
            if(result[i-1] != ' '&&result[i+1]!= ' ')
            {
                normal = normal +result[i];
            }
        }else if(result[i] == ';')
        {
            normal = normal +result[i];
        }else if(result[i] =='{')
        {
            normal = normal+result[i];
        }else if(result[i] == '}')
        {
            normal = normal +result[i];
        }
    }
    std::cout<<normal<<std::endl;
    std::vector<std::string> words;
    std::string word;
    for(int i = 0;i<normal.size();i++)
    {
        //std::cout<<normal[i]<<std::endl;
        if(normal[i] != ' ')
        {
            if(normal[i] == ';')
            {
                words.push_back(word);
                word.clear();
//                word.push_back(normal[i]);
//                words.push_back(word);
//                word.clear();
            }else if(normal[i] == '{' || normal[i] == '}')
            {
                words.push_back(word);
                word.clear();
                word.push_back(normal[i]);
                words.push_back(word);
                word.clear();
            }
            else
            {
                word.push_back(normal[i]);
            }
        }else
        {
            if(word != " ")
            {
                words.push_back(word);
                word.clear();
            }
        }
    }

    std::vector<std::string> tmpWords;
    for(auto w:words)
    {
        if(w != " "&&w !="")
        {
            tmpWords.push_back(w);
        }
    }

    for(auto tmp:tmpWords)
    {
        //std::cout<<tmp<<std::endl;
    }

    bool start = false;
    int left =0;
    int right =0;
    for(auto tmp:tmpWords)
    {
        if(tmp == "struct")
        {
            start = true;
            outWords.push_back(tmp);
        }else if(start)
        {
            if(tmp == "{")
            {
                left++;
                outWords.push_back(tmp);
            }else if(tmp =="}")
            {
                right++;
                outWords.push_back(tmp);
            }else if(left - right == 0&&left > 0&& right >0)
            {
                break;
            }else
            {
                outWords.push_back(tmp);
            }
        }
    }

    if(left - right != 0)
    {
        std::cout<<"file is error"<<std::endl;
        return false;
    }
    return true;
}

bool add(const std::vector<std::string>& words,p2& p)
{
    for(auto w : words)
    {
    }
    return true;
}

int main()
{
    char * s = getenv("filePath");
    std::string result;
    if(s == nullptr)
    {
        std::cout<<"getenv failed"<<std::endl;
    }
    std::cout<<"path is "<<s<<std::endl;
    getfile(s,result);
    std::cout<<"result is "<<result<<std::endl;
    json j = json::parse(result);
    std::string includePath = j["includepath"];
    std::cout<<includePath<<std::endl;
    result.clear();
    getfile(includePath,result);

    std::vector<std::string> words;
    check(result,words);

    for(auto w:words)
    {
        std::cout<<w<<std::endl;
    }

    p2 pp;
    add(words,pp);

    return 0;
}
