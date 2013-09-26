/*
 * Name: Lut.h
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description:
 *
 * Copyright (C) 2008-2009 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#ifndef _LUT_H_
#define _LUT_H_

#include <climits>
#include <cmath>
#include "VisionDefinitions.h"
#include "colorFilter/ColorModelConversions.h"
#include "../Common.h"

using namespace std;

class Lut
{
public:
	Lut(HSVClrParams orange, HSVClrParams blue, HSVClrParams yellow, HSVClrParams green, HSVClrParams white, HSVClrParams cyan, HSVClrParams magenta);
	~Lut();

	void create(HSVClrParams orange, HSVClrParams blue, HSVClrParams yellow, HSVClrParams green, HSVClrParams white, HSVClrParams cyan, HSVClrParams magenta);

	inline unsigned char getValue (unsigned char y, unsigned char u, unsigned char v)
	{
		return table[y >> BITS][u >> BITS][v >> BITS];
	};

	static void yuv2rgb(unsigned char y, unsigned char u, unsigned char v,
			unsigned char &r, unsigned char &g, unsigned char &b);
	static void rgb2hsv(unsigned char r, unsigned char g, unsigned char b,
			float &H, float &S, float &V);

private:

	static const int ENTRIES_CH 	= 64;	// Entries per channel
	static const int UCHAR_RANGE 	= UCHAR_MAX + 1;
	static const int STEP 			= UCHAR_RANGE / ENTRIES_CH;

	int BITS;
	unsigned char table[ENTRIES_CH][ENTRIES_CH][ENTRIES_CH];
};

#endif //_LUT_H_
