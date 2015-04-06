/*
 * FotonFile.h
 *
 *  Created on: Apr 5, 2015
 *      Author: Christopher
 */

#ifndef FOTONFILE_H_
#define FOTONFILE_H_


enum StreamType{
	READ,
	WRITE,
	READ_WRTIE
};


extern void initializeSystemMemory();

class FotonFile {
public:
	FotonFile();
	~FotonFile();
	bool open(char * file_path_name, StreamType stream_direction );
    long readFor(char** buffer, long read_length);
    char ** readAll();
    bool write(char ** data_out);
    bool writeFor(char ** data_out, long length);
    bool seekTo(long index);
    bool close();

private:

	bool mOpen;

};

#endif /* FOTONFILE_H_ */
