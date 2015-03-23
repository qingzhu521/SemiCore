#include<cstdio>
#include<cstring>
#include<string>
#include<ctime>

#include "Application.h"
#include "MyIO.h"
#include "MyFile.h"

using namespace std;

void test_sort_edge() {
	string tmp_dir = "/Users/dongwen/workspace/Hummingbird/sort_edge_tmp/";

	string sep = ",";
	// string file = "/Users/dongwen/workspace/SemiKCore/dataset/uk-2002.txt";
	string file = "/Users/dongwen/workspace/Hummingbird/dataset/ljournal-2008.txt";
	string output_dir = "/Users/dongwen/workspace/Hummingbird/dataset/";


	int max_mem_edge = 100000000;
	MyIO::sort_edge(file, sep, tmp_dir, output_dir, max_mem_edge);
}

void test_semi_core() {
	Application *app = new Application;

	string graph_path = "/Users/dongwen/workspace/Hummingbird/dataset/";
	
	app->load_graph_idx( graph_path );
	//app->test_graph_scan();
	app->process_basic();
	app->print_distribution();
	delete app;
}


int main(){
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	printf( "---start-------\n" );
	// long t = clock();

	test_sort_edge();
	test_semi_core();

	// t = clock() - t;
	printf( "---end-------\n" );
	// printf( "total time=%0.3lf sec", t/1000000.0);
	return 0;
}
