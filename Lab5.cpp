#include "stdafx.h"
#include <memory>
#include <cstdlib>
#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <stack>
#include <iomanip>
#include <map>

using namespace std;
static const int size = 4;

// insert your memory manager here
// Start of My Code

// Allocate
// Serves as custom allocation/de-allocation for the maps in the MemoryManager Class
template <typename V>
class Allocate {
public:
	typedef V value_type;
	Allocate () {};
	template <typename V>
	Allocate (const Allocate <V>& a) {};
	V* allocate(size_t s) 
	{
		return static_cast<V*>(malloc(s * sizeof(V)));
	};

	void deallocate(V* pointer, size_t s) 
	{
		free(pointer);
	};
};

// MemoryManager Class
// In charge of recording memory used within the program using an int to keep track of memory left allocated and 
// maps to record pointers made in the program
class MemoryManager {
private:
	int memory_left_allocated = 0;
	map <void*, size_t, less<void*>, Allocate<pair<void*, size_t>>> allocatedMemoryMap;
	map <void**, size_t, less<void**>, Allocate<pair<void**, size_t>>> deallocatedMemoryMap;
public:
	MemoryManager() {};
	void* allocateMemory(size_t s);
	void deleteMemory(void* p, size_t s);
	void deleteMemory(void* p);
	void printMemoryUsage();
};

// Global Instance of the MemoryManager Class
MemoryManager memory_supervisor;

void* operator new (size_t st)
{
	return memory_supervisor.allocateMemory(st);
}

void operator delete(void* p, size_t s)
{
	memory_supervisor.deleteMemory(p, s);
}

void operator delete(void* p) {
	memory_supervisor.deleteMemory(p);
}

// End of my code
class Student
{
	string* _firstname;
	string* _lastname;
	string* _course;
	double* _gpa;
public:
	Student()
	{
		_course = NULL;
		_lastname = NULL;
		_firstname = NULL;
		_gpa = NULL;
	}

	Student(char* pc, char* pl, char* pf, char* pg)
	{
		_course = new string(pc);
		_lastname = new string(pl);
		_firstname = new string(pf);
		_gpa = new double(atof(pg));
	}

	Student(string* pitems)
	{
		_course = new string(pitems[0]);
		_lastname = new string(pitems[1]);
		_firstname = new string(pitems[2]);
		_gpa = new double(atof(pitems[3].c_str()));
	}

	Student(const vector<string>& vs)
	{
		if (_course)
		{
			_course = new string(vs[0]);
			_lastname = new string(vs[1]);
			_firstname = new string(vs[2]);
			_gpa = new double(atof(vs[3].c_str()));
		}
	}

	Student(const Student& s)
	{
		_course = NULL;
		_lastname = NULL;
		_firstname = NULL;
		_gpa = NULL;
		if (s._course)
		{
			_course = new string(*s._course);
			_lastname = new string(*s._lastname);
			_firstname = new string(*s._firstname);
			_gpa = new double(*s._gpa);
		}
	}

	Student(Student&& s)
	{
		_course = s._course; s._course = NULL;
		_lastname = s._lastname; s._lastname = NULL;
		_firstname = s._firstname; s._firstname = NULL;
		_gpa = s._gpa; s._gpa = NULL;
	}

	~Student()
	{
		if (_course) delete _course;
		if (_lastname) delete _lastname;
		if (_firstname) delete _firstname;
		if (_gpa) delete _gpa;
	}
	string GetFirstName() { return *_firstname; }
	string GetLastName() { return *_lastname; }
	string GetCourseName() { return *_course; }
	double GetGpa() { return *_gpa; }
	void operator = (const Student& right)
	{
		if (_course)
		{
			_course = new string(const_cast<Student&>(right).GetCourseName());
			_lastname = new string(const_cast<Student&>(right).GetLastName());
			_firstname = new string(const_cast<Student&>(right).GetFirstName());
			_gpa = new double(const_cast<Student&>(right).GetGpa());
		}
	}

	friend ostream& operator << (ostream& os, Student& s)
	{
		os << s.GetCourseName();
		os << '\t';
		os << s.GetLastName();
		os << '\t';
		os << s.GetFirstName();
		os << '\t';
		os << s.GetGpa();
		return os;
	}
};

template <typename T>
T RegexString(string text, string split)
{
	T container;
	sregex_token_iterator end; // size is ZERO when created
	regex pattern(split);
	for (sregex_token_iterator pos(text.begin(), text.end(), pattern); pos != end; ++pos)
	{
		if ((*pos).length() > 0)
		{
			if ((static_cast<string>(*pos))[0] != 0x20) // 0x20 = BLANK
				container.push_back(*pos);
		}
	}
	return container;
}

template <typename T>
int ReadFile(T& container, string sfilename, string delimiter)
{
	vector<string> vs;
	ifstream filein(sfilename);
	while (!filein.eof())
	{
		string buffer;
		getline(filein, buffer);
		vs = RegexString<vector<string>>(buffer, delimiter);
		if (vs.size() > 0)
			container.push_back(unique_ptr<Student>(new Student(vs)));
		vs.clear();
	}
	filein.close();
	return container.size();
}

// Main Function
void main()
{
	{
		vector<unique_ptr<Student>> vs;
		int size = ReadFile(vs, "c:/Users/Vincent Ha/Downloads/Students.csv", "[^,]*");
		for (int x = 0; x < vs.size(); x++) {
			cout << vs[x]->GetFirstName() << '\t' << vs[x]->GetLastName() << endl;
		}
		cout << endl;
 	}
	memory_supervisor.printMemoryUsage();
}

// Function Definitions 
// MemoryManager Class
void* MemoryManager::allocateMemory(size_t s) {
	void* p = malloc(s);
	allocatedMemoryMap[p] = s;
	memory_left_allocated += s;
	return p;
}

void MemoryManager::deleteMemory(void* p, size_t s){
	map<void*, size_t>::iterator iter = allocatedMemoryMap.find(p);
	if (iter != allocatedMemoryMap.end()) 
		allocatedMemoryMap.erase(iter);
	deallocatedMemoryMap[&p] = s;
	memory_left_allocated -= s;
	free(p);
}

void MemoryManager::deleteMemory(void* p) {
	map<void*, size_t>::iterator iter = allocatedMemoryMap.find(p);
	if (iter != allocatedMemoryMap.end()) {
		deallocatedMemoryMap[&p] = iter->second;
		memory_left_allocated -= iter->second;
		allocatedMemoryMap.erase(iter);
	}
	free(p);
}

void MemoryManager::printMemoryUsage() {
	cout << "Memory Used" << endl;
	cout << "-----------" << endl << endl;
	cout << "Memory Address" << setw(30) << "Amount De-Allocated (bytes)" << endl;
	cout << "--------------" << setw(30) << "---------------------------" << endl;
	map<void**, size_t>::iterator iter = deallocatedMemoryMap.begin();
	while (iter != deallocatedMemoryMap.end()) {
		cout << iter->first << setw(23) << iter->second << endl;
		iter++;
	}
	cout << endl;
	cout << "Memory Left Allocated" << endl;
	cout << "---------------------" << endl;
	cout << endl;
	cout << "Memory Address" << setw(35) << "Amount Left Allocated (bytes)" << endl;
	cout << "--------------" << setw(35) << "-----------------------------" << endl;
	map<void*, size_t>::iterator iter2 = allocatedMemoryMap.begin();
	while (iter2 != allocatedMemoryMap.end()) {
		cout << iter2->first << setw(27) << iter2->second << endl;
		iter2++;	
	}
	cout << endl;
	cout << "Amount of memory not deallocated: " << memory_left_allocated << endl;
};