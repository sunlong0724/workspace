#include <stack>
#include <stdio.h>
using namespace std;
class GetMinStack{
	public:
		void push(int val){
			s.push(val);
			if (min_s.empty() || val < min_s.top()){
				min_s.push(val);
			}else{
				min_s.push(min_s.top());
			}
		}
		int pop(){
			if (s.empty()) return -1;
			int ret = s.top();
			s.pop();
			min_s.pop();
			return ret;
		}
		int get_min(){
			if (min_s.empty()) return -1;
			return min_s.top();
		}
		bool empty(){
			return s.empty();
		}
	//private:
		stack<int> s,min_s;

};
int main(int argc, char** argv){
	GetMinStack gms;
	for (int i = 0; i < 100; ++i) gms.push(i);
	int ms = gms.min_s.size();
	int ss = gms.s.size();
	fprintf(stdout,"%u,%u\n", ms,ss);
	while(!gms.empty()){
		//fprintf(stdout,"{%d, %d},",gms.get_min(),gms.pop());  !!!!get_min(),pop() as argument effect calling order!!!	
		fprintf(stdout,"{%d, %d},",gms.pop(),gms.get_min());	
	}
	fprintf(stdout,"\n");
	return 0;
}
