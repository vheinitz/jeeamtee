/**
The MIT License

Copyright (c) 2010 Valentin Heinitz, http://heinitz-it.de

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef _GMT_H_
#define _GMT_H_

#include <bitset>
#include <limits>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "flagdata.h"


#define READALL( strm, str ) for ( std::string str; strm.good() && !strm.eof(); strm >> str )
#define READALL2( strm, str,str1 ) for ( std::string str, str1; strm.good() && !strm.eof(); strm >> str>>str1 )

typedef unsigned long RegisterType; 

/**
Structure holding a register attributes like access mode and offset within memory
*/
struct Register
{
    enum AccessType{RW=0,RO=1,WO=2};
    AccessType _type;
    RegisterType _addr;
};

/**
\brief Generic Memory Tool - accessing adata in memory in human-friendly way.

GMT allows to displays and modify data in memory using simple commands.
The class is intended to be used in console tools, therefore the commands are 
human readable strings.

With GMT you can:
 -dump memory in human friendly form. GMT displays memory in hex- and binary representation
  along with interpretation of data in memory
 -set/clear one bit in memory
 -assign a value to a word of particular word-length

In order to show data in human friendly form GMT should be able to interprete the data.
GMT interpretes the data using data description. The data is described as human
readable text. See parseConfiguration() for the syntax.

The basic usage of GMT is as follows:


*/
class GMT
{
 	RegisterType * _mem;
   RegisterType * _mem_device;
 	RegisterType _offset;
 	typedef std::map<std::string, Register > TRegisterMap;
 	TRegisterMap _registerMap;
 	std::list <std::string> _registers;
 	typedef std::list<FlagData<RegisterType> > TRegisterFlags;
	typedef std::map<std::string, TRegisterFlags> TRegisterFlagMaps;
	TRegisterFlagMaps _registerFlagMaps;
	typedef std::list<std::string> TStringList;

public: 	
   
   GMT(  );
   void setMemoryPointer( RegisterType * m );
   void setOffset( RegisterType offset );
   const std::list <std::string> & registers()const{ return _registers;}
 	void readConfiguration( const std::string & fname );
   void parseConfiguration( const std::string & fname );
 	int memsize() const;
   RegisterType registerValue( const std::string & rname ) const;
   void setRegisterValue( const std::string & rname, const std::string & value );
   void setRegisterValue( const std::string & rname, RegisterType value );
   void processBitCommand( const std::string &cmd, const std::string & reg, const std::string & expr );
   void processCommand( const std::string & cmd);
	void readRegisterFlag( const std::string & flagfile );
	void printUsedFlags( const std::string &reg, RegisterType value );
};

#endif /*GMT_H_*/
