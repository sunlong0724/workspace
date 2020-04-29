/*************************************************************************
*File Name: 0076_minWindow.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月25日 星期六 21时43分41秒
 ************************************************************************/
#include <string>
#include <unordered_map>
using namespace std;
string minWindow(string s, string t){

    unordered_map<char,int> t_map, s_map;
    for(auto& c : t) t_map[c]++;
    int len = s.size();
    int left = 0, right = 0, start = 0;
    int matches = 0, min_len = s.size();
    while(right < len){
        char& c = s[ right ];
        if (t_map.count(c)) {
            s_map[ c ]++;
            if (s_map[ c ] == t_map[ c ]) ++matches;
        }

        ++right;
        if (matches >= t_map.size()){
            if (right - left < min_len){
                min_len = right - left;
                start = left;
            }
            if (--s_map[ s[left] ] == 0){
                s_map.erase( s[left] );
                ++left;
                while( left < right && t_map.count( s[left] ) == 0 ) {
                    if (--s_map[ s[left] ] == 0) {
                        s_map.erase( s[left] );
                    }
                    ++left;
                }  
            }
            --matches;
        }        
    }
    return min_len == s.size() ? "" : s.substr(start, min_len);
    
}
int main(int argc, char** argv)
{
    string s = "ADOBECODEBANC";
    string t = "ABC";

    string min_str = minWindow(s,t);
    fprintf(stdout,"###%s\n", min_str.c_str());
    return 0;
}

