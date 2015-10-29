#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <sys/time.h>
#include <cstdlib>

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
	unsigned short* ub;
	unsigned short* cnt;

	vector<int>** m_dynamicAdd;
	unordered_set<int>** m_dynamicDel;

	bool* m_addBit;
	bool* m_delBit;

	long m_addIO;
	long m_delIO;


	int m_maxID;
	const static unsigned short m_maxCore = 60000;
	int* m_vertexMap;
	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	bool mergeFinished(Edge* es, int size);
	void merge(int size);
	void loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat);
	void loadNbrForDynamic(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat);
	int selectNbr(int a);

	bool isNbr(int a, int b);

public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile, int scale, bool byEdge);
	void semiKCore();
	void semiKCoreNaive();
	void semiKCoreNaivePlus();
	void printCoreDistribution();
	void addEdge(int a, int b);
	void addEdgeNaive(int a, int b);

	void removeEdge(int a, int b);
	void dynamicCore(int num);
	void saveCore();
	void loadCore();
	void getGraphInfo();

};

#endif /* APPLICATION_H_ */