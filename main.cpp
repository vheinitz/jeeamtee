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

#include "gmt/gmt.h"
#include <iostream>

//Configuration example (see func. body)
const char * getConfigData();

//Example for using gmt.
int main( ){

   //Testdata. In this case 4 x 32-bit registers.
   //Usually the data is read from file, a dump or accessed via mmap
   RegisterType DA_registers[4]={0};
   
   //Create gmt instance
   GMT gmt;
   
   //Read configuration from embedded data. Usually configuration is read from file.
   gmt.parseConfiguration( getConfigData() );

   //Setting memory pointer, where data should be interpreted according the configuration
	gmt.setMemoryPointer(DA_registers);

	std::string cmd, lastCmd; //usefull when pressing <enter> repeatd the command
	std::cout <<"Type 'h' for help or 'x' for exit\n";
	while(1) //gmm handles exit
	{			    
    		std::getline(std::cin, cmd);
    		
    		if ( cmd.empty() )
    		{
    			cmd = lastCmd; //repeat last command
    		}
         else if (cmd == "x")
         {
            break; //exit
         }
    		
    		gmt.processCommand( cmd );
    		
    		if ( !cmd.empty() )
    		{
    			lastCmd	= cmd;
    		}
	}
return 0;
}


const char * getConfigData()
{
 return
   "#Example: DA Converter\n"\
   "#Assume, there are 2 identical DA converters implemented in FPGA.\n"\
   "#The 32-Bit registers are mapped into user space memory. \n"\
   "#There are registers for output and configuration.\n"\
   "#Registers:\n"\
   "# AnOut<X> - Data output to this register will be converted to analogue signal\n"\
   "# AnOutConf<X> - Configuration register\n"\
   "#   Bit  0     - Enable if 1 disable otherwise\n"\
   "#   Bits 1-9   - Set voltage offset \n"\
   "#   Bits 10-19 - Output voltage MIN \n"\
   "#   Bits 20-29 - Output voltage MAX \n"\
   "#   Bits 30-31 - Output update mode, where modes are: \n"\
   "#     0        -   OnChange \n"\
   "#     1        -   1us \n"\
   "#     2        -   1ms \n"\
   "#     3        -   1s \n"\
   ""\
   "#Syntax of configuration file: \n"\
   "# Lines beginning with # are comments\n"\
   "# Register definition:\n"\
   "#    reg <REGISTER_NAME> <OFFSET> <MODE> [#Comment]\n"\
   "#    REGISTER_NAME is a string, OFFSET is hex-number and MODE is [rw]\n"\
   "# Flags definition\n"\
   "#    for_regs <REGISTER_NAME> [<REGISTER_NAME> ...] [#Comment]\n"\
   "#     flag <FLAG_NAME> <starting bit> <number of bits> <value> [#Comment]\n"\
   "#     data <FLAG_NAME> <starting bit> <number of bits> [#Comment]\n"\
   "#  FLAG_NAME is a string, other parameters are decimal numbers\n"\
   ""\
   "#Register definition for DA Converter 1 :\n"\
   "reg AnOut0       0   rw\n"\
   "reg AnOutConf0   1   rw\n"\
   "#Register definition for DA Converter 2 :\n"\
   "reg AnOut1       2   rw\n"\
   "reg AnOutConf1   3   rw\n"\
   "#Flags definition for config registers.\n"\
   "for_regs AnOutConf0 AnOutConf1\n"\
   "  flag ENABLE 0 1 1      #Bit 0 - enable if 1\n"\
   "  data VOL_OFFSET 1 9    #Bits 1-9   - voltage offset \n"\
   "  data VOL_MIN 10 9      #Bits 10-19   - Output voltage MIN \n"\
   "  data VOL_MAX 20 9      #Bits 20-29   - Output voltage MAX \n"\
   "  flag OM_CHG  30 2 0    #Bits 30-31   - 0 = output mode 'On change'\n"\
   "  flag OM_CHG  30 2 1    #Bits 30-31   - 1 = output mode 1us\n"\
   "  flag OM_CHG  30 2 2    #Bits 30-31   - 2 = output mode 1ms\n"\
   "  flag OM_CHG  30 2 3    #Bits 30-31   - 3 = output mode 1s\n"\
   "#Flags definition for data register. \n"\
   "for_regs AnOut0 AnOut1\n"\
   "  data OUT_VALUE 0 31    #32-bit DA data \n";
}
