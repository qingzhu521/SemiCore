#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){

	
	Application* app = new Application("/Users/dongwen/workspace/dataset/lj/");

	//app->sortEdge("/Users/dongwen/workspace/dataset/com-lj.ungraph.txt",100);
	
	// app->getGraphInfo();
	app->semiKCore();
	//app->saveCore();
	// app->loadCore();
	// app->dynamicCore(100);
	
	app->printCoreDistribution();

	return 0;
}