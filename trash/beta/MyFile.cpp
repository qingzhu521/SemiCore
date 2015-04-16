/*
 * MyFile.cpp
 *
 *  Created on: Dec 30, 2013
 *      Author: luqin
 */

#include "MyFile.h"

////////////MyReadFile

MyReadFile::MyReadFile( const string &path ) {
	this->path = path;
	fin = -1;
	buf_pos = -1;
	pos = 0;
	mode = BUFFERED;
}

MyReadFile::MyReadFile() {
	fin = -1;
	buf_pos = -1;
	pos = 0;
	mode = BUFFERED;
}

void MyReadFile::set_file( const string &path ) {
	this->path = path;
	fin = -1;
	buf_pos = -1;
}

MyReadFile::~MyReadFile() {
	fclose();
}

bool MyReadFile::fopen(int mode) {
	this->mode = mode;
	fin = open( path.c_str(), O_RDONLY );
	if( fin < 0 )
		return false;
	pos = 0;
	if( mode == BUFFERED ) {
		buf_pos = 0;
		read( fin, buf, BUFSIZE );
	}
	return true;
}

void MyReadFile::fclose() {
	if( fin >= 0 ) {
		close( fin );
		fin = -1;
		buf_pos = -1;
	}
}

void MyReadFile::fseek( fileint pos ) {
	if( mode == BUFFERED ) {
		if( pos >= buf_pos + BUFSIZE || pos < buf_pos ) {
			buf_pos = (pos / BUFSIZE) * BUFSIZE;
			lseek( fin, buf_pos, SEEK_SET );
			read( fin, buf, BUFSIZE );
		}
	} else {
		lseek( fin, pos, SEEK_SET );
	}

	this->pos = pos;
}

void MyReadFile::fread( void *dat, fileint size ) {
	if( mode == BUFFERED ) {
		if( pos + size < buf_pos + BUFSIZE ) {
			memcpy( dat, buf + (pos - buf_pos), size );
		}
		else {
			memcpy( dat, buf + (pos-buf_pos), BUFSIZE-(pos-buf_pos) );
			while( buf_pos + BUFSIZE <= pos + size ) {
				buf_pos += BUFSIZE;
				read( fin, buf, BUFSIZE );
				if( buf_pos + BUFSIZE > pos + size && buf_pos != pos + size ) {
					memcpy( (char*)dat + (buf_pos - pos), buf, pos + (size - buf_pos) );
				}
				else if( buf_pos != pos + size ) {
					memcpy( (char*)dat + (buf_pos - pos), buf, BUFSIZE );
				}
			}
		}
	} else {
		read( fin, dat, size );
	}

	pos += size;
}

//////// MyWriteFile
MyWriteFile::MyWriteFile( const string &path ) {
	this->path = path;
	fout = -1;
	buf_pos = -1;
	pos = 0;
	mode = BUFFERED;
}

MyWriteFile::MyWriteFile() {
	fout = -1;
	buf_pos = -1;
	pos = 0;
	mode = BUFFERED;
}

void MyWriteFile::set_file( const string &path ) {
	this->path = path;
	fout = -1;
	buf_pos = -1;
}

MyWriteFile::~MyWriteFile() {
	fclose();
}

void MyWriteFile::fcreate( fileint size ) {
	size = ((size+BUFSIZE-1) / BUFSIZE + 1)* BUFSIZE;

	fout = open( path.c_str(), O_WRONLY | O_CREAT, S_IREAD|S_IWRITE );
	char* tmp = new char[BUFSIZE];
	memset( tmp, -1, BUFSIZE );
	for( fileint i = 0; i < size; i += BUFSIZE )
		write( fout, tmp, BUFSIZE );
	close( fout );
	fout = -1;
	delete[] tmp;
}

void MyWriteFile::fcreate( fileint size, char ch ) {
	size = ((size+BUFSIZE-1) / BUFSIZE + 1)* BUFSIZE;

	fout = open( path.c_str(), O_WRONLY | O_CREAT, S_IREAD|S_IWRITE );
	char* tmp = new char[BUFSIZE];
	memset( tmp, ch, BUFSIZE );
	for( fileint i = 0; i < size; i += BUFSIZE )
		write( fout, tmp, BUFSIZE );
	close( fout );
	fout = -1;
	delete[] tmp;
}

void MyWriteFile::fflush() {
	if( mode == BUFFERED ) {
		lseek( fout, buf_pos, SEEK_SET );
		write( fout, buf, BUFSIZE );
	}
}

bool MyWriteFile::fopen( int mode ) {
	this->mode = mode;
	pos = 0;
	if( mode == BUFFERED ) {
		buf_pos = 0;
		fout = open( path.c_str(), O_RDWR );
		if( fout < 0 )
			return false;
		read( fout, buf, BUFSIZE );
	} else {
		fout = open( path.c_str(), O_WRONLY );
		if( fout < 0 )
			return false;
	}
	return true;
}

void MyWriteFile::fclose() {
	if( fout < 0 )
		return;
	if( mode == BUFFERED ) {
		lseek( fout, buf_pos, SEEK_SET );
		write( fout, buf, BUFSIZE );
	}

	close( fout );
	fout = -1;
	buf_pos = -1;
}

void MyWriteFile::fseek( fileint pos ) {
	if( mode == BUFFERED ) {
		if( pos >= buf_pos + BUFSIZE || pos < buf_pos ) {
			lseek( fout, buf_pos, SEEK_SET );
			write( fout, buf, BUFSIZE );
			buf_pos = (pos / BUFSIZE) * BUFSIZE;
			lseek( fout, buf_pos, SEEK_SET );
			read( fout, buf, BUFSIZE );
		}
	} else {
		lseek( fout, pos, SEEK_SET );
	}

	this->pos = pos;
}

void MyWriteFile::fwrite( void *dat, fileint size ) {
	if( mode == BUFFERED ) {
		if( pos + size < buf_pos + BUFSIZE ) {
			memcpy( buf + (pos-buf_pos),  dat,  size );
		} else {
			memcpy( buf + (pos-buf_pos), dat, BUFSIZE-(pos-buf_pos) );
			while( buf_pos + BUFSIZE <= pos + size ) {
				lseek( fout, buf_pos, SEEK_SET );
				write( fout, buf, BUFSIZE );
				buf_pos += BUFSIZE;
				read( fout, buf, BUFSIZE );

				if( buf_pos + BUFSIZE > pos + size && buf_pos != pos + size ) {
					memcpy( buf, (char*)dat+buf_pos-pos, pos+(size-buf_pos) );
				}
				else if( buf_pos != pos + size ) {
					memcpy( buf, (char*)dat + (buf_pos - pos), BUFSIZE );
				}
			}
		}
	} else {
		write( fout, dat, size );
	}
	pos += size;
}

void MyWriteFile::fset( int pos_in_byte ) {
	if( pos_in_byte < 0 || pos_in_byte >= 8 )
		return;
	if( mode == BUFFERED )
		buf[pos-buf_pos] |= (1<<pos_in_byte);
	else {
		char tmp;
		read( fout, &tmp, sizeof(char) );
		tmp |= (1<<pos_in_byte);
		write( fout, &tmp, sizeof(char) );
	}
}

void MyWriteFile::fclear( int pos_in_byte ) {
	if( pos_in_byte < 0 || pos_in_byte >= 8 )
		return;
	if( mode == BUFFERED )
		buf[pos-buf_pos] &= (~(1<<pos_in_byte));
	else {
		char tmp;
		read( fout, &tmp, sizeof(char) );
		tmp &= (~(1<<pos_in_byte));
		write( fout, &tmp, sizeof(char) );
	}
}

