#include <string>
#include <fstream>
#include <iostream>
#include "lz4.h"

using namespace std;

int main()
{
    char szString[] = "2013-01-07 00:00:04,0.98644,0.98676 2013-01-07 00:01:19,0.98654,0.98676 2013-01-07 00:01:38,0.98644,0.98696";
    const char* pchSource = szString;
    int nInputSize = sizeof(szString);

    cout <<"- pchSource -" << endl << pchSource << endl;
    cout <<"nbytes = "<< nInputSize << endl << endl;
    ofstream source("pchSource.txt");
    source << pchSource;

    int nbytesPassed = 0;
    int nMaxCompressedSize = LZ4_compressBound(nInputSize);

    char *pszDest = new char[nMaxCompressedSize];
    nbytesPassed = LZ4_compress(pchSource, pszDest, nInputSize);
    cout <<"- pszDest Compressed-" << endl;
    cout <<"nbytesPassed = "<< nbytesPassed << endl;
    cout << pszDest << endl << endl;
    // pszDest garbage ?

    char *pszDestUnCompressed = new char[nInputSize];
    LZ4_uncompress(pszDest, pszDestUnCompressed, nInputSize);
    cout <<"- pszDestUnCompressed -" << endl;
    cout <<"nbytesPassed = "<< nbytesPassed << endl;
    cout << pszDestUnCompressed << endl << endl;
    //pszDestUnCompressed is correct ?

    delete[] pszDestUnCompressed;
    pszDestUnCompressed = 0;

    // ok lets write compressed pszDest to pszDest.dat
    ofstream outCompressedFile("pszDest.dat",std::ofstream::binary);
    outCompressedFile.write (pszDest,nMaxCompressedSize);

    delete[] pszDest;
    pszDest = 0;

    //read it back in and try to uncompress it
    ifstream infile("pszDest.dat",std::ifstream::binary);
    infile.seekg (0,infile.end);
    int nCompressedInputSize = infile.tellg();
    infile.seekg (0);

    char* buffer = new char[nCompressedInputSize];
    infile.read (buffer,nCompressedInputSize);
    const char* pchbuffer = buffer;

    char* pszUnCompressedFile = new char[nInputSize];
    nbytesPassed = LZ4_uncompress(pchbuffer, pszUnCompressedFile, nInputSize);
    cout <<"- pszUnCompressedFile -" << endl;
    cout <<"nbytesPassed = "<< nbytesPassed << endl;
    cout << pszUnCompressedFile << endl;

    //write uncompressed pszDest.dat to pszUnCompressedFile.txt
    ofstream outUncompressedSource("pszUnCompressedFile.txt");
    outUncompressedSource << pszUnCompressedFile;
    // On my system 32bit ArchLinux 3.7.10-1 - gcc 4.7.2-4
    // file contains random Garbage

    delete[] buffer;
    buffer = 0;

    delete[] pszUnCompressedFile;
    pszUnCompressedFile = 0;

    return 0;
}