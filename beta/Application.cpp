#include "Application.h"

Application::Application(char* base){
	m_base = base;
}
Application::Application(){

}
Application::~Application(){

}

void Application::sortEdge(char* txtFile, int maxID){

	FILE* fp = fopen(txtFile,"w+");

	m_vertexMap = new int[maxID];
	memset(m_vertexMap,-1,sizeof(int)*maxID);
	// save every line read from txtFile
	char line[50];

	int u,v;

	while(fgets(line,50,fp)){
		if( line[0] < '0' || line[0] > '9' )
			continue;
		sscanf(line,"%d,%d",)
	}


	fclose(fp);
}
