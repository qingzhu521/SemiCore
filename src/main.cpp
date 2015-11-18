#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){
	// This is the path of graph file (graph.idx, graph.dat and graph.info files)
	Application* app = new Application("/Users/DongWen/workspace/dataset/lj/");

	// process .txt file to graph.idx, graph.dat and graph.info
	// first parameter is path of .txt
	// second parameter is used to sample graph (100 means keep 100% of origin graph, i.e. don't sample)
	// third parameter is sampling edges if true, or sampling nodes if false (it does not matter if second parameter is 100)
	app->sortEdge("/Users/DongWen/workspace/kcore/dataset/test.txt",100,true);
	
	app->semiKCore();
	// app->semiKCoreNaive();
	// app->semiKCoreNaivePlus();
	// app->saveCore();


	// for dynamic graph
	// app->dynamicCore(100);
	
	
	app->printCoreDistribution();

	return 0;
}