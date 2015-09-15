#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){

	
	Application* app = new Application("/Users/dongwen/workspace/Hummingbird/dataset/test/");

	//app->sortEdge("/Users/dongwen/workspace/Hummingbird/dataset/test.txt");
	//printf("Edges: %u\n",app->getEdgeNumber());
	app->semiKCore();
	app->saveCore();
	//app->loadCore();
	//app->dynamicCore(100);
	
	//app->printCoreDistribution();

	return 0;
}