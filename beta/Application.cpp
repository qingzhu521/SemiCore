#include "Application.h"

Application::Application(string base){
	m_base = base;
	m_dat = base+"graph.dat";
	m_idx = base+"graph.idx"
}
Application::Application(){

}
Application::~Application(){

}

void Application::sortEdge(string txtFile){
	printf("Start... txtFile: %s\n",txtFile.c_str());
	FILE* fp = fopen(txtFile.c_str(),"r");

	int maxID = 1000000000;
	int memEdges = 100000000;
	Edge* edges = new Edge[memEdges];


	m_vertexMap = new int[maxID];
	memset(m_vertexMap,-1,sizeof(int)*maxID);
	// save every line read from txtFile
	char line[100];

	int u,v;

	int size = 0,num = 0,tmpFile = 0;
	printf("Separating: \n");
	while(fgets(line,100,fp)){
		
		if( line[0] < '0' || line[0] > '9' )
			continue;

		sscanf(line,"%d,%d",&u,&v);

		if(u == v)
			continue;

		u = getVertexID(u,num);
		v = getVertexID(v,num);


		edges[size].a = u;
		edges[size].b = v;
		++size;

		edges[size].a = v;
		edges[size].b = u;
		++size;

		if(size>=memEdges){
			printf("load %d edges\n",size);
			saveTmpEdges(edges,size,tmpFile);
			size = 0;
			++tmpFile;
		}

	}
	printf("load %d edges\n",size);
	saveTmpEdges(edges,size,tmpFile);


	delete[] m_vertexMap;
	delete[] edges;

	fclose(fp);

}

bool Application::edgeCompare(const Edge &e1, const Edge &e2){
	if(e1.a < e2.a){
		return true;
	}
	if( e1.a > e2.a ){
		return false;
	}
	return e1.b < e2.b;
}

void Application::saveTmpEdges(Edge* edges, int size, int tmpFile){
	printf("sort edges...\n");
	sort(edges,edges+size,edgeCompare);
	char fileName[200];
	sprintf(fileName,"%ssort_edge_tmp/tmp_%d",m_base.c_str(),tmpFile);
	printf("Creating tmp file %d: %s\n",tmpFile,fileName);

	FILE* fo = fopen(fileName,"wb");
	for (int i = 0; i < size; ++i){
		fwrite( edges+i, sizeof(Edge), 1, fo );
	}
	fclose(fo);
	
}

void Application::merge(int tmpFile){
	FILE** frl = new FILE*[tmpFile+1];
	Edge* es = new Edge[tmpFile+1];
	char fileName[200];
	for (int i = 0; i <= tmpFile ; ++i){
		sprintf(fileName,"%ssort_edge_tmp/tmp_%d",m_base.c_str(),tmpFile);
		
		frl[i] = fopen(fileName,"r");
		fread(&es[i],sizeof(Edge),1,frl[i]);
	}

	int minIndex;
	minIndex = min(es,tmpFile+1);

	


	for (int i = 0; i <= tmpFile ; ++i){
		fclose(frl[i]);
	}
	delete[] frl;
}

int Application::min(Edge* es,int size){
	int min = 0;
	for (int i = 1; i < size; ++i){
		if(es[i].a < es[min].a){
			min = i;
			continue;
		}else if(es[i].a > es[min].a){
			continue;
		}else if(es[i].b < es[min].b){
			min = i;
		}else{
			continue;
		}
	}
	return min;
}

int Application::getVertexID(int u,int &num){
	if(m_vertexMap[u]<0){
		m_vertexMap[u] = num++;
	}
	return m_vertexMap[u];
}
