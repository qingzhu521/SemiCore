#include "Application.h"

Application::Application(string base){
	m_base = base;
	m_dat = base+"graph.dat";
	m_idx = base+"graph.idx";
	m_info = base+"graph.info";
	m_maxID = 1000000000;
	// m_maxCore = 30000;
	m_maxCore = 500;
}
Application::Application(){

}
Application::~Application(){
	if(m_ub!=NULL){
		delete[] m_ub;
	}
	if(m_lb!=NULL){
		delete[] m_lb;
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
	fInfo.fread(&m_m,sizeof(int));
	
	fInfo.fread(&m_maxDegree,sizeof(int));
	printf("max degree: %d\n",m_maxDegree );
	
	fInfo.fclose();

	m_maxCore = m_maxCore>m_maxDegree?m_maxDegree:m_maxCore;

	// array for saving upper bound of vertex core number, array will update iteratively
	m_ub = new short[m_m];
	// lower bound
	m_lb = new short[m_m];
	// array for saving count number of vertex
	short* cnt = new short[m_m];

	// array for loading neighbors of every vertex
	int* nbr = new int[m_maxDegree];
	// neighbors' count number for top-down processing
	short* nbrCnt = new short[m_maxDegree];

	// degree of every vertex
	int* m_degree = new int[m_m];

	// if vertex's neighbors are loaded in memory, iteration back to this vertex
	queue<int> waitVertices;
	
	// array for save vertices in subgraph
	vector<int> imVertices;

	// array for in-memory subgraph
	Vertex** imGraph = new Vertex*[m_m];
	for (int i = 0; i < m_m; ++i)
	{
		imGraph[i] = new Vertex;
		imGraph[i]->id = i;
		imGraph[i]->core = 0;
		imGraph[i]->discard = 0;
		imGraph[i]->previous = NULL;
		imGraph[i]->next = NULL;
	}

	// remember first edge save in subgraph to prveenting from overlapping
	int firstU = -1,firstV = -1;
	
	// array for im-memory k-core
	Vertex** imCore = new Vertex*[m_maxCore+1];
	memset(imCore,0,sizeof(Vertex*)*(m_maxCore+1));
	printf("maxCore: %d\n",m_maxCore );

	// calculate the rest size
	int size = 3*sizeof(short)*m_m+sizeof(short)*m_maxDegree+sizeof(int)*m_maxDegree+(sizeof(int)*3+sizeof(long)*3+sizeof(short))*m_m+sizeof(long)*(m_maxCore+1);
	int x = size/1024/1024;
	printf("total memmory: %dMB\n",x );

	m_restEdges = (1*1024 - x)/2/sizeof(int)*1024*1024;
	m_restEdges = 1000;
	printf("load most %ld edges\n",m_restEdges);

	long t = clock();


	
	MyReadFile fIdx( m_idx );
	fIdx.fopen( BUFFERED );
	MyReadFile fDat( m_dat );
	fDat.fopen( BUFFERED );


	// initialize array ub and cnt by degree and 0 respectively
	long tmp;
	int degreeTmp;
	for (int i = 0; i < m_m; ++i){
		fIdx.fread(&tmp,sizeof(long));
		fIdx.fread(&degreeTmp,sizeof(int));
		m_degree[i] = degreeTmp; 
		m_ub[i] = degreeTmp>m_maxCore?m_maxCore:degreeTmp;
		m_lb[i] = 1;
		cnt[i] = 0;
	}
	// memset(cnt,0,sizeof(short)*m_m);


	int degree,v;
	long currentEdges=0;

	int iteration = 0;
	
	// int memory = (sizeof(short)*m_m*2+sizeof(int)*m_maxDegree+sizeof(short)*m_maxDegree)/1024/1024;
	// printf("Memory useage: %d MB\n",memory );
	// if all vertexs satisfy: cnt number >= ub number, loop will terminate and we get final core number.
	bool update = true;
	while(update){
		update = false;
		printf("iteration: %d\n",++iteration );

		
		for (int u = 0; u < m_m; ++u){

			bool inMemory = false;
			if(waitVertices.size()>0){
				u = waitVertices.front();
				waitVertices.pop();
				inMemory = true;
				// if(u == 88){
				// 	printf("reload u = %d\n",u );
				// }
				
			}
			// if(u == 88 ){
			// 	printf("into 88,ub=%d,lb=%d,cnt=%d\n",m_ub[u],m_lb[u],cnt[u] );
			// 	if(inMemory){
			// 		printf("isInMemory\n");
			// 	}
			// }
			if(m_lb[u] == m_ub[u] || cnt[u]>=m_ub[u]){
				// if(u == 88){
				// 	printf("continue\n");
				// }
				continue;
			}
			
			short originUb = m_ub[u];

			int loadDegree,deposit=0;
			// get neighbors of vertex i
			if(inMemory){
				degree = imGraph[u]->core;
				for (int i = 0; i < imGraph[u]->nbr.size(); ++i){
					nbr[i] = imGraph[u]->nbr[i];
				}
				loadDegree = imGraph[u]->nbr.size();
			}else{
				loadNbr(u,nbr,degree,fIdx,fDat,currentEdges,imGraph,imCore,imVertices,firstU,firstV);
				loadDegree = degree;
			}
			
			
			if(m_lb[u] == originUb-1){
				// printf("lb[%d]=%d,originUb=%d\n",u,m_lb[u],originUb );
				// printf("hit!!!!!!!!!!!\n");
				--m_ub[u];
			}else{
				// get the core distribution for neighbors' contribution
				if(inMemory){
					deposit = imGraph[u]->core - loadDegree;
				}
				memset(nbrCnt,0,sizeof(short)*(originUb+1));
				for (int j = 0; j < loadDegree; ++j){
					v = nbr[j];
					++nbrCnt[m_ub[v]<m_ub[u]?m_ub[v]:m_ub[u]];
				}


				// calculate new ub and new cnt
				cnt[u] = deposit;
				for (int i = originUb-deposit; i > 0; --i){
					cnt[u] += nbrCnt[i];
					if(cnt[u] >= i){
						m_ub[u] = i;
						break;
					}
				}


			}
			
			// process neighbors
			if(m_ub[u]<originUb){
				update = true;
				for (int i = 0; i < degree; ++i){
					v = nbr[i];
					if(m_ub[v]>m_ub[u] && m_ub[v]<= originUb){
						--cnt[v];
						
						if(imGraph[v]->core + imGraph[v]->discard == m_degree[v] && m_ub[v]!=m_lb[v]){
							// printf("save 1 IO\n");
							// waitVertices.push(v);
							if(waitVertices.size() == 0){
								waitVertices.push(v);
								waitVertices.push(u+1);
							}else{
								int curIteration = waitVertices.back();
								waitVertices.back() = v;
								waitVertices.push(curIteration);
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
	
	
	
	delete[] nbrCnt;
	delete[] nbr;
	delete[] cnt;
	delete[] imCore;
	for (int i = 0; i < m_m; ++i){
		delete imGraph[i];
	}
	delete[] imGraph;
	
	
	fDat.fclose();
	fIdx.fclose();

}


void Application::loadNbr(int u, int* nbr, int& degree, MyReadFile& fIdx, MyReadFile& fDat, long& currentEdges, Vertex** imGraph, Vertex** imCore, vector<int>& imVertices, int& firstU, int& firstV){
	
	fIdx.fseek(u*(sizeof(long)+sizeof(int)));
	long pos;
	fIdx.fread(&pos,sizeof(long));
	fIdx.fread(&degree,sizeof(int));

	fDat.fseek(pos);
	
	// load all neighbors of vertex u
	for (int i = 0; i < degree; ++i){
		fDat.fread(&nbr[i],sizeof(int));
		if(currentEdges >= m_restEdges || (u == firstU && nbr[i] == firstV)){
			imKCore(imGraph,imCore,imVertices);
			currentEdges = 0;
			imVertices.clear();
		}
		
		// remember first edge of subgraph to prevent from overlapping
		if(currentEdges == 0){
			firstU = u;
			firstV = nbr[i];
		}else if( u > firstU && nbr[i]>=firstU && nbr[i]<u){
			// check if edge(u,nbr[i]) is loaded
			continue;
		}else if(u <= firstU && (nbr[i]<u || nbr[i] >= firstU) ){
			continue;
		}
		
		if(m_lb[u]>=m_ub[nbr[i]]){
			if(imGraph[nbr[i]]->core == 0){
				imVertices.push_back(nbr[i]);
			}
			++(imGraph[nbr[i]]->core);
			++(imGraph[u]->discard);

		}else if(m_ub[u]<=m_lb[nbr[i]]){
			if(imGraph[u]->core == 0){
				imVertices.push_back(u);
			}
			++(imGraph[u]->core);
			++(imGraph[nbr[i]]->discard);
		}else{
			if(imGraph[u]->core == 0){
				imVertices.push_back(u);
			}
			if(imGraph[nbr[i]]->core == 0){
				imVertices.push_back(nbr[i]);
			}
			// update vertex number
			++currentEdges;
			imGraph[u]->nbr.push_back(nbr[i]);
			++(imGraph[u]->core);
			
			imGraph[nbr[i]]->nbr.push_back(u);
			++(imGraph[nbr[i]]->core);
		}
		
		
		
	}
	// fDat.fread(nbr,sizeof(int)*degree);

}

void Application::imKCore(Vertex** imGraph, Vertex** imCore, vector<int>& imVertices ){
	printf("in-memory core start...\n");
	// initialize imCore for in-memory k-core computing
	for (vector<int>::iterator it = imVertices.begin() ; it != imVertices.end() ; ++it){
		Vertex* u = imGraph[*it];
		u->previous = NULL;
		u->next = NULL;

		if(m_lb[u->id] == m_ub[u->id]){
			// exact core number has been calculated
			u->core = m_ub[u->id];
		}
		
		if(u->core < m_lb[u->id]){
			u->core = m_lb[u->id];
		}

		int level = u->core > m_maxCore ? m_maxCore:u->core;
		if(imCore[level]){
			imCore[level]->previous = u;
			u->next = imCore[level];
		}
		imCore[level] = u;
	}


	// calculate core
	printf("bottom-up processing\n");
	
	for (int i = 1; i <= m_maxCore; ++i){

		while(imCore[i]){

			Vertex* u = imCore[i];
			if(i == 1){
				printf("load vertex %d\n",u->id );
			}
			// update lower bound
			if(i>m_lb[u->id]){
				m_lb[u->id] = i;
			}
			
			// remove vertex from doubly linked list
			imCore[i] = u->next;
			if(imCore[i]){
				imCore[i]->previous = NULL;
			}


			// delete induced edges
			for (vector<int>::iterator it = u->nbr.begin(); it != u->nbr.end(); ++it){
				Vertex* v = imGraph[*it];

				v->nbr.erase(find(v->nbr.begin(),v->nbr.end(),u->id));
				if(v->core == i){
					continue;
				}
				int newCore = --(v->core);
				if(newCore < m_maxCore){
					// remove vertex from original doubly linked list
					if(v->previous){
						v->previous = v->next;
					}else{
						imCore[newCore+1] = v->next;
					}
					if(v->next){
						v->next->previous = v->previous;
					}
					v->previous = NULL;
					v->next = NULL;

					// add vertex to new doubly linked list
					if(imCore[newCore]){
						imCore[newCore]->previous = v;
						v->next = imCore[newCore];
					}
					imCore[newCore] = v;
					

				}
			}

			// clear the v->nbr, release memory
			vector<int>().swap(u->nbr);
			u->core = 0;
			u->discard = 0;
			u->next = NULL;
			u->previous = NULL;

		}
	}
	printf("in-memory core finished...\n");
}

void Application::printCoreDistribution(){
	int* core = new int[m_maxDegree+1];
	memset(core,0,sizeof(int)*(m_maxDegree+1));
	int maxCore = 0;
	for (int i = 0; i < m_m; ++i){
		++core[m_ub[i]];
		maxCore = m_ub[i]>maxCore?m_ub[i]:maxCore;
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