#include "Application.h"

Application::Application(string base){
	m_base = base;
	m_dat = base+"graph.dat";
	m_idx = base+"graph.idx";
	m_info = base+"graph.info";
	m_maxID = 1000000000;
	m_dynamicAdd = NULL;
	ub = NULL;
	cnt = NULL;
}
Application::Application(){

}
Application::~Application(){
	if(ub!=NULL){
		delete[] ub;
	}
	if(cnt!=NULL){
		delete[] cnt;
	}
	if(m_dynamicAdd != NULL){
		for(int i = 0;i<m_m;++i){
			if(m_dynamicAdd[i]){
				delete m_dynamicAdd[i];
			}
		}
		delete[] m_dynamicAdd;
	}
	if(m_dynamicDel != NULL){
		for(int i = 0;i<m_m;++i){
			if(m_dynamicDel[i]){
				delete m_dynamicDel[i];
			}
		}
		delete[] m_dynamicDel;
	}
}

void Application::sortEdge(string txtFile,int scale){
	printf("Start... txtFile: %s\n",txtFile.c_str());

	scale = scale/10-1;
	
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

		if(u == v) continue;

		if(u%10>scale || v%10>scale) continue;

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

void Application::semiKCoreNaive(){
	MyReadFile fInfo( m_info );
	fInfo.fopen( BUFFERED );
	// initialize verterx number: n
	fInfo.fread(&m_m,sizeof(int));
	fInfo.fread(&m_maxDegree,sizeof(int));
	printf("Number of vertices: %d, max degree: %d\n",m_m, m_maxDegree );
	fInfo.fclose();


	struct timeval start,finish;
	gettimeofday(&start,NULL);
	double totaltime = 0.0;
	
	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );

	// array for saving upper bound of vertex core number, array will update iteratively
	ub = new short[m_m];
	

	// array for loading every
	int* nbr = new int[m_maxDegree];
	short* nbrCnt = new short[m_maxDegree];


	// initialize array ub and cnt by degree and 0 respectively
	long tmp;
	int degreeTmp;
	for (int i = 0; i < m_m; ++i){
		fIdx.fread(&tmp,sizeof(long));

		fIdx.fread(&degreeTmp,sizeof(int));
		ub[i] = degreeTmp>m_maxCore?m_maxCore:degreeTmp;
	}
	


	int degree;
	int v;
	int iteration = 0;
	
	double memory = (sizeof(short)*m_m+sizeof(int)*m_maxDegree+sizeof(short)*m_maxDegree)/1024;
	printf("[Naive] Memory useage: %.3f KB, %.3f MB\n",memory,memory/1024 );

	
	bool update = true;
	while(update){
		update = false;
		printf("[Naive] iteration: %d\n",++iteration );

		
		for (int u = 0; u < m_m; ++u){

			short originUb = ub[u];
			// get neighbors of vertex i
			loadNbr(u,nbr,degree,fIdx,fDat);
			

			// get the core distribution for neighbors' contribution
			memset(nbrCnt,0,sizeof(short)*(originUb+1));
			for (int j = 0; j < degree; ++j){
				v = nbr[j];
				++nbrCnt[ub[v]<ub[u]?ub[v]:ub[u]];
			}


			// calculate new ub and new cnt
			int cnt = 0;
			for (int i = originUb; i > 0; --i){
				cnt += nbrCnt[i];
				if(cnt >= i){
					ub[u] = i;
					break;
				}
			}
			
			// process neighbors
			if(ub[u]<originUb){
				update = true;
			}
			
			
		}

	}
	
	gettimeofday(&finish,NULL);
	totaltime = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0;
	
	printf("[Naive]Total processing time = %.3f sec\n",totaltime);
	
	printf("[Naive]Memory useage: %.3f KB, %.3f MB\n",memory,memory/1024 );
	printf("[Naive]I/O number: %ld (.dat file), %ld (.idx file) \n",fDat.get_total_io(),fIdx.get_total_io());
	printf("[Naive]Number of vertices: %d, max degree: %d\n",m_m, m_maxDegree );

	delete[] nbrCnt;
	delete[] nbr;
	
	fDat.fclose();
	fIdx.fclose();
}

void Application::semiKCore(){
	
	MyReadFile fInfo( m_info );
	fInfo.fopen( BUFFERED );
	// initialize verterx number: n
	fInfo.fread(&m_m,sizeof(int));
	fInfo.fread(&m_maxDegree,sizeof(int));
	printf("Number of vertices: %d, max degree: %d\n",m_m, m_maxDegree );
	fInfo.fclose();


	struct timeval start,finish;
	gettimeofday(&start,NULL);
	double totaltime = 0.0;
	
	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );

	// array for saving upper bound of vertex core number, array will update iteratively
	ub = new short[m_m];
	// array for saving count number of vertex
	cnt = new short[m_m];

	// array for loading every
	int* nbr = new int[m_maxDegree];
	short* nbrCnt = new short[m_maxDegree];


	// initialize array ub and cnt by degree and 0 respectively
	long tmp;
	int degreeTmp;
	for (int i = 0; i < m_m; ++i){
		fIdx.fread(&tmp,sizeof(long));

		fIdx.fread(&degreeTmp,sizeof(int));
		ub[i] = degreeTmp>m_maxCore?m_maxCore:degreeTmp;
	}
	memset(cnt,0,sizeof(short)*m_m);


	int degree;
	int v;
	int iteration = 0;
	
	double memory = (sizeof(short)*m_m*2+sizeof(int)*m_maxDegree+sizeof(short)*m_maxDegree)/1024;
	printf("Memory useage: %.3f KB, %.3f MB\n",memory,memory/1024 );

	
	bool update = true;
	while(update){
		update = false;
		printf("iteration: %d\n",++iteration );

		
		for (int u = 0; u < m_m; ++u){

			if(cnt[u]>=ub[u]){
				continue;
			}

			short originUb = ub[u];
			// get neighbors of vertex i
			loadNbr(u,nbr,degree,fIdx,fDat);
			

			// get the core distribution for neighbors' contribution
			memset(nbrCnt,0,sizeof(short)*(originUb+1));
			for (int j = 0; j < degree; ++j){
				v = nbr[j];
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
					v = nbr[i];
					if(ub[v]>ub[u] && ub[v]<= originUb){
						--cnt[v];
					}
				}
			}
			
			
		}

	}
	
	gettimeofday(&finish,NULL);
	totaltime = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0;

	printf("Total processing time = %.3f sec (by gettimeofday() function)\n",totaltime);
	
	printf("Memory useage: %.3f KB, %.3f MB\n",memory,memory/1024 );
	printf("I/O number: %ld (.dat file), %ld (.idx file) \n",fDat.get_total_io(),fIdx.get_total_io());
	printf("Number of vertices: %d, max degree: %d\n",m_m, m_maxDegree );

	delete[] nbrCnt;
	delete[] nbr;
	
	fDat.fclose();
	fIdx.fclose();

}


void Application::loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat){
	
	fIdx.fseek(u*(sizeof(long)+sizeof(int)));

	long pos;
	fIdx.fread(&pos,sizeof(long));
	fIdx.fread(&degree,sizeof(int));

	fDat.fseek(pos);
	
	// load all neighbors of vertex u
	fDat.fread(nbr,sizeof(int)*degree);

}

void Application::printCoreDistribution(){
	int* core = new int[m_maxDegree];
	memset(core,0,sizeof(int)*m_maxDegree);
	int maxCore = 0;
	for (int i = 0; i < m_m; ++i){
		++core[ub[i]];
		maxCore = ub[i]>maxCore?ub[i]:maxCore;
	}

	printf("1-core: %d, %d-core: %d\n",core[1],maxCore,core[maxCore]);

	for (int i = 0; i <= maxCore; ++i){
		if(core[i]>0){
			printf("core %d: %d\n",i,core[i] );
		}
		
	}

	delete[] core;

}

void Application::loadNbrForDynamic(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat){
	fIdx.fseek(u*(sizeof(long)+sizeof(int)));

	long pos;
	fIdx.fread(&pos,sizeof(long));
	int purDegree;
	fIdx.fread(&purDegree,sizeof(int));

	fDat.fseek(pos);
	
	// load all neighbors of vertex u
	degree = 0;
	int t;
	for(int i = 0;i<purDegree;++i){
		
		fDat.fread(&t,sizeof(int));
		if(!m_dynamicDel[u] || m_dynamicDel[u]->find(t)==m_dynamicDel[u]->end()){
			nbr[degree++] = t;
		}
	}
	if(m_dynamicAdd[u]){
		int addDegree = m_dynamicAdd[u]->size();
		for(int i = 0;i<addDegree;++i){
			t = m_dynamicAdd[u]->at(i);
			if(!m_dynamicDel[u] || m_dynamicDel[u]->find(t)==m_dynamicDel[u]->end()){
				nbr[degree++] = t;
			}
		}
	}

	
}
void Application::addEdge(int a, int b){

	
	if(!m_dynamicAdd[a]){
		m_dynamicAdd[a] = new vector<int>;
	}
	if(!m_dynamicAdd[b]){
		m_dynamicAdd[b] = new vector<int>;
	}
	m_dynamicAdd[a]->push_back(b);
	m_dynamicAdd[b]->push_back(a);
	

	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );

	int* nbr = new int[m_maxDegree];
	int degree;

	short* cntPlus = new short[m_m];

	bool* x = new bool[m_m];
	bool* y = new bool[m_m];
	memset(x,false,m_m);
	memset(y,false,m_m);


	// printf("Add Edge(%d, %d),UB[%d] = %d, UB[%d] = %d\n",a,b,a,ub[a],b,ub[b] );
	if(ub[a] == ub[b]){
		y[b] = true;
		++cnt[b];
	}

	int root = ub[a] > ub[b] ? b : a;
	y[root] = true;
	++cnt[root];

	bool update = true;
	int v;
	while(update){
		update = false;
		for(int u = 0; u<m_m; ++u){
			if(y[u] == true && x[u] == false){
				x[u] = true;
				y[u] = true;
				update = true;

				// compute cntPlus
				loadNbrForDynamic(u,nbr,degree,fIdx,fDat);
				
				cntPlus[u] = 0;
				for(int i = 0; i < degree; ++i){
					v = nbr[i];
					if(ub[v] > ub[u] || (ub[v] == ub[u] && cnt[v] > ub[v] && (x[v]!=true || y[v]!=false))){
						++cntPlus[u];
					}
				}

				if(cntPlus[u] > ub[u]){
					for(int i = 0; i < degree; ++i){
						v = nbr[i];
						if(cnt[v] > ub[v] && !x[v] && !y[v]){
							x[v] = false;
							y[v] = true;

						}
					}
				}
			}
			if(x[u] && y[u] && cntPlus[u]<=ub[u]){
				x[u] = true;
				y[u] = false;
				update = true;
				if(degree == -1){
					loadNbrForDynamic(u,nbr,degree,fIdx,fDat);
				}
				for(int i = 0; i < degree; ++i){
					v = nbr[i];
					if(x[v] && y[v]){
						--cntPlus[v];
					}
				}

			}
			degree = -1;
		}

	}

	for(int u = 0; u < m_m; ++u){
		if(!y[u]){
			continue;
		}
		++ub[u];

		cnt[u] = 0;
		loadNbrForDynamic(u,nbr,degree,fIdx,fDat);
		for(int i = 0; i<degree;++i){
			int v = nbr[i];
			if(ub[v]>=ub[u] || y[v]){
				++cnt[u];
			}
		}
		// y[u] = false;
		// x[u] = false;
	}

	

	delete[] x;
	delete[] y;
	delete[] nbr;

	fDat.fclose();
	fIdx.fclose();
}

void Application::removeEdge(int a, int b){


	if(!m_dynamicDel[a]){
		m_dynamicDel[a] = new unordered_set<int>;
	}
	if(!m_dynamicDel[b]){
		m_dynamicDel[b] = new unordered_set<int>;
	}
	m_dynamicDel[a]->insert(b);
	m_dynamicDel[b]->insert(a);
	


	if(ub[a] == ub[b]){
		--cnt[a];
		--cnt[b];
		if(cnt[a]>=ub[a] && cnt[b]>=ub[b]){
			return;
		}
	}else{
		int root = ub[a] > ub[b] ? b : a;
		--cnt[root];
		if(cnt[root]>=ub[root]){
			return;
		}
	}


	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );

	int* nbr = new int[m_maxDegree];
	int degree;
	short* nbrCnt = new short[m_maxDegree];

	

	bool update = true;
	while(update){
		update = false;

		
		for (int u = 0; u < m_m; ++u){

			if(cnt[u]>=ub[u]){
				continue;
			}

			short originUb = ub[u];
			// get neighbors of vertex i
			loadNbrForDynamic(u,nbr,degree,fIdx,fDat);
			
			int v;
			// get the core distribution for neighbors' contribution
			memset(nbrCnt,0,sizeof(short)*(originUb+1));
			for (int j = 0; j < degree; ++j){
				v = nbr[j];
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
					v = nbr[i];
					if(ub[v]>ub[u] && ub[v]<= originUb){
						--cnt[v];
					}
				}
			}
			
			
		}

	}
	
	delete[] nbr;
	delete[] nbrCnt;

	fDat.fclose();
	fIdx.fclose();
}
int Application::selectNbr(int a){
	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );
	int* nbr = new int[m_maxDegree];
	int degree;

	fIdx.fseek(a*(sizeof(long)+sizeof(int)));

	long pos;
	fIdx.fread(&pos,sizeof(long));
	fIdx.fread(&degree,sizeof(int));

	fDat.fseek(pos);
	
	// load all neighbors of vertex u
	fDat.fread(nbr,sizeof(int)*degree);

	int r = nbr[0];

	delete nbr;

	fDat.fclose();
	fIdx.fclose();
	return r;
}
void Application::dynamicCore(int num){
	m_dynamicAdd = new vector<int>*[m_m];
	m_dynamicDel = new unordered_set<int>*[m_m];

	memset(m_dynamicAdd,0,sizeof(vector<int>*)*m_m);
	memset(m_dynamicDel,0,sizeof(unordered_set<int>*)*m_m);

	// prepare dynamic edges
	int dynamic[num*2];
	int sep = m_m/num;
	int curVertex = 0, cut = 0;
	int min, max;
	for(int i=0;curVertex<m_m;i+=2){
		int b = selectNbr(curVertex);
		if(b%num == 0 && b<curVertex){
			++cut;
			i -= 2;
		}else{
			dynamic[i] = curVertex;
			dynamic[i+1] = b;
		}
		curVertex += sep;
	}
	num -= cut;
	printf("select %d edges\n",num );
	// add edge
	printf("start add edges: \n");

	struct timeval start,finish;
	gettimeofday(&start,NULL);
	double totaltime = 0.0;

	for (int i = 0; i < num; ++i){
		printf("[%d,%d] ",dynamic[i*2],dynamic[i*2+1] );
		addEdge(dynamic[i*2],dynamic[i*2+1]);
	}
	gettimeofday(&finish,NULL);
	totaltime = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0;
	printf("\nAdd %d edges, time = %.3f sec, average time for one edge: %.3f \n",num,totaltime,totaltime/num);
	

	// remove edge
	printf("start remove edges:\n");
	gettimeofday(&start,NULL);
	for (int i = 0; i < num; ++i){
		printf("[%d,%d] ",dynamic[i*2],dynamic[i*2+1] );
		removeEdge(dynamic[i*2],dynamic[i*2+1]);
	}
	gettimeofday(&finish,NULL);
	totaltime = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0;
	printf("\nRemove %d edges, time = %.3f sec, average time for one edge: %.3f \n",num,totaltime,totaltime/num);

}
void Application::getGraphInfo(){
	MyReadFile fInfo( m_info );
	fInfo.fopen( BUFFERED );
	// initialize verterx number: n
	fInfo.fread(&m_m,sizeof(int));
	fInfo.fread(&m_maxDegree,sizeof(int));
	
	fInfo.fclose();

	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	unsigned long r = 0;
	for(int i = 0;i<m_m;++i){
		// fIdx.fseek(i*(sizeof(long)+sizeof(int)));
		long pos;
		int degree;
		fIdx.fread(&pos,sizeof(long));
		fIdx.fread(&degree,sizeof(int));
		r += degree;
	}
	fIdx.fclose();
	printf("Number of vertices: %d, max degree: %d, edges: %lu\n",m_m, m_maxDegree,r>>1 );
}
void Application::saveCore(){
	printf("save core number and cnt number\n");
	string core = m_base+"core.st";
	FILE* fSt = fopen(core.c_str(),"wb");
	fwrite(ub,sizeof(short),m_m,fSt);
	fwrite(cnt,sizeof(short),m_m,fSt);
	fclose(fSt);
}
void Application::loadCore(){
	MyReadFile fInfo( m_info );
	fInfo.fopen( BUFFERED );
	// initialize verterx number: n
	fInfo.fread(&m_m,sizeof(int));
	fInfo.fread(&m_maxDegree,sizeof(int));
	printf("Number of vertices: %d, max degree: %d\n",m_m, m_maxDegree );
	fInfo.fclose();
	printf("1\n");
	printf("load ub and cnt\n");
	string core = m_base+"core.st";
	MyReadFile fSt( core );
	printf("2\n");
	fSt.fopen( BUFFERED );
	printf("3\n");
	ub = new short[m_m];
	cnt = new short[m_m];
	fSt.fread(ub,sizeof(short)*m_m);
	fSt.fread(cnt,sizeof(short)*m_m);
	fSt.fclose();
}
