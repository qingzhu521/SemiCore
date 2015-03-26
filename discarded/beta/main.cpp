#include <cstdio>
#include <string>
#include "Application.h"

using namespace std;

int main(){


	Application* app = new Application("/Users/dongwen/workspace/Hummingbird/");

	//app->sortEdge("/Users/dongwen/workspace/Hummingbird/dataset/ljournal-2008.txt");
	app->semiKCore();

	return 0;
}