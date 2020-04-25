/*************************************************************************
*File Name: 0046_permute.cc
*Author: sunlong0724
*sunlong0724@sina.com
*Created Time: 2020年04月25日 星期六 10时34分32秒
 ************************************************************************/

#include <vector>
#include <functional>
#include <stdio.h>

using namespace std;

vector< vector<int> > permute(vector<int>& nums){
    //1.依次递归固定每一位，一条递归链结束时回溯当前位，固定下一位。
    vector< vector<int> > res;
    vector< bool > visited(nums.size(), false);
    vector< int > cur;

    function<void (vector<int>&)> dfs = [&dfs, &res, &visited, &nums](vector<int>& cur){
        if (cur.size() == nums.size()){
            res.push_back(cur);
            return;
        }

        for (int i = 0; i < nums.size(); ++i){
            if ( visited[i] ) continue;
            visited[i] = true;
            cur.push_back( nums[i] );
            dfs(cur);
            cur.pop_back(); 
            visited[i] = false;
        }
    };

    dfs(cur);
    return res;
}

int main(int argc, char** argv)
{
    vector<int> nums{1,2,3,4,5,6};
    vector< vector<int> > res = permute( nums );
    for (auto v : res){
        fprintf(stdout,"\n");
        for (auto i : v){
            fprintf(stdout,"%d,",i);
        }
    }
    return 0;
}

