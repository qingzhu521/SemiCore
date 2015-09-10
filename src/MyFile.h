/*
 * MyFile.h
 *
 *  Created on: Dec 30, 2013
 *      Author: luqin
 */

#ifndef MYFILE_H_
#define MYFILE_H_

#include<stdio.h>
#include<string.h>
#include<string>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 4096
#define BUFFERED 1
#define NOBUFFER 0

typedef long fileint;

using namespace std;


class MyReadFile {
private:
	int fin;
	string path;
	char buf[BUFSIZE];
	fileint pos;
	fileint buf_pos;
	int mode;//0:direct 1:buffered
	fileint total_io;
public:
	MyReadFile();
	MyReadFile(const string &path);
	MyReadFile(const string &path, int mode);
	~MyReadFile();
	void set_file(const string &path);
	bool fopen( int mode );
	bool fopen(const string &path, int mode);
	void fseek( fileint pos );
	void fread( void *dat, fileint size );
	void fclose();
	fileint get_total_io();
};

class MyWriteFile {
private:
	int fout;
	string path;
	char buf[BUFSIZE];
	fileint pos;
	fileint buf_pos;
	int mode;//0:direct 1:buffered
	fileint total_io;
public:
	MyWriteFile();
	MyWriteFile(const string &path);
	MyWriteFile(const string &path, int mode);
	~MyWriteFile();
	void set_file(const string &path);
	void fcreate( fileint size );
	void fcreate( fileint size, char ch );
	void fflush();
	bool fopen( int mode );
	bool fopen(const string &path, int mode);
	void fseek( fileint pos );
	//void fread( void *dat, fileint size );
	void fwrite( void *dat, fileint size );
	void fset( int pos_in_byte ); //set the pos'th bit in the current byte
	void fclear( int pos_in_byte );//clear the pos'th bit in the current byte
	void fclose();
	fileint get_total_io();
};

#endif /* MYFILE_H_ */
