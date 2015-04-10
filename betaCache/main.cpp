#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){

	
	Application* app = new Application("/Users/dongwen/workspace/Hummingbird/");

	// app->sortEdge("/Users/dongwen/workspace/Hummingbird/dataset/uk-2002.txt");
	app->semiKCore();
	app->printCoreDistribution();

	return 0;
}