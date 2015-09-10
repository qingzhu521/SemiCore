#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <sys/time.h>

#include "MyFile.h"

using namespace std;

struct Edge{
	int a;
	int b;
};

class Application{
private:
	string m_idx;
	string m_dat;
	string m_base;
	string m_info;
	int m_m;
	int m_maxDegree;
	short* ub;
	short* cnt;

	unordered_set<int>** m_dynamic;



	int m_maxID;
	const static short m_maxCore = 30000;
	int* m_vertexMap;
	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	bool mergeFinished(Edge* es, int size);
	void merge(int size);
	void loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat);
	
public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile);
	void semiKCore();
	void printCoreDistribution();
	void addEdge(int a, int b);
	void removeEdge(int a, int b);
};

#endif /* APPLICATION_H_ */