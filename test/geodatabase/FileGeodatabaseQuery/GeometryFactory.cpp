#include "stdafx.h"
#include "GeometryFactory.h"


GeometryFactory::GeometryFactory()
{
}


GeometryFactory::~GeometryFactory()
{
}


Point GeometryFactory::createPoint(double x, double y)
{
	_twoDimensionalCoordinates[0] = x;
	_twoDimensionalCoordinates[1] = y;
	return Point(_twoDimensionalCoordinates, TwoDimensions);
}
