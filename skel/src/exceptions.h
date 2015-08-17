/**********************************************************************
* Author:      Leo Liberti                                            *
* Name:        exceptions.h                                           *
* Source:      GNU C++                                                *
* Purpose:     Expression v3 exceptions                               *
* History:     010622 0.0 work started                                *
* License:     (C) Leo Liberti, all rights reserved. Code published under the 
               Common Public License.
***********************************************************************/

#ifndef __EV3EXCEPTIONSH__
#define __EV3EXCEPTIONSH__

#define RCS8 "$Id: exceptions.h,v 1.5 2003/10/08 11:13:58 liberti Exp liberti $"

#define HELPURL "http://liberti.dhs.org/"
#define NONE "[none]"
//#define STDACTION cerr << interface << "::" << scope << ": in [" << operation << "]: " << description << ", code = " << code << ", see " << moreinfo << endl
#define STDACTION ; 

namespace ev3 {

using namespace std;

class ErrBase {
 public:
  unsigned long code;
  string interface;
  string scope;
  string operation;
  string description;
  string moreinfo;
  ErrBase() :
    code(0), interface(NONE), scope(NONE), operation(NONE), 
    description(NONE),  moreinfo(HELPURL) { STDACTION; }
  ErrBase(unsigned long mycode,
	  string myif,
	  string myscope,
	  string myop,
	  string mydesc,
	  string myinfo) :
    code(mycode), interface(myif), scope(myscope), operation(myop), 
    description(mydesc), moreinfo(myinfo) { STDACTION; }
};

class ErrUnknown : public ErrBase { 
 public:
  ErrUnknown(unsigned long mycode,
	     string myif,
	     string myscope,
	     string myop,
	     string mydesc,
	     string myinfo) : 
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo) { STDACTION; }
};

class ErrNotPermitted : public ErrBase { 
 public:
  ErrNotPermitted(unsigned long mycode,
		  string myif,
		  string myscope,
		  string myop,
		  string mydesc,
		  string myinfo) :
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo) { STDACTION; }
};

class ErrDivideByZero : public ErrBase { 
 public:
  string dividend;
  ErrDivideByZero(unsigned long mycode,
		  string myif,
		  string myscope,
		  string myop,
		  string mydesc,
		  string myinfo, 
		  string mydiv) : 
    ErrBase(mycode, myif, myscope, myop, mydesc, myinfo),
    dividend(mydiv) { STDACTION; }
};

}

#endif
