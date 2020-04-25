#include <stack>
#include <stdio.h>
using namespace std;

class Solution{
	public:
		void reverseStack(stack<int>& s){
			if (s.empty()) return;
			int ret = s.top();
			s.pop();
			reverseStack(s);
			stack<int> tmp;
			while(!s.empty()){
				tmp.push(s.top());
				s.pop();	
			}

			s.push(ret);
			while(!tmp.empty()){
				s.push(tmp.top());
				tmp.pop();
			}
		}
		//NOT GOOD!
		int reverse(stack<int> &s){
			int ret = s.top();
			s.pop();
			if (s.empty()){
				return ret;
			}else{
				int last = reverse(s);
				s.push(ret);
				return last;
			}
		}
};

int main(int argc, char** argv){
	
	Solution sol;
	stack<int> s, s2;
	for (int i = 0; i < 100; ++i) s.push(i);
	s2 = s;
	while(!s2.empty()){
		fprintf(stdout,"%d,", s2.top());
		s2.pop();
	}	
	fprintf(stdout,"\n*******************************************************************************************************\n");
    sol.reverseStack(s);
	//sol.reverse(s);
    while(!s.empty()){
		fprintf(stdout,"%d,", s.top());
		s.pop();
	}	
	fprintf(stdout,"\n");

	return 0;
}
