#pragma once

#include "GeometryFactory.h"

using namespace FileGDBAPI;
using namespace SpatialIndex;
using namespace std;

class GeodatabaseIndexLoader
{
public:
	GeodatabaseIndexLoader();
	~GeodatabaseIndexLoader();

	ISpatialIndex* loadIntoIndex(IStorageManager *storageManager, const wstring &geodatabasePath);

private:
	GeometryFactory _geometryFactory;
};

