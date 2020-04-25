/*************************************************************************
*File Name: 0003_lengthOfLongestSubstring.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月25日 星期六 21时11分02秒
 ************************************************************************/
#include <string>
#include <memory.h>
using namespace std;
int lengthOfLongestSubstring(string s){
    int len = s.size();
    if (len == 0) return 0;
    int max_len = 1;
    int start = 0;

    int c_map[256];
    memset(c_map, -1, sizeof c_map);

    for (int i = 0; i < len; ++i){
        char& c = s[i];
        if (c_map[ c ] != -1){
            if (i - start > max_len){
                max_len = i - start;
            }

            for (int k = start; k < c_map[ c ]; ++k)
                c_map[ s[k] ]=-1;
            start = c_map[ c ] +1;
        }
        c_map[ c ] = i;
    }
    if (len - start > max_len) return len-start;
    return max_len;
}

int main(int argc, char** argv)
{
    string s = "ba";
    fprintf(stdout,"%d \n", lengthOfLongestSubstring( s ));
    return 0;
}

