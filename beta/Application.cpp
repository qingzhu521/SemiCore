#include "Application.h"

Application::Application() {
	n = 0;
	max_deg = 0;
	ub = NULL;
	cnt_nbr = NULL;
	cnt_ub = NULL;
	nbr = NULL;
}

int Application::get_len( char nowid, int nowpos, char nxtid, int nxtpos ) {
	if( nxtid == nowid )
		return (nxtpos-nowpos)/sizeof(int);
	return (nxtpos + (MyIO::bulk_size-nowpos))/sizeof(int);
}

int Application::get_core(int *cnt_b, int ub, int &n_nbr) {
	n_nbr = 0;
	int nowmax = 0;
	int nows = 0;
	// int nowv = 0;
	for( int i = ub; i >= 0; --i ) {
		// nows += cnt_b[i];
		// nowv = (nows < i ? nows : i);
		// if( nowv < nowmax )
		// 	break;
		// nowmax = nowv;
		// n_nbr = nows;
		nows += cnt_b[i];
		if(nows>=i){
			n_nbr = nows;
			nowmax = i;
			break;
		}
	}
	return nowmax;
}

void Application::load_graph_idx(string path) {
	graph_path = path;
	path = path + "graph.idx";
	printf( "loading graph %s\n", path.c_str() );

	MyReadFile rf( path );
	rf.fopen( BUFFERED );

	rf.fread( &n, sizeof(int) );
	// printf( "n=%d\n", n );


	char nowid, nxtid;
	int nowpos, nxtpos;

	max_deg = 0;
	for( int i = 0; i < n; ++i ) {
		rf.fread( &nxtid, sizeof(char) );
		rf.fread( &nxtpos, sizeof(int) );

		if( i > 0 ) {
			int len = get_len( nowid, nowpos, nxtid, nxtpos );
			if( len > max_deg )
				max_deg = len;
		}
		nowid = nxtid;
		nowpos =nxtpos;
	}
	rf.fclose();

	--n;

	
	printf( "max_deg=%d\n", max_deg );
}

void Application::print_distribution() {
	int *cnt = new int[max_core+1];
	memset( cnt, 0, sizeof(int) * (max_core+1));
	for( int i = 0; i < n; ++i )
		++cnt[ub[i]];

	printf( "Distribution:\n");
	for( int i = 0; i <= max_core; ++i )
		if( cnt[i] > 0 )
			printf( "%d:%d\n", i, cnt[i] );
	delete[] cnt;
}



int Application::load_nbr(MyReadFile &rf, MyReadFile &rf_idx, int p, int *nbr) {

	fileint pos = sizeof(int) + (fileint) p*(sizeof(char)+sizeof(int));
	rf_idx.fseek(pos);
	char nowid, nxtid;
	int nowpos, nxtpos;

	rf_idx.fread( &nowid, sizeof(char) );
	rf_idx.fread( &nowpos, sizeof(int) );
	rf_idx.fread( &nxtid, sizeof(char) );
	rf_idx.fread( &nxtpos, sizeof(int) );

	int n_nbr = get_len(nowid, nowpos, nxtid, nxtpos);

	pos = nowid * (fileint) MyIO::bulk_size + nowpos;
	rf.fseek(pos);
	rf.fread( nbr, sizeof(int) * n_nbr );

	return n_nbr;
}

void Application::process_basic() {
	long t = clock();



	ub = new short[n];
	cnt_nbr = new short[n];
	printf("n=%d, max_deg=%d\n",n,max_deg );



	
	string st_idx = graph_path + "graph.idx";

	MyReadFile rf_idx( st_idx );
	rf_idx.fopen( BUFFERED );
	rf_idx.fseek( sizeof(int) );

	char nowid, nxtid;
	int nowpos, nxtpos;

	for( int i = 0; i <= n; ++i ) {
		rf_idx.fread( &nxtid, sizeof(char) );
		rf_idx.fread( &nxtpos, sizeof(int) );

		if( i > 0 ) {
			int nowub = get_len( nowid, nowpos, nxtid, nxtpos );
			ub[i-1] = nowub > max_core ? max_core : nowub;
			cnt_nbr[i-1] = ub[i-1] > 1 ? 0 : ub[i-1];
		}
		nowid = nxtid;
		nowpos = nxtpos;
	}

	rf_idx.fclose();


	cnt_ub = new int[max_core+1];
	nbr = new int[max_deg];

	string st_dat = graph_path + "graph.dat";

	long tt = time(NULL);
	int cnt = 1;
	int iteration = 0;

	
	while( cnt > 0 ) {
		
		++iteration;
		cnt = 0;
		printf( "Iteration %d...\n", iteration );

		// if(iteration==2){
		// 	t = clock() - t;
		// 	printf( "it 2 time = %0.3lf sec\n", t/1000000.0 );
		// }
		
		
		

		MyReadFile rf(st_dat);
		rf.fopen( BUFFERED );

		MyReadFile rf_idx(st_idx);
		rf_idx.fopen( BUFFERED );
		

		int j, u, v, n_nbr, nowub, now_cnbr;

		fileint dlt = 0;
		for( u = 0; u < n; ++u ) {
			// if( (u+1) % 1000000 == 0 )
			// 	printf( "%0.3lf%%|", (u+1)*100.0/n );

			if( cnt_nbr[u] >= ub[u] )
				continue;

			
			
			//n_nbr = load_nbr(rf, u, nbr);
			n_nbr = load_nbr( rf, rf_idx, u, nbr );
			memset( cnt_ub, 0, sizeof(int) * (ub[u]+1) );

			
			for( j = 0; j < n_nbr; ++j ) {
				++dlt;
				v = nbr[j];
				++cnt_ub[ub[v]<ub[u]?ub[v]:ub[u]];

				
			}

			nowub = ub[u];
			ub[u] = get_core(cnt_ub, nowub, now_cnbr);
			
			

			cnt_nbr[u] = now_cnbr;
			
			if( ub[u] < nowub ) {
				++cnt;
				for( j = 0; j < n_nbr; ++j ) {
					v = nbr[j];
					if( nowub >= ub[v] && ub[u] < ub[v] ){
						
						
						--cnt_nbr[v];
					}
				}
			}
			
		}

		rf_idx.fclose();
		rf.fclose();
		//printf( "\ncnt=%d, t=%ldsec, dlt=%dM\n", cnt, time(NULL)-tt, (int)(dlt/1000000) );
	}

	// printf("iteration = %d\n",iteration );

	t = clock() - t;
	printf( "processing time = %0.3lf sec\n", t/1000000.0 );
}

Application::~Application() {
	if( ub != NULL )
		delete[] ub;
	if( cnt_nbr != NULL )
		delete[] cnt_nbr;
	//if( bulk_id != NULL )
	//	delete[] bulk_id;
	//if( bulk_pos != NULL )
	//	delete[] bulk_pos;
	if( cnt_ub != NULL )
		delete[] cnt_ub;
	if( nbr != NULL )
		delete[] nbr;
}

