#ifndef DEBUG_ICE
#define DEBUG_ICE

#include "common.ice"

module bica {
 
 	enum ColorType { BLUE, GREEN, RED, ORANGE, PURPLE, BROWN, GREY, YELLOW, BLACK, WHITE };
 
 	/**
   	*  Description of a general shape
   	*/
	class Shape {
		ColorType 	color;	//	= BLACK;	// Color of the shape
		bool   		filled; // 	= false;	// Will be filled inside or not
		string		accKey; //	= "";		// Accelerated key associated
		short		opacity;//  = 255;		// Opacity (0 = transparent -> 255 = opaque)
		string		label;//  = "";
	};
	
	/**
   	*  Description of a 3D point
   	*/
	class Point3D {
		float x;
		float y;
		float z;	// = 0.0f;
	};
	
	/**
   	*  Description of a 3D line
   	*/
	class Line extends Shape {
		Point3D p1;
		Point3D p2;
		short	width; // = 1;
	};
	
	/**
   	*  Description of a 3D arrow
   	*/
	class Arrow extends Shape {
		Point3D src;
		Point3D dst;
		short	width; // = 1;
	};
	
	/**
   	*  Description of a circle parallel to the ground plane
   	*/
	class Circle extends Shape {
		Point3D center;
		float	radius;
		float 	z; 		// = 0.0f;
	};
	
	/**
   	*  Description of an ellipse parallel to the ground plane
   	*/
	class Ellipse extends Shape {
		Point3D center;
		float	width;
		float	length;
		float	angle;
		float 	z;		// = 0.0f;
	};
	
	/**
   	*  Description of a rectangle parallel to the ground plane
   	*/
	class Rectangle extends Shape {
		Point3D p1;
		Point3D p2;
	};
	
	/**
   	*  Description of a cube parallel to the ground plane
   	*/
   	class Cube extends Rectangle {
   		float height;
   	};
 
 	sequence<Shape> ShapeList;
 
	/** 
	* Interface to the debug component
	*/
	interface GrDebugProvider
	{
		ShapeList getGrDebugRel();
		ShapeList getGrDebugAbs();
		ShapeList getGrDebugImg();
		void debugOn ( string component );
		void debugOff ( string component );
	};
    
};

#endif //DEBUG_ICE
