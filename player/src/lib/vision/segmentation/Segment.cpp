/*
 * Name: Segment.h
 * @Author: Eduardo Perdices (eperdices@gsyc.es)
 *
 * Description: Class to collect up several pixels aligned with the same color.
 *
 * Copyright (C) 2009-2010 Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "Segment.h"

Segment::Segment()
{
	this->next = NULL;
	this->previous = NULL;
	this->region = NULL;
	this->length = 0;
	this->anglePrevious = 0;
}

Segment::~Segment()
{
}
