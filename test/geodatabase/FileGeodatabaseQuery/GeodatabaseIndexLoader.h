#pragma once

using namespace FileGDBAPI;
using namespace SpatialIndex;
using namespace std;

class GeodatabaseIndexLoader
{
public:
	GeodatabaseIndexLoader();
	~GeodatabaseIndexLoader();

	void loadIntoIndex(ISpatialIndex *index, const wstring &geodatabasePath);
};

