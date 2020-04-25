#include <set>
#include <vector>
#include <stdio.h>
using namespace std;

class Solution{
	public:
		vector<int> getMaxWindow(vector<int>& v, int k){
			vector<int> ret;
			multiset<int,std::greater<int>> s;
			for (int i = 0; i < v.size(); ++i){
				s.insert(v[i]);
				if (s.size() > k){
					s.erase(s.find(v[i-k]));//mutilset/mutimap can use iterator to delete a single element!!!
				} 
				if (s.size() == k) 
					ret.push_back(*s.begin());
			}
			return ret;

		}
};

int main(int argc, char** argv){
	Solution sol;
	vector<int> v = {2,3,1,3,4,5,12,21,6,73,4,5,6,12,3,4,5,9,8,77,65,4,543};
	auto&& ret = sol.getMaxWindow(v,3);
	for(auto x: ret) fprintf(stdout,"%d,",x);
	return 0;
}
