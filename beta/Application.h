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

	int* m_vertexMap;
	int getVertexID(int u,int& num);
	void saveTmpEdges(Edge* edges,int size,int tmpFile);
	bool static edgeCompare(const Edge &e0, const Edge &e1);
	int min(Edge* es, int size);
	
public:
	Application();
	Application(string base);
	~Application();
	void sortEdge(string txtFile);
};

