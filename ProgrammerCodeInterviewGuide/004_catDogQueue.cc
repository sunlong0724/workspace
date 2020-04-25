
#include <stdio.h>
#include <string>
#include <queue>
using namespace std;
class Pet{
	public:
		Pet(string type){
			this->type = type;
		}
		string getPetType(){ return type;}

	private:
		std::string type;
};

class Dog : public Pet{
	public:
		Dog():Pet("dog"){}
};

class Cat : public Pet{
	public:
		Cat():Pet("cat"){}
};

class CatDogQueue{

	public:

		void add(Pet* pet){
			q.push(pet);
		}
		void pollAll(){
			while(!q.empty()){
				q.pop();
			}
		}


		void pollDog(){
			pop_push_func("cat");	
		}
		void pollCat(){
			pop_push_func("dog");	
		}
		bool isEmpty(){
			return q.empty();
		}
		bool isDogEmpty(){
			if (pop_func("dog")) return true;
			return false;
		}
		bool isCatEmpty(){
			if (pop_func("cat")) return true;
			return false;
		}
		void printf(){
			fprintf(stdout,"%s",__FUNCTION__); 
			int q_size  = q.size(); 
			while(q_size -- > 0){
				Pet* p = q.front();
				q.pop();
				q.push(p);
				fprintf(stdout,"%s,",p->getPetType().c_str());
			}
			fprintf(stdout,"\n");
		}
	private:
		void pop_push_func(string type){
			int q_size = q.size();
			while(q_size-- > 0){
				Pet* p = q.front();
				q.pop();
				if (p->getPetType() == type) q.push(p);
			}
		}
		bool pop_func(string type){
			int q_size = q.size();
			while(q_size-- > 0){
				Pet* p = q.front();
				q.pop();
				if (p->getPetType() == type) return true;
			}
			return false;
		}
	


	private:
		queue<Pet*> q;
};
int main(int argc,char** argv){
	CatDogQueue cdq;
	for (int i = 0; i < 100; ++i) {
		cdq.add(new Dog);
		cdq.add(new Cat);
	}
	cdq.printf();
	cdq.pollDog();
	cdq.printf();
	cdq.pollCat();
	cdq.printf();

		
	return 0;
}
