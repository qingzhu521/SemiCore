#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <queue>

#include "MyFile.h"

using namespace std;

struct Edge{
	int a;
	int b;
};

struct Vertex{
	int id;
	int core;
	vector<int> nbr;
	Vertex* next;
	Vertex* previous;
};

class Application{
private:
	string m_idx;
	string m_dat;
	string m_base;
	string m_info;
	int m_m;
	int m_maxDegree;
	short* m_ub;
	short* m_lb;

	int m_maxID;
	short m_maxCore;
	int* m_vertexMap;
	int* m_degree;

	long m_restEdges;

	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	bool mergeFinished(Edge* es, int size);
	void merge(int size);
	void loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat, long& currentEdges, Vertex** imGraph, Vertex** imCore, vector<int>& imVertices, int& firstU, int& firstV);

	void imKCore(Vertex** imGraph, Vertex** imCore, vector<int>& imVertices);
	
public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile);
	void semiKCore();
	void printCoreDistribution();
};

#endif /* APPLICATION_H_ */