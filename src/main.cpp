#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){

	
	Application* app = new Application("/Users/dongwen/workspace/Hummingbird/dataset/test/");

	//app->sortEdge("/Users/dongwen/workspace/Hummingbird/dataset/enron.txt");
	
	app->semiKCore();
	app->dynamicCore();
	app->printCoreDistribution();

	return 0;
}