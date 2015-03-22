/*
 * Application.h
 *
 *  Created on: Dec 27, 2013
 *      Author: luqin
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include<vector>
#include<cstring>
#include<string>
#include<cstdio>
#include<ctime>

#include "MyFile.h"
#include "MyIO.h"

using namespace std;

class Application {
public:
	Application();
	virtual ~Application();

public:
	void load_graph_idx(string path);
	void print_distribution();
	void process_basic();
	//void test_graph_scan();

private:
	//int get_len(int p);
	int get_len( char nowid, int nowpos, char nxtid, int nxtpos );
	int get_core(int *cnt_b, int ub, int& n_nbr);
	//int load_nbr(MyReadFile &rf, int p, int *nbr);
	int load_nbr(MyReadFile &rf, MyReadFile &rf_idx, int p, int *nbr);

private:
	const static int max_core = 30000;
	int n;
	int max_deg;

	short *ub;		//size n
	short *cnt_nbr;	//size n

	//char *bulk_id;	//size n;
	//int *bulk_pos;	//size n;

	int *cnt_ub;	//size max_core
	int *nbr;		//size max_deg;

	string graph_path;
};

#endif /* APPLICATION_H_ */
