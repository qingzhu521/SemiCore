#include <cstdio>
#include <cstring>
#include "Application.h"

using namespace std;

int main(){

	char* base = "/Users/dongwen/workspace/SemiKCore/";

	Application* app = new Application(base);

	app->sortEdge("/Users/dongwen/workspace/SemiKCore/dataset/l-journal-2008.txt",100000000);


	return 0;
}