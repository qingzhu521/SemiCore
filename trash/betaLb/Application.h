#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "MyFile.h"

using namespace std;

struct Edge{
	int a;
	int b;
};

struct Node{
	int degree;
	unordered_set<int> nbr;
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
	short* lb;
	int m_maxEdges;
	int m_maxID;
	const static short m_maxCore = 30000;
	int* m_vertexMap;
	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	bool mergeFinished(Edge* es, int size);
	void merge(int size);
	void loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat, unordered_map<int,Node*>& subgraph, int& currentEdges);
	
public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile);
	void semiKCore();
	void printCoreDistribution();
	int imKCore(vector<vector<int>>& graph,short* lb);
};

#endif /* APPLICATION_H_ */