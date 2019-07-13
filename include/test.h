#include <string> 

struct stu
{
    char name[8];
    int num;  
    int age;  
    char group;  
    float score;  
    struct familiy
    {
      char name[8];
      int num;
      int age;
    };
};
