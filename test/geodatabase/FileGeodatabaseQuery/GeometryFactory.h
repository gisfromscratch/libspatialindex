#pragma once

#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;

class GeometryFactory
{
public:
	GeometryFactory();
	~GeometryFactory();

	Point createPoint(double x, double y);

private:
	const static size_t TwoDimensions = 2;

	double _twoDimensionalCoordinates[TwoDimensions];
};

