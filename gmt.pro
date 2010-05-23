#######################################################################
## Project File for test project: DA-Converter.
## Valentin Heinitz, 2010.05.22, http://heinitz-it.de
## Build it on Linux:
##   qmake && make
## Build it on Windows:
##   1. get free-version of Qt at http://qt.nokia.com/
##     It is absolutely unnecessary for this project, but you will love it!
##   2. Build using Visual Studio Project.

#######################################################################

#The MIT License

#Copyright (c) 2010 Valentin Heinitz, http://heinitz-it.de

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.


CONFIG += console
TEMPLATE = app
TARGET = gmt
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += gmt/gmt.h gmt/flagdata.h
SOURCES += gmt/gmt.cpp main.cpp 
