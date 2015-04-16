/*
 * MyIO.h
 *
 *  Created on: Dec 28, 2013
 *      Author: luqin
 */

#ifndef MYIO_H_
#define MYIO_H_

#include<string>
#include<cstring>
#include<vector>
#include<map>
#include "MyFile.h"

using namespace std;

class Edge {
public:
	int a, b;

public:
	Edge();
	Edge(int a, int b);
};

bool edge_compare(const Edge &e1, const Edge &e2);

class MyIO {
public:
	MyIO();
	virtual ~MyIO();

public:
	static const unsigned bulk_size = ((unsigned)2) * 1024 * 1024 * 1024;
	static const int max_node_id = 300000000;

private:
	static void save_tmp_edge(string tmp_dir, Edge *mem_edge, int &m, int &n);
	static void merge(string tmp_dir, string output_dir, int n);
	static void save_buf_edge(FILE *fout, int u, vector<int> &adj, vector<fileint> &pos, fileint &nowpos);
	static int sort_edge_tmp(string file, string sep, string tmp_dir, int max_mem_edge);
	static void output_idx(vector<fileint> &pos, string output_dir);
	static int get_node_id(int *m_nid, int &n_node, int p);

public:
	static void sort_edge(string file, string sep, string tmp_dir, string output_dir, int max_mem_edge);
};

#endif /* MYIO_H_ */
