#include <iostream>
#include "LinkedList.h"
#include "Vector.hpp"
#include <chrono>

using namespace std;
using namespace aisdi;

template <typename Fun>
auto measureTime(Fun f)
{
	auto start = std::chrono::system_clock::now();

	f();

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	return elapsed;
}


int main(){
		
		Vector<int> v1;
		v1.insert(v1.begin(), 1);
		
		auto appendVector = []{
			Vector<int> v1;
			for(int i= 0; i < 100'000; i++)
				v1.append(i);
		};
		
		auto appendList = []{
			LinkedList<int> l1;
			for(int i= 0; i < 100'000; i++)
				l1.append(i);
		};
		
		cout<<"Appending 100 000 elements to vector took " << measureTime(appendVector).count()<<endl;
		cout<<"Appending 100 000 elements to list took " << measureTime(appendList).count()<<endl;
		
		
		auto popLastVector = []{
			Vector<int> v1;
			for(int i= 0; i < 100'000; i++)
				v1.append(i);
				
			for(int i= 0; i < 100'000; i++)
				v1.popLast();
		};
		
		auto popLastList = []{
			LinkedList<int> l1;
			for(int i= 0; i < 100'000; i++)
				l1.append(i);
				
			for(int i= 0; i < 100'000; i++)
				l1.append(i);
		};
		
		cout<<"Popping last 100 000 elements from vector took " << measureTime(popLastVector).count()<<endl;
		cout<<"Popping last  100 000 elements from list took" << measureTime(popLastList).count()<<endl;
		

		
		

		
	
		
		auto popFirstVector = []{
			Vector<int> v1;
			for(int i= 0; i < 100'000; i++)
				v1.append(i);
			for(int i= 0; i < 100'000; i++)
				v1.popFirst();
		};
		
		auto popFirstList = []{
			LinkedList<int> l1;
			for(int i= 0; i < 100'000; i++)
				l1.append(i);
			
			for(int i= 0; i < 100'000; i++)
				l1.popFirst();
		};
		
		cout<<"Popping fist 100 000 elements from vector took " << measureTime(popFirstVector).count()<<endl;
		cout<<"Popping first 100 000 elements from list took " << measureTime(popFirstList).count()<<endl;
		
		auto popMiddleVector = []{
			Vector<int> v1;
			for(int i= 0; i < 100'000; i++)
				v1.append(i);
			for(int i= 0; i < 49'000; i++)
				v1.erase(v1.begin() + 50'000);
		};
		
		auto popMiddleList = []{
			LinkedList<int> l1;
			for(int i= 0; i < 100'000; i++)
				l1.append(i);
			
			for(int i= 0; i < 49'000; i++)
				l1.erase(l1.begin() + 50'000);
		};
		
		cout<<"Popping middle 49 000 elements from vector took " << measureTime(popFirstVector).count()<<endl;
		cout<<"Popping middle 49 000 elements from list took " << measureTime(popFirstList).count()<<endl;
		
		
		return 0;
		
}
