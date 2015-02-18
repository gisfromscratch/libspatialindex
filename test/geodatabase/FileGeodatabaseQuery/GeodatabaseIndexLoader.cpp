#include "stdafx.h"
#include "GeodatabaseIndexLoader.h"


GeodatabaseIndexLoader::GeodatabaseIndexLoader()
{
}


GeodatabaseIndexLoader::~GeodatabaseIndexLoader()
{
}


void GeodatabaseIndexLoader::loadIntoIndex(ISpatialIndex *index, const wstring &geodatabasePath)
{
	Geodatabase geodatabase;
	if (S_OK != OpenGeodatabase(geodatabasePath, geodatabase))
	{
		return;
	}

	CloseGeodatabase(geodatabase);
}
