/*
 * MYIO.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: luqin
 */

#include "MyIO.h"

bool edge_compare(const Edge &e1, const Edge &e2) {
	if( e1.a < e2.a )
		return true;
	if( e1.a > e2.a )
		return false;
	return e1.b < e2.b;
}

Edge::Edge(int a, int b) {
	this->a = a;
	this->b = b;
}

Edge::Edge(){
	this->a = 0;
	this->b = 0;
}

void MyIO::save_tmp_edge(string tmp_dir, Edge *mem_edge, int &m, int &n){
	printf( "sorting\n" );
	sort( mem_edge, mem_edge + m, edge_compare );
	char st[1024];
	sprintf( st, "%sedges_%d", tmp_dir.c_str(), n );
	printf( "creating file %s\n", st );

	FILE* fout = fopen( st, "wb" );
	for( int i = 0; i < m; ++i )
		fwrite( mem_edge+i, sizeof(Edge), 1, fout );
	fclose( fout );

	++n;
	m = 0;
	printf( "finished write mem edge\n" );
}

void MyIO::save_buf_edge(FILE *fout, int u, vector<int> &adj, vector<fileint> &pos, fileint &nowpos) {
	while( pos.size() < u )
		pos.push_back( -1 );
	pos.push_back( nowpos );

	int len = (int) adj.size();

	//fwrite( &len, sizeof(int), 1, fout );
	//nowpos += sizeof(int);

	for( int i = 0; i < len; ++i ) {
		int val = adj[i];
		fwrite( &val, sizeof(int), 1, fout );
		nowpos += sizeof(int);
	}

	adj.clear();
}

void MyIO::output_idx(vector<fileint> &pos, string output_dir){
	char st[1024];
	sprintf( st, "%sgraph.idx", output_dir.c_str() );

	FILE *fout = fopen(st, "wb");
	int len = (int)pos.size();
	fwrite( &len, sizeof(int), 1, fout);
	for( int i = 0; i < len; ++i ) {
		char block_id = -1;
		int pos_in_block = -1;

		if( pos[i] >= 0 ) {
			block_id = (char) (pos[i]/bulk_size);
			pos_in_block = (int) (pos[i]%bulk_size);
		}
		fwrite( &block_id, sizeof(char), 1, fout);
		fwrite( &pos_in_block, sizeof(int), 1, fout );
	}
	fclose( fout );
}

void MyIO::merge(string tmp_dir, string output_dir, int n){
	FILE** fin = new FILE*[n];
	char st[1024];
	for( int i = 0; i < n; ++i ) {
		sprintf( st, "%sedges_%d", tmp_dir.c_str(), i );
		fin[i] = fopen( st, "rb" );
	}

	vector<fileint> pos;
	vector<int> adj;
	fileint nowpos = 0;

	sprintf( st, "%sgraph.dat", output_dir.c_str() );

	FILE *fout = fopen(st, "wb");
	Edge *e = new Edge[n];
	for( int i = 0; i < n; ++i )
		fread( e+i, sizeof(Edge), 1, fin[i] );

	fileint cnt = 0;
	Edge pre(-1,-1);
	while( true ){
		Edge now(-1,-1);
		int p = -1;
		for( int i = 0; i < n; ++i)
			if( e[i].a >= 0 )
				if( now.a < 0 || edge_compare(e[i], now) ) {
					p = i;
					now = e[i];
				}
		if( now.a < 0 )
			break;
		if( feof( fin[p] ) )
			e[p] = Edge(-1,-1);
		else
			fread(e+p, sizeof(Edge), 1, fin[p]);

		if( now.a != pre.a && pre.a >= 0 )
			save_buf_edge( fout, pre.a, adj, pos, nowpos );

		if( adj.size() == 0 || now.b != pre.b )
			adj.push_back( now.b );
		pre = now;

		++cnt;
		if( cnt < 1000 )
			printf( "[%d,%d]", now.a, now.b );
		if( cnt < 1000 && cnt % 20 == 0 )
			printf( "\n" );
		if( cnt % 1000000 == 0 )
			printf( "[%d]", (int)(cnt/1000000) );
		if( cnt % 20000000 == 0 )
			printf( "\n" );
	}

	if( adj.size() > 0 )
		save_buf_edge( fout, pre.a, adj, pos, nowpos );

	++pre.a;
	save_buf_edge( fout, pre.a, adj, pos, nowpos );

	delete[] e;
	fclose( fout );

	for( int i = 0; i < n; ++i )
		fclose( fin[i] );

	delete[] fin;

	for( int i = (int)pos.size()-2; i >= 0; --i )
		if( pos[i] < 0 )
			pos[i] = pos[i+1];

	printf( "\n" );
	for( int i = 0;  i < 100 && i < (int) pos.size(); ++i )
		printf( "[%d:%ld]", i, pos[i] );
	printf( "\n" );

	output_idx( pos, output_dir );
}

int MyIO::get_node_id(int *m_nid, int &n_node, int p){
	if( m_nid[p] < 0 ) {
		m_nid[p] = n_node++;
		return n_node-1;
	}
	return m_nid[p];
}

int MyIO::sort_edge_tmp(string file, string sep, string tmp_dir, int max_mem_edge) {
	Edge *mem_edge = new Edge[max_mem_edge];

	//map<int,int> m_nid;
	int* m_nid = new int[max_node_id];
	memset( m_nid, -1, sizeof(int) * max_node_id );

	int n_node = 0;
	int n = 0;
	int m = 0;
	string fmt = "%d" + sep + "%d";
	FILE *fin = fopen( file.c_str(), "r" );

	char line[1024];
	int cnt = 0;
	while( fgets(line, 1024, fin) ) {
		if( line[0] < '0' || line[0] > '9' )
			continue;

		int u, v;
		sscanf( line, fmt.c_str(), &u, &v );
		u = get_node_id(m_nid, n_node, u);
		v = get_node_id(m_nid, n_node, v);

		++cnt;
		if( cnt % 1000000 == 0 )
			printf( "[%d]", cnt/1000000 );

		mem_edge[m].a = u;
		mem_edge[m].b = v;
		++m;
		mem_edge[m].a = v;
		mem_edge[m].b = u;
		++m;

		if( m == max_mem_edge )
			save_tmp_edge( tmp_dir, mem_edge, m, n );
	}

	fclose( fin );

	if( m > 0 )
		save_tmp_edge( tmp_dir, mem_edge, m, n );

	delete[] mem_edge;
	delete[] m_nid;

	return n;
}

void MyIO::sort_edge(string file, string sep, string tmp_dir, string output_dir, int max_mem_edge) {
	int n = sort_edge_tmp(file, sep, tmp_dir,  max_mem_edge);
	merge( tmp_dir, output_dir, n );
}

MyIO::MyIO() {
}

MyIO::~MyIO() {
}

