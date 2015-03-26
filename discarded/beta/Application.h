#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>

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
	short m_maxDegree;

	int m_maxID;

	int* m_vertexMap;
	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	bool mergeFinished(Edge* es, int size);
	void merge(int size);
	void loadNbr(int u, int* nbr, short& degree, FILE* fIdx, FILE* fDat);
	
public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile);
	void semiKCore();
	void printCoreDistribution(short* ub);
};

