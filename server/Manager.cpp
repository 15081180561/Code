#include "Manager.h"
using namespace std;

void Manager::vec2string(vector<float> &feature,string &f_data,int size)
{
    std::stringstream ss;
    for (int i = 0; i < size; i++)
    {
        feature.push_back(i);
    }

    for (size_t i = 0; i < feature.size(); ++i)
    {
        if (i != 0)
        {
            ss<<" ";
            ss<<feature[i];
        }
        f_data=ss.str();
        ss.str("");
    }   
}
