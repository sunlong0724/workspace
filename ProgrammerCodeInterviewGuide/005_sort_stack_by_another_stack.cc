#include <stdio.h>
#include <stack>
using namespace std;

class Solution{
	public:
		void sort_stack(stack<int>& s){
			stack<int> tmp;

			while(!s.empty()){
				int cur = s.top();
				s.pop();
				while (!tmp.empty() &&  cur < tmp.top()){
					s.push(tmp.top());
					tmp.pop();
				}
				tmp.push(cur);
			}

			swap(s,tmp);
		}

};

int main(int argc,char** argv){
	Solution sol;
	stack<int> s,s2;
	for (int i = 100; i > 0; --i) s.push(i);
	s2 = s;

	sol.sort_stack(s);

	while(!s.empty()){
		fprintf(stdout,"{%d,%d},",s.top(),s2.top());
		s2.pop();
		s.pop();
	}
	return 0;
}
