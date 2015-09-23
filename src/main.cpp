#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){
	
	Application* app = new Application("/Users/DongWen/workspace/dataset/lj/");

	//app->sortEdge("/Users/DongWen/workspace/Hummingbird/dataset/test.txt",100);
	
	// app->getGraphInfo();
	//app->semiKCore();
	//app->saveCore();
	app->loadCore();
	// app->dynamicCore(100);
	
	
	app->printCoreDistribution();

	return 0;
}