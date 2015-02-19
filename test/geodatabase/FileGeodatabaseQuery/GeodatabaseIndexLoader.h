#pragma once

#include "GeometryFactory.h"

using namespace FileGDBAPI;
using namespace SpatialIndex;
using namespace std;

class GeodatabaseIndexLoader
{
public:
	GeodatabaseIndexLoader(size_t indexCapacity = 100, size_t leafCapacity = 100);
	~GeodatabaseIndexLoader();

	ISpatialIndex* loadIntoIndex(IStorageManager *storageManager, const wstring &geodatabasePath);

private:
	GeometryFactory _geometryFactory;

	size_t _indexCapacity;
	size_t _leafCapacity;
};

