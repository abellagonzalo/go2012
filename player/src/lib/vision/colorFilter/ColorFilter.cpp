/*
 * Name: ColorFilter.cpp
 * @Author: Carlos Agüero (caguero@gsyc.es)
 *
 * Description: Abstract class that specify a generic interface for any HSV color filter. The class needs
 *  the H, S and V thresholds and also the parameters of the image to filter. The method
 * getFiltered() returns a pointer to the last filtered image. It could be useful for debugging purpose.
 *
 * Copyright (C) Universidad Rey Juan Carlos
 * All Rights Reserved.
 */

#include "ColorFilter.h"

const string ColorFilter::COLOR_FILTER_PARAMS_FILE_UPPER =
		"/home/nao/bica/conf/ballPerception/colorHSVupper.conf";
const string ColorFilter::COLOR_FILTER_PARAMS_FILE_LOWER =
		"/home/nao/bica/conf/ballPerception/colorHSVlower.conf";

ColorFilter::ColorFilter(): ImageInput()
{
	this->colorFileNameUpper = COLOR_FILTER_PARAMS_FILE_UPPER;
	this->colorFileNameLower = COLOR_FILTER_PARAMS_FILE_LOWER;

	//Create the dictionaries
	colorConfigUpper.createDictionary();
	colorConfigLower.createDictionary();

	// Load the filter parameters
	loadFilterParams(ImageInput::UPPER_CAMERA, colorFileNameUpper.c_str());
	loadFilterParams(ImageInput::LOWER_CAMERA, colorFileNameLower.c_str());

	//Create a YUV->HSV look up table (64 entries by channel)
	lutYUVupper = new Lut(orangeUpper, blueUpper, yellowUpper, greenUpper, whiteUpper, cyanUpper, magentaUpper);
	lutYUVlower = new Lut(orangeLower, blueLower, yellowLower, greenLower, whiteLower, cyanLower, magentaLower);
}


/**
 * Class destructor.
 **/
ColorFilter::~ColorFilter()
{
	delete lutYUVupper;
	delete lutYUVlower;
	colorConfigUpper.freeDictionary();
	colorConfigLower.freeDictionary();
}


/**
 * getFilterParams. Method that allows to read the current thresholds of the filter.
 * @param hmin Gets the pointer to the lower threshold of the H filter's component.
 * @param hmax Gets the pointer to the upper threshold of the H filter's component.
 * @param smin Gets the pointer to the lower threshold of the S filter's component.
 * @param smax Gets the pointer to the upper threshold of the S filter's component.
 * @param vmin Gets the pointer to the lower threshold of the V filter's component.
 * @param vmax Gets the pointer to the upper threshold of the V filter's component.
 **/
void
ColorFilter::getFilterParams(const int cam, HSVClrParams& orange, HSVClrParams& blue, HSVClrParams& yellow,
		HSVClrParams& green, HSVClrParams& white, HSVClrParams& cyan, HSVClrParams& magenta)
{
	if (cam == ImageInput::UPPER_CAMERA){
		orange 		= orangeUpper;
		blue		= blueUpper;
		yellow		= yellowUpper;
		green		= greenUpper;
		white		= whiteUpper;
		cyan		= cyanUpper;
		magenta		= magentaUpper;
	}else{
		orange 		= orangeLower;
		blue		= blueLower;
		yellow		= yellowLower;
		green		= greenLower;
		white		= whiteLower;
		cyan		= cyanLower;
		magenta		= magentaLower;
	}
}


std::string float_to_str(float f)
{
	std::stringstream stream;
	stream << f;
	return stream.str();
}


/**
 * setFilterParams. Method that updates the current thresholds of the filter.
 * @param hmin Sets the new lower threshold of the H filter's component.
 * @param hmax Sets the new upper threshold of the H filter's component.
 * @param smin Sets the new lower threshold of the S filter's component.
 * @param smax Sets the new upper threshold of the S filter's component.
 * @param vmin Sets the new lower threshold of the V filter's component.
 * @param vmax Sets the new upper threshold of the V filter's component.
 **/
void
ColorFilter::setFilterParams(const int cam, const HSVClrParams orange, const HSVClrParams blue, const HSVClrParams yellow,
		const HSVClrParams green, const HSVClrParams white, const HSVClrParams cyan, const HSVClrParams magenta)
{
	Dictionary 	 *colorConfig;
	string		 colorFileName;

	if (cam == ImageInput::UPPER_CAMERA){
		colorConfig = &colorConfigUpper;
		colorFileName = colorFileNameUpper;
		orangeUpper 	= orange;
		blueUpper		= blue;
		yellowUpper		= yellow;
		greenUpper		= green;
		whiteUpper		= white;
		cyanUpper		= cyan;
		magentaUpper	= magenta;
	}else{
		colorConfig = &colorConfigLower;
		colorFileName = colorFileNameLower;
		orangeLower 	= orange;
		blueLower		= blue;
		yellowLower		= yellow;
		greenLower		= green;
		whiteLower		= white;
		cyanLower		= cyan;
		magentaLower	= magenta;
	}

	colorConfig->setValue("ORANGE_HMIN", float_to_str(orange.hmin).c_str());
	colorConfig->setValue("ORANGE_HMAX", float_to_str(orange.hmax).c_str());
	colorConfig->setValue("ORANGE_SMIN", float_to_str(orange.smin).c_str());
	colorConfig->setValue("ORANGE_SMAX", float_to_str(orange.smax).c_str());
	colorConfig->setValue("ORANGE_VMIN", float_to_str(orange.vmin).c_str());
	colorConfig->setValue("ORANGE_VMAX", float_to_str(orange.vmax).c_str());

	colorConfig->setValue("BLUE_HMIN", float_to_str(blue.hmin).c_str());
	colorConfig->setValue("BLUE_HMAX", float_to_str(blue.hmax).c_str());
	colorConfig->setValue("BLUE_SMIN", float_to_str(blue.smin).c_str());
	colorConfig->setValue("BLUE_SMAX", float_to_str(blue.smax).c_str());
	colorConfig->setValue("BLUE_VMIN", float_to_str(blue.vmin).c_str());
	colorConfig->setValue("BLUE_VMAX", float_to_str(blue.vmax).c_str());

	colorConfig->setValue("YELLOW_HMIN", float_to_str(yellow.hmin).c_str());
	colorConfig->setValue("YELLOW_HMAX", float_to_str(yellow.hmax).c_str());
	colorConfig->setValue("YELLOW_SMIN", float_to_str(yellow.smin).c_str());
	colorConfig->setValue("YELLOW_SMAX", float_to_str(yellow.smax).c_str());
	colorConfig->setValue("YELLOW_VMIN", float_to_str(yellow.vmin).c_str());
	colorConfig->setValue("YELLOW_VMAX", float_to_str(yellow.vmax).c_str());

	colorConfig->setValue("GREEN_HMIN", float_to_str(green.hmin).c_str());
	colorConfig->setValue("GREEN_HMAX", float_to_str(green.hmax).c_str());
	colorConfig->setValue("GREEN_SMIN", float_to_str(green.smin).c_str());
	colorConfig->setValue("GREEN_SMAX", float_to_str(green.smax).c_str());
	colorConfig->setValue("GREEN_VMIN", float_to_str(green.vmin).c_str());
	colorConfig->setValue("GREEN_VMAX", float_to_str(green.vmax).c_str());

	colorConfig->setValue("WHITE_HMIN", float_to_str(white.hmin).c_str());
	colorConfig->setValue("WHITE_HMAX", float_to_str(white.hmax).c_str());
	colorConfig->setValue("WHITE_SMIN", float_to_str(white.smin).c_str());
	colorConfig->setValue("WHITE_SMAX", float_to_str(white.smax).c_str());
	colorConfig->setValue("WHITE_VMIN", float_to_str(white.vmin).c_str());
	colorConfig->setValue("WHITE_VMAX", float_to_str(white.vmax).c_str());

	colorConfig->setValue("CYAN_HMIN", float_to_str(cyan.hmin).c_str());
	colorConfig->setValue("CYAN_HMAX", float_to_str(cyan.hmax).c_str());
	colorConfig->setValue("CYAN_SMIN", float_to_str(cyan.smin).c_str());
	colorConfig->setValue("CYAN_SMAX", float_to_str(cyan.smax).c_str());
	colorConfig->setValue("CYAN_VMIN", float_to_str(cyan.vmin).c_str());
	colorConfig->setValue("CYAN_VMAX", float_to_str(cyan.vmax).c_str());

	colorConfig->setValue("MAGENTA_HMIN", float_to_str(magenta.hmin).c_str());
	colorConfig->setValue("MAGENTA_HMAX", float_to_str(magenta.hmax).c_str());
	colorConfig->setValue("MAGENTA_SMIN", float_to_str(magenta.smin).c_str());
	colorConfig->setValue("MAGENTA_SMAX", float_to_str(magenta.smax).c_str());
	colorConfig->setValue("MAGENTA_VMIN", float_to_str(magenta.vmin).c_str());
	colorConfig->setValue("MAGENTA_VMAX", float_to_str(magenta.vmax).c_str());

	colorConfig->saveToFile(colorFileName.c_str());

	if (cam == ImageInput::UPPER_CAMERA){
		cout << "[ColorFilter] New color filter loaded for upper camera" << endl;
	}else{
		cout << "[ColorFilter] New color filter loaded for lower camera" << endl;
	}
}

void
ColorFilter::loadFilterParams(const int cam, const char *file)
{
	Dictionary 	 *colorConfig;
	HSVClrParams *orange, *blue, *yellow, *green, *white, *cyan, *magenta;

	if (cam == ImageInput::UPPER_CAMERA){
		colorConfig = &colorConfigUpper;
		orange 		= &orangeUpper;
		blue		= &blueUpper;
		yellow		= &yellowUpper;
		green		= &greenUpper;
		white		= &whiteUpper;
		cyan		= &cyanUpper;
		magenta		= &magentaUpper;
	}else{
		colorConfig = &colorConfigLower;
		orange 		= &orangeLower;
		blue		= &blueLower;
		yellow		= &yellowLower;
		green		= &greenLower;
		white		= &whiteLower;
		cyan		= &cyanLower;
		magenta		= &magentaLower;
	}

	colorConfig->loadFromFile(file);

	if (!((colorConfig->getValueFloat("ORANGE_HMIN", &(orange->hmin))) && (colorConfig->getValueFloat("ORANGE_HMAX", &(orange->hmax))) &&
			(colorConfig->getValueFloat("ORANGE_SMIN", &(orange->smin))) && (colorConfig->getValueFloat("ORANGE_SMAX", &(orange->smax))) &&
			(colorConfig->getValueFloat("ORANGE_VMIN", &(orange->vmin))) && (colorConfig->getValueFloat("ORANGE_VMAX", &(orange->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading orange color filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("BLUE_HMIN", &(blue->hmin))) && (colorConfig->getValueFloat("BLUE_HMAX", &(blue->hmax))) &&
			(colorConfig->getValueFloat("BLUE_SMIN", &(blue->smin))) && (colorConfig->getValueFloat("BLUE_SMAX", &(blue->smax))) &&
			(colorConfig->getValueFloat("BLUE_VMIN", &(blue->vmin))) && (colorConfig->getValueFloat("BLUE_VMAX", &(blue->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading blue color filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("YELLOW_HMIN", &(yellow->hmin))) && (colorConfig->getValueFloat("YELLOW_HMAX", &(yellow->hmax))) &&
			(colorConfig->getValueFloat("YELLOW_SMIN", &(yellow->smin))) && (colorConfig->getValueFloat("YELLOW_SMAX", &(yellow->smax))) &&
			(colorConfig->getValueFloat("YELLOW_VMIN", &(yellow->vmin))) && (colorConfig->getValueFloat("YELLOW_VMAX", &(yellow->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading yellow color filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("GREEN_HMIN", &(green->hmin))) && (colorConfig->getValueFloat("GREEN_HMAX", &(green->hmax))) &&
			(colorConfig->getValueFloat("GREEN_SMIN", &(green->smin))) && (colorConfig->getValueFloat("GREEN_SMAX", &(green->smax))) &&
			(colorConfig->getValueFloat("GREEN_VMIN", &(green->vmin))) && (colorConfig->getValueFloat("GREEN_VMAX", &(green->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading green color filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("WHITE_HMIN", &(white->hmin))) && (colorConfig->getValueFloat("WHITE_HMAX", &(white->hmax))) &&
			(colorConfig->getValueFloat("WHITE_SMIN", &(white->smin))) && (colorConfig->getValueFloat("WHITE_SMAX", &(white->smax))) &&
			(colorConfig->getValueFloat("WHITE_VMIN", &(white->vmin))) && (colorConfig->getValueFloat("WHITE_VMAX", &(white->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading white color filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("CYAN_HMIN", &(cyan->hmin))) && (colorConfig->getValueFloat("CYAN_HMAX", &(cyan->hmax))) &&
			(colorConfig->getValueFloat("CYAN_SMIN", &(cyan->smin))) && (colorConfig->getValueFloat("CYAN_SMAX", &(cyan->smax))) &&
			(colorConfig->getValueFloat("CYAN_VMIN", &(cyan->vmin))) && (colorConfig->getValueFloat("CYAN_VMAX", &(cyan->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading  color cyan filter from " << file << end_color << endl;

	if (!((colorConfig->getValueFloat("MAGENTA_HMIN", &(magenta->hmin))) && (colorConfig->getValueFloat("MAGENTA_HMAX", &(magenta->hmax))) &&
			(colorConfig->getValueFloat("MAGENTA_SMIN", &(magenta->smin))) && (colorConfig->getValueFloat("MAGENTA_SMAX", &(magenta->smax))) &&
			(colorConfig->getValueFloat("MAGENTA_VMIN", &(magenta->vmin))) && (colorConfig->getValueFloat("MAGENTA_VMAX", &(magenta->vmax)))))
		cerr << begin_red << "[ColorFilter] Problem loading magenta color filter from " << file << end_color << endl;
}

void
ColorFilter::reloadLut(const int cam, const HSVClrParams orange, const HSVClrParams blue, const HSVClrParams yellow,
		const HSVClrParams green, const HSVClrParams white, const HSVClrParams cyan, const HSVClrParams magenta)
{
	if (cam == ImageInput::UPPER_CAMERA){
		lutYUVupper->create(orange, blue, yellow, green, white, cyan, magenta);
	}else{
		lutYUVlower->create(orange, blue, yellow, green, white, cyan, magenta);
	}
}

