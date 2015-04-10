#include "Application.h"

Application::Application(string base){
	m_base = base;
	m_dat = base+"graph.dat";
	m_idx = base+"graph.idx";
	m_info = base+"graph.info";
	m_maxID = 1000000000;
}
Application::Application(){

}
Application::~Application(){
	if(ub!=NULL){
		delete[] ub;
	}
}

void Application::sortEdge(string txtFile){
	printf("Start... txtFile: %s\n",txtFile.c_str());

	
	int memEdges = 100000000;

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


	delete[] m_vertexMap;
	delete[] edges;

	fclose(fp);

	merge(tmpFile+1);

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

// sort edges and save
void Application::saveTmpEdges(Edge* edges, int size, int tmpFile){
	printf("sort edges...\n");
	sort(edges,edges+size,edgeCompare);
	char fileName[200];
	sprintf(fileName,"%ssort_edge_tmp/edges_tmp_%d",m_base.c_str(),tmpFile);
	printf("Creating tmp_file_%d: %s\n",tmpFile,fileName);

	FILE* fo = fopen(fileName,"wb");
	for (int i = 0; i < size; ++i){
		fwrite( edges+i, sizeof(Edge), 1, fo );
		// printf("edge[%d,%d]\n",edges[i].a,edges[i].b );
	}
	printf("------\n\n");
	fclose(fo);
	
}

// merge_sort edges from all edges_tmp files
void Application::merge(int size){
	FILE** frl = new FILE*[size];
	Edge* es = new Edge[size];

	FILE* fIdx = fopen(m_idx.c_str(),"wb");
	FILE* fDat = fopen(m_dat.c_str(),"wb");

	long pos;
	


	for (int i = 0; i < size ; ++i){
		char fileName[200];
		sprintf(fileName,"%ssort_edge_tmp/edges_tmp_%d",m_base.c_str(),i);
		
		frl[i] = fopen(fileName,"rb");
		// get first edge of all edges_tmp files
		fread(&es[i],sizeof(Edge),1,frl[i]);
	}


	int minIndex,previousA = -1,previousB = -1;

	int i = 0;

	int maxDegree = 0;

	int degree = -1;
	printf("start merge\n");
	int x=0;
	int f = 0;
	while(mergeFinished(es,size)){
		minIndex = min(es,size);
		int u = es[minIndex].a;
		int v = es[minIndex].b;
		
		if(u != previousA){
			// u != previousA demonstrates that all previousA's neighbors have been writen
			if (u%100000 == 0){
				printf("[%d]\n",u );
			}
			
			if(degree != -1){

				// write the vertex degree in .idx file
				fwrite(&degree,sizeof(int),1,fIdx);
				maxDegree = degree>maxDegree?degree:maxDegree;
				// printf("max degree: %d\n",maxDegree );
			}

			// write the vertex beginning position in .dat file to .idx file
			pos = ftell(fDat);
			fwrite(&pos,sizeof(long),1,fIdx);

			degree = 1;

			fwrite(&v,sizeof(int),1,fDat);


		}else if(v != previousB){

			fwrite(&v,sizeof(int),1,fDat);
			++degree;

		}
		
		// if u==previousA & v==previousB, ignore edge(u,v) cause it is same as previous one.

		previousA = u;
		previousB = v;

		// replace es[minIndex] by picking up the first edge from file edges_tmp_minIndex
		if(!fread(&es[minIndex],sizeof(Edge),1,frl[minIndex])){
			es[minIndex].a = m_maxID;
		}
		

	}

	fwrite(&degree,sizeof(int),1,fIdx);

	maxDegree = degree>maxDegree?degree:maxDegree;

	// write the vertex num and max degree
	int vertexNum = previousA+1;
	printf("vertex num: %d\n",vertexNum);
	printf("max degree: %d\n",maxDegree);

	FILE* fInfo = fopen(m_info.c_str(),"wb");
	fwrite(&vertexNum,sizeof(int),1,fInfo);
	fwrite(&maxDegree,sizeof(int),1,fInfo);
	fclose(fInfo);

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

// get minimal edge from edge list
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

// get final vertexID from map array
int Application::getVertexID(int u,int &num){
	if(m_vertexMap[u]<0){
		m_vertexMap[u] = num++;
	}
	return m_vertexMap[u];
}

void Application::semiKCore(){
	
	MyReadFile fInfo( m_info );
	fInfo.fopen( BUFFERED );
	
	// initialize verterx number: n
	fInfo.fread(&m_n,sizeof(int));
	
	fInfo.fread(&m_maxDegree,sizeof(int));
	printf("max degree: %d\n",m_maxDegree );
	
	fInfo.fclose();
	
	
	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );

	// array for saving upper bound of vertex core number, array will update iteratively
	ub = new short[m_n];
	// array for saving count number of vertex
	short* cnt = new short[m_n];

	// array for loading every
	int* nbr = new int[m_maxDegree];
	short* nbrCnt = new short[m_maxDegree];

	m_cacheFront = 0;
	m_cacheBack = -1;
	m_cacheCur = 0;
	m_cacheIndex = new int[m_n+1];

	m_cacheSize = 1*1024/sizeof(int)*1024;
	m_cacheSize = 5000;
	m_cache = new int[m_cacheSize];

	
	// int memory = (sizeof(short)*m_n*2+sizeof(int)*m_maxDegree+sizeof(short)*m_maxDegree)/1024/1024;
	// printf("Memory useage: %d MB\n",memory );

	long t = clock();
	// initialize array ub and cnt by degree and 0 respectively
	long tmp;
	int degreeTmp;
	for (int i = 0; i < m_n; ++i){
		fIdx.fread(&tmp,sizeof(long));

		fIdx.fread(&degreeTmp,sizeof(int));
		ub[i] = degreeTmp>m_maxCore?m_maxCore:degreeTmp;
	}
	memset(cnt,0,sizeof(short)*m_n);
	m_degree = new int[m_n];

	int degree;
	int v;

	int iteration = 0;
	
	queue<int> imVertices;

	
	// if all vertexs satisfy: cnt number >= ub number, loop will terminate and we get final core number.
	bool update = true;
	while(update){
		update = false;
		printf("iteration: %d\n",++iteration );

		
		for (int u = 0; u < m_n; ++u){
			
			bool inMemory = false;
			if(imVertices.size()>0){
				u = imVertices.front();
				imVertices.pop();
				
				if(imVertices.size()>0){
					inMemory = true;
					
				}

			}
			
			if(cnt[u]>=ub[u]){
				
				continue;
			}

			short originUb = ub[u];
			// get neighbors of vertex i
			
			if(inMemory){

				degree = m_cacheIndex[u+1]>m_cacheIndex[u]?m_cacheIndex[u+1]-m_cacheIndex[u]:m_cacheSize-m_cacheIndex[u]+m_cacheIndex[u+1];
			}else{
				loadNbr(u,nbr,degree,fIdx,fDat);
				
			}

			
			
			
			// get the core distribution for neighbors' contribution
			memset(nbrCnt,0,sizeof(short)*(originUb+1));
			for (int j = 0; j < degree; ++j){
				if(inMemory){
					
					v = m_cache[m_cacheIndex[u]+j];
					
				}else{
					v = nbr[j];
				}
				
				++nbrCnt[ub[v]<ub[u]?ub[v]:ub[u]];
			}

			
			// calculate new ub and new cnt
			cnt[u] = 0;
			for (int i = originUb; i > 0; --i){
				cnt[u] += nbrCnt[i];
				if(cnt[u] >= i){
					ub[u] = i;
					break;
				}
			}

			
			// process neighbors
			if(ub[u]<originUb){
				update = true;
				for (int i = 0; i < degree; ++i){
					if(inMemory){
						v = m_cache[m_cacheIndex[u]+i];
					}else{
						v = nbr[i];
					}
					if(ub[v]>ub[u] && ub[v]<= originUb){
						--cnt[v];
						if(cnt[v]<ub[v]){
							if((m_cacheFront<m_cacheBack && v<=m_cacheBack && v>=m_cacheFront)||(m_cacheFront>m_cacheBack && (v>=m_cacheFront || v<=m_cacheBack))){
								if(imVertices.size()==0){
									imVertices.push(v);
									imVertices.push(u+1);
								}else{
									int nextV = imVertices.back();
									imVertices.back() = v;
									imVertices.push(nextV);
								}
							}
						}
					}
				}
			}
			
			
		}
	}
	
	t = clock() - t;
	printf( "processing time = %0.3lf sec\n", t/1000000.0 );
	// printf("Memory useage: %d MB\n",memory );
	
	
	delete[] m_degree;
	delete[] nbrCnt;
	delete[] nbr;
	delete[] cnt;
	delete[] m_cacheIndex;
	delete[] m_cache;
	
	fDat.fclose();
	fIdx.fclose();

}


void Application::loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat){
	
	fIdx.fseek(u*(sizeof(long)+sizeof(int)));

	long pos;
	fIdx.fread(&pos,sizeof(long));
	fIdx.fread(&degree,sizeof(int));
	m_degree[u] = degree;
	fDat.fseek(pos);
	
	// load all neighbors of vertex u
	// fDat.fread(nbr,sizeof(int)*degree);


	if(u==m_n-1 && m_cacheFront == 0){
		fDat.fread(nbr,sizeof(int)*degree);
		return;
	}
	
	m_cacheIndex[u] = m_cacheCur;
	for (int i = 0; i < degree; ++i){
		fDat.fread(&nbr[i],sizeof(int));
		if(m_cacheBack!=-1 && m_cacheCur == m_cacheIndex[m_cacheFront]){
			m_cacheFront++;
			if(m_cacheFront>=m_n){
				m_cacheFront = 0;
			}
		}
		
		m_cache[m_cacheCur] = nbr[i];

		++m_cacheCur;

		if(m_cacheCur>=m_cacheSize){
			m_cacheCur = 0;

		}
	}
	
	m_cacheBack = u;
	m_cacheIndex[u+1] = m_cacheCur;

	if(u == 46 || u==47){
		printf("%d.idx = %d,nextIdx=%d\n",u,m_cacheIndex[u],m_cacheIndex[u+1] );
	}
	

}

void Application::printCoreDistribution(){

	int* core = new int[m_maxDegree];
	memset(core,0,sizeof(int)*m_maxDegree);
	int maxCore = 0;
	for (int i = 0; i < m_n; ++i){
		++core[ub[i]];
		maxCore = ub[i]>maxCore?ub[i]:maxCore;
	}

	// printf("core[1]: %d\n",core[1]);
	// printf("core[%d]: %d\n",maxCore,core[maxCore] );
	for (int i = 0; i <= maxCore; ++i){
		if(core[i]>0){
			printf("core %d: %d\n",i,core[i] );
		}
		
	}

	delete[] core;


	
}