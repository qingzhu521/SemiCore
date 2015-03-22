#include <cstdio>
#include <cstring>

using namespace std;

class Application{
private:
	char* m_idx;
	char* m_dat;
	char* m_base;

	int* m_vertexMap;
	int getVertexID(int originID);
	
public:
	Application();
	Application(char* base);
	~Application();
	void sortEdge(char* txtFile,int maxID);
}