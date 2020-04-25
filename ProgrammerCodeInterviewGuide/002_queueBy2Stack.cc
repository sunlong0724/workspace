#include <stack>
#include <stdio.h>
using namespace std;
class Queue{
	public:
	int front(){
		shift();
		if (!s2.empty()) return s2.top();
		else return -1;
	}
	void push(int val){
		s1.push(val);
	}
	void pop(){
		shift();
		if (!s2.empty()) s2.pop();
	}
	bool empty(){
		return s1.empty() && s2.empty();
	}

	private:
		void shift(){
			if (s2.empty()){
				while(!s1.empty()){
					s2.push(s1.top());
					s1.pop();
				}
			}
		}

	private:
		stack<int> s1,s2;
};

int main(int argc,char** argv){

	Queue q;
	for (int i = 0; i < 100; ++i) q.push(i);
	while(!q.empty()){
		fprintf(stdout,"%d,", q.front());
		q.pop();
	}
	fprintf(stdout,"\n");

	return 0;
}
