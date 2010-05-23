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

#include "gmt.h"
#include <map>
using std::map;


/**
Helper class for help information output.
*/
class Cmd
{
friend bool operator==( const std::string & str, const Cmd& cmd  );
	typedef map<std::string, Cmd> CmdMap;
	static CmdMap _cmap;
	std::string _name;
	std::string _help;
	std::string _syntax;
	public:
	Cmd( ){	} // required by map
	Cmd( const char * name, const char *help, const char *syntax=0 ): _name(name), _help(help), _syntax(syntax) {
		_cmap[_name] = *this ;
	}
	
	static void printHelp( std::ostream & str, const std::string &cmd )
	{
		CmdMap::const_iterator it=_cmap.find( cmd );
		if( it == _cmap.end() )
		{			
			str << "Usage: Cmmand [Parameter]\n";
			str << "Attention, no error information is outout on parameter error.\n";
			str << "Empty command (RETURN-press) repeats last command.\n";
			str << "\nFor obtaining help on command type: h [command]\n";
			str << "Available commands:\n";
			for ( CmdMap::const_iterator it=_cmap.begin(),end=_cmap.end(); it!=end; it++ )
			{
				str <<"\t"<< (*it).first <<"\t"<<(*it).second._help << std::endl;
			}			
		}
		else
		{
			str <<(*it).second._help;
			if ( !(*it).second._syntax.empty() )
			{
				str << "\nSyntax:\n"<<(*it).second._syntax << std::endl;
			}
		}
	}	
};

Cmd::CmdMap Cmd::_cmap;

bool operator==( const std::string & str, const Cmd& cmd  )
{
	return str == cmd._name;
}

/**
Default constructor
*/
GMT::GMT(  ):_mem(0),_mem_device(0),_offset(0)
{
}

/**
Default constructor
*/
void GMT::setMemoryPointer( RegisterType * m )
{
   _mem_device=m;
   _mem=_mem_device;
   _mem+=_offset;
}

/**
Sets offset from where the mapping of register begins.
*/
void GMT::setOffset( RegisterType offset )
{
   _offset=offset;
   _mem=_mem_device;
   _mem+=_offset;
}

void GMT::processCommand( const std::string & cl )
{
    std::string  cmdline(cl);
    std::string  cmd;
    cmdline +=" 0"; // Finishing FORALL
    std::stringstream sstr(cmdline);
    sstr >> cmd;
    
    //todo: Cmd-objects are created on the fly each time - slow. Could be created before, e.g. as global objects. 
    if (cmd == Cmd(	"p",
 			   "Print register(s) information.",
 			   "\tp REG1 [REG2 ...]" ) )
    {
        READALL( sstr, str )
        {
            if (str.empty())continue;
            std::cout<<std::left<< std::setw(20)<< str<<std::setw(1)
                    <<std::bitset<std::numeric_limits<RegisterType>::digits>(registerValue(str)).to_string().c_str()
                    <<"  0x"<<std::hex<<registerValue(str)
                    <<std::endl;
        }
    }
    else if (cmd == Cmd(	"pn",
 				   "Print register(s) information n-times with a pause(ms). Usefull for polling.",
 				   "\tpn Times Pause REG1 [REG2 ...]" ) )
    {
 	   int times =0, pause=0;
 	   sstr >> times >> pause;
 	   std::list<std::string> regs;    	
 	   READALL( sstr, str ){ if (str.empty())continue; regs.push_back(str); } 
 	   for ( int t=0; t<=times; ++t )
 	   {
 	      //todo: usleep( pause*1000 );
          for ( std::list<std::string>::const_iterator it =  regs.begin(), end = regs.end(); it!=end; ++it)
          {
               std::cout<<std::left<< std::setw(20)<< *it <<std::setw(1)
	               <<std::bitset<std::numeric_limits<RegisterType>::digits>(registerValue(*it)).to_string().c_str()
	               <<"  0x"<<std::setw(8)<<std::hex<<registerValue(*it)<<": ";
	               printUsedFlags(*it, registerValue(*it));
	               std::cout<<std::endl;
           }
 	   }
    }

    else if (cmd == Cmd(	"d",
 				   "Dump all registers","" ) )
    {
        const std::list <std::string> & reglist =  registers();
        for ( std::list <std::string>::const_iterator it =  reglist.begin(), end =  reglist.end(); it != end; ++it)
        {
            std::cout<<std::left<< std::setw(20)<< *it <<std::setw(1)
                    <<std::bitset<std::numeric_limits<RegisterType>::digits>(registerValue(*it)).to_string().c_str()
                    <<"  0x"<<std::setw(8)<<std::hex<<registerValue(*it)<<": ";
                    printUsedFlags(*it, registerValue(*it));
                    std::cout<<std::endl;
        }
    }

    else if (cmd == Cmd(	"s",
 				   "Set register value (Decimal).",
 				   "\ts REG Value" ) )
    {
        READALL2( sstr, reg, value )
        {
            if (reg.empty() || value.empty())continue;
            setRegisterValue( reg, value );
        }
    }

    else if (	cmd == Cmd(	"sb",
 				   "Set bit(s) in a register.",
 				   "\tsb REG BitNr [BitNr ...]"\
 				   "\n\tsb REG FLAG[=VALUE]"\
 				   "\n\tsb REG DATA=VALUE" )
 		   ||
 		   cmd == Cmd(	"cb",
 				   "Clear bit(s) in a register.",
 				   "\tcb REG BitNr"\
 				   "\n\tcb REG FLAG" ) )
    {
 	   std::string reg;
        sstr >> reg;
        READALL( sstr, value )
        {
            if ( value.empty())continue;
            std::stringstream str(value);
            std::string expr;
            str >> expr;            
            processBitCommand( cmd, reg, expr);                        
        }
    }
   else if (cmd == Cmd(	"r",
    				"Read commands from file.",
    				"\tr FileName" ) )
   {
		std::string fname;
		sstr >> fname; // file name to read
		std::ifstream ifs(fname.c_str());
		if( !ifs.good() || ifs.eof() )
		{
			std::cerr << "ERROR: can't read commands-file "<< fname<< std::endl;
			return ;
		}
		while( ifs.good() && !ifs.eof() )
		{			
			std::string read_cmd_line;
			std::getline(ifs, read_cmd_line);
			processCommand( read_cmd_line );
	    }
	}        
   else if (cmd == Cmd(	"h",
			   "Print help.",
			   "\th [Command]") ) 
   {
      std::string cmd;
      sstr >> cmd; // file name to read
      Cmd::printHelp( std::cout, cmd );
   }
}


/**
Reading memory-data format description from file.
*/
void GMT::readConfiguration( const std::string & fname )
{
	std::ifstream is(fname.c_str());
   std::string confData;
   while ( is.good() && !is.eof() )
	{
       std::string line;
	    std::getline( is, line );
       confData+="\n";
       confData+=line;
   }
   parseConfiguration( confData );
}

/**
Reading memory-data format description from string data.
Syntax:
 Lines beginning with # are comments
 Register definition:
    reg <REGISTER_NAME> <OFFSET> <MODE> [#Comment]
    REGISTER_NAME is a string, OFFSET is hex-number and MODE is [rw]
 Flags definition
    for_regs <REGISTER_NAME> [<REGISTER_NAME> ...] [#Comment]
     flag <FLAG_NAME> <starting bit> <number of bits> <value> [#Comment]
     data <FLAG_NAME> <starting bit> <number of bits> [#Comment]
  FLAG_NAME is a string, other parameters are decimal numbers
*/
void GMT::parseConfiguration( const std::string & confData )
{
   _registerMap.clear();
   _registers.clear();
	std::string regname;
	Register reg;
	TStringList regs;
	int regtype;
	std::stringstream is(confData);
	while ( is.good() && !is.eof() )
	{
       std::string line, cmd;
	    std::getline( is, line );
	    std::stringstream sstr(line);

		if ( line.empty() || line[0]=='#' ) continue;
		cmd.clear();
		sstr >> cmd;
		if( cmd == "reg" )
		{
			regs.clear(); //reset previous for_regs instruction
			sstr >> regname >> std::hex>> reg._addr >> regtype;
			if ( regtype<Register::RW || regtype>Register::WO )
			{
           		regtype=Register::RW;// todo: Wrong file format: Invalid register type
   		}
   		reg._type = static_cast<Register::AccessType>(regtype);
			_registerMap[regname]=reg;
			_registers.push_back(regname);
		}
		else if( cmd == "for_regs" )
		{
			regs.clear();
			std::string reg;
			while ( sstr.good() && !sstr.eof() )
			{
				sstr >> reg;
				regs.push_back( reg );
			}
		}
		else if( cmd == "flag" )
		{
			std::string flag;
			RegisterType from=-1, bits=-1, val=-1;
			sstr >> flag >> from >> bits >> val;
			if ( from>=0 && bits>=0 && val>=0 )
			{
				for ( TStringList::const_iterator it = regs.begin(), end=regs.end(); it!=end; ++it )
				{
					TRegisterFlags &rflags = _registerFlagMaps[*it];
					rflags.push_back( FlagData<RegisterType>( flag, from, bits, val ) );				
				}
			}
			else
			{
				std::cerr << "ERROR: wrong format in flags-file"<<std::endl;
			}
		}
		else if( cmd == "data" )
		{
			std::string flag;
			RegisterType from, bits;
			sstr >> flag >> from >> bits ;
			if ( sstr.good() )
			{
				for ( TStringList::const_iterator it = regs.begin(), end=regs.end(); it!=end; ++it )
				{
					TRegisterFlags &rflags = _registerFlagMaps[*it];
					rflags.push_back( FlagData<RegisterType>( flag, from, bits, 0, true ) );
				}
			}
		}
	}
}

/**
Calculated the size of the entire memory - memory defined by 
format description plus offset. You may require it for mem-mapping
a block of appropriate size.
*/
int GMT::memsize() const
{
    int max=0;
    for ( TRegisterMap::const_iterator it=_registerMap.begin(), end=_registerMap.end(); it!=end; ++it)
    {
        if( int((*it).second._addr) > max)
        {
            max = (*it).second._addr;
        }
    }
    return max+_offset;
}

/**
Obtain register value.
*/
RegisterType GMT::registerValue( const std::string & rname ) const
{
    TRegisterMap::const_iterator it =  _registerMap.find(rname);
    if( it ==  _registerMap.end() )
    {
        std::cout <<"ERROR: Undefined Register:[" <<rname.c_str()<<"] while requesting register value."<<std::endl;
        return RegisterType(0xdeadbeef);
    }
    return *(((RegisterType*)_mem) + (*it).second._addr);
}

/**
Setting entire register from string-number.
*/
void GMT::setRegisterValue( const std::string & rname, const std::string & value )
{
    std::stringstream tmp(value);
    RegisterType val;
    tmp >> val;
    setRegisterValue(rname, val);
}

/**
Setting entire register.
*/
void GMT::setRegisterValue( const std::string & rname, RegisterType val )
{
    TRegisterMap::iterator it =  _registerMap.find(rname);
    if( it ==  _registerMap.end() )
    {
        std::cerr <<"ERROR: Undefined Register:[" <<rname.c_str()<<"] while setting register value:"<<val <<std::endl;
        return;
    }
    (*(((RegisterType*)_mem) + (*it).second._addr)) = val;
}

/**
Processing a bit-manipulation command
*/
void GMT::processBitCommand( const std::string &cmd, const std::string & reg, const std::string & expr )
{
	RegisterType ival=registerValue( reg );
	if ( expr.empty()  )
	{
		return;
	}
	else if ( expr[0] >='0' && expr[0] <='9' )
	{
		
		std::stringstream ts(expr);
		RegisterType bitnr;
		ts >> bitnr;	
		if (cmd == "sb")
		{	  
			ival |= 1<<bitnr;
		}
		else if(cmd == "cb")
		{
			ival &= ~(1<<bitnr);
		}    	
	}

	else // possibly flag
	{
		std::string flag=expr, val;
		size_t dp = expr.find('=');
		if( dp != std::string::npos )
		{
			flag=expr.substr(0,dp);
			val=expr.substr(dp+1, std::string::npos);
		}
		TRegisterFlags &rflags = _registerFlagMaps[reg];
		for ( TRegisterFlags::const_iterator it = rflags.begin(), end = rflags.end(); it!= end; ++it )
		{
			if( it->name() == flag )
			{
				
				ival &= ~( it->mask() << it->start() );
				RegisterType tmpval = it->value();
				if(!val.empty())
				{
					std::stringstream ts(val);
					ts >> tmpval;
				}
				if (cmd == "sb")
	    		{	  
	    			ival |= ( tmpval << it->start() );
	    		}
	    		else if(cmd == "cb")
	    		{
	    			// remains 0
	    		}
	    		break;
			}	
		}
	}    	
	setRegisterValue( reg, ival );
}

/**
Output flags along with their values for given register
*/
void GMT::printUsedFlags( const std::string &reg, RegisterType regval )
{
	TRegisterFlagMaps::iterator it = _registerFlagMaps.find( reg );
	if ( it != _registerFlagMaps.end() )
	{
		for ( TRegisterFlags::const_iterator fit = it->second.begin(), end = it->second.end(); fit!=end; ++fit )
		{
			const std::string & tv = fit->getFlagString(regval);
			if (!tv.empty()){
				std::cout << tv <<" ";
			}
		}
	}
}
