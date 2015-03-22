#include "Application.h"

Application::Application(string base){
	m_base = base;
	m_dat = base+"graph.dat";
	m_idx = base+"graph.idx";
}
Application::Application(){

}
Application::~Application(){

}

void Application::sortEdge(string txtFile){
	printf("Start... txtFile: %s\n",txtFile.c_str());

	m_maxID = 1000000000;
	int memEdges = 100000000;

	merge(2);
	return;
	FILE* fp = fopen(txtFile.c_str(),"r");

	


	Edge* edges = new Edge[memEdges];


	m_vertexMap = new int[m_maxID];
	memset(m_vertexMap,-1,sizeof(int)*m_maxID);

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

	merge(tmpFile+1);

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

void Application::merge(int size){
	FILE** frl = new FILE*[size];
	Edge* es = new Edge[size];

	FILE* fIdx = fopen(m_idx.c_str(),"wb");
	FILE* fDat = fopen(m_dat.c_str(),"wb");

	fpos_t pos;
	


	for (int i = 0; i < size ; ++i){
		char fileName[200];
		sprintf(fileName,"%ssort_edge_tmp/tmp_%d",m_base.c_str(),i);
		
		frl[i] = fopen(fileName,"rb");
		fread(&es[i],sizeof(Edge),1,frl[i]);
	}


	int minIndex,previousA = -1,previousB = -1;

	int i = 0;

	short maxDegree = 0;

	short degree = -1;
	printf("start merge\n");
	// Edge s;
	// for (int i = 0; i < 100; ++i)
	// {
	// 	fread(&s,sizeof(Edge),1,frl[0]);
	// 	printf("a=%d,b=%d\n",s.a,s.b );
	// }
	while(mergeFinished(es,size)){
		minIndex = min(es,size);
		int u = es[minIndex].a;
		int v = es[minIndex].b;
		
		if(u != previousA){
			if (u%100000 == 0){
				printf("[%d]\n",u );
			}
			
			if(degree != -1){
				fwrite(&degree,sizeof(short),1,fIdx);
				maxDegree = degree>maxDegree?degree:maxDegree;
			}

			fgetpos(fDat,&pos);
			fwrite(&pos,sizeof(fpos_t),1,fIdx);

			degree = 1;

			fwrite(&v,sizeof(int),1,fDat);

			

		}else if(v != previousB){

			fwrite(&v,sizeof(int),1,fDat);
			++degree;

		}
		previousA = u;
		previousB = v;
		if(!fread(&es[minIndex],sizeof(Edge),1,frl[minIndex])){
			es[minIndex].a = m_maxID;
		}
		

	}

	fwrite(&degree,sizeof(short),1,fIdx);

	maxDegree = degree>maxDegree?degree:maxDegree;

	printf("max degree: %d\n",maxDegree);

	for (int i = 0; i < size ; ++i){
		fclose(frl[i]);
	}

	fclose(fIdx);
	fclose(fDat);

	delete[] frl;
	delete[] es;
}

bool Application::mergeFinished(Edge* es, int size){
	
	for (int i = 0; i < size; ++i){
		if(es[i].a!=m_maxID){
			return true;
		}
	}

	return false;
}

int Application::min(Edge* es,int size){
	int min = 0;
	for (int i = 1; i < size; ++i){
		if(es[i].a < es[min].a){
			min = i;
		}else if(es[i].a > es[min].a){
			continue;
		}else if(es[i].b < es[min].b){
			min = i;
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
