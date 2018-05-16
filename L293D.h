/* 
 * L293D.h - Library for using a L293D motor controller chip
 * Created by Tyler Sweat, May 25 2015
 * Released under the MIT License
 */

#ifndef L293D_H
#define L293D_H

#include "Arduino.h"

class L293D
{
  public:
	extern "C" L293D(int, int, int);
  	L293D(int, int, int);
	  
	extern "C" void set(double);
  	void set(double);
	  
	extern "C" void set(int);
  	void set(int);
	  
	extern "C" int get();
  	int get();
  private:
  	int pin_E;
  	int pin_A;
  	int pin_B;
  	int val;
};

#endif
