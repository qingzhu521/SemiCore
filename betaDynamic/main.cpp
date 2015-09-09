#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){

	
	Application* app = new Application("/Users/dongwen/workspace/Hummingbird/dataset/test/");

	//app->sortEdge("/Users/dongwen/workspace/Hummingbird/dataset/test.txt");
	
	app->semiKCore();

	
	app->printCoreDistribution();

	printf("add an edge: 0 3\n");
	app->addEdge(0,3);
	app->printCoreDistribution();


	return 0;
}