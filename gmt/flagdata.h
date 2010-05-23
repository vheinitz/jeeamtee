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

#ifndef _FLAG_DATA_H_
#define _FLAG_DATA_H_

#include <string>

static const std::string _snull;  ///<Pseudo null string value

/**
\brief Class holding data for one flag

The flag could consist of one or several bits.
Contains flag-name, flag-value and position of flag within register.
*/
template<typename RegType> // Should be some integer type.
class FlagData
{			
	std::string _name;   ///<Flag name
	RegType _startbit;    ///<Beginning of flag.
	RegType _mask;        ///<Bitmask matching the flag.
	RegType _value;       ///<
	bool _anyvalue;		///<
	mutable std::string _svalue;      ///<String value
public:
	
   ///Flag name
   const std::string & name() const
   {
      return _name;
   }

   ///Offset of flag within register
   RegType start()const
   {
      return _startbit;
   }

   /**
   Bitmask-signature of flag.
   todo: document
   */
   RegType mask()const
   {
      return _mask;
   } 

   /**
   Value of flag.
   todo: document
   */
   RegType value()const
   {
      return _value;
   }

   /**
   Constructor
   */
   FlagData( const std::string & name, RegType start, RegType bits=1, RegType val=1, bool any=false ):
   _name(name),_startbit(start), _value(val), _anyvalue(any)
   {
	   _mask=1;
	   for (int i=0; i< (int)bits - 1; ++i)
	   {
		   _mask <<= 1;
		   _mask |=  1;
	   }
   }

   /**
   returns either flagname (_anyvalue=false and _value matches bitpattern), flagname=_value (_anyvalue=true) or empty string
   */
   const std::string & getFlagString( RegType regval ) const
   {
	   if ( _anyvalue )
	   {
		   std::stringstream tmpstr;
		   tmpstr << ((regval >> _startbit) & _mask);
		   _svalue=_name;
		   _svalue+="=";
		   _svalue+=tmpstr.str();
		   return _svalue;
	   }
	   else if( ((regval >> _startbit) & _mask) == _value  )
	   {
		   return _name;
	   }
	   return _snull;
   }
};

#endif