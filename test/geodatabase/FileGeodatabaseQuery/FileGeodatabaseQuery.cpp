// FileGeodatabaseQuery.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "GeodatabaseIndexLoader.h"
#include "GeometryFactory.h"
#include "SpatialVisitor.h"


using namespace SpatialIndex;
using namespace std;


static void addPointsToIndex(GeometryFactory *factory, ISpatialIndex *index)
{
	auto lowerLeft = factory->createPoint(0, 0);
	auto upperRight = factory->createPoint(1, 1);
	auto region = Region(lowerLeft, upperRight);

	const id_type id = 0;
	index->insertData(0, 0, region, id);
}

static void showStatistics(SpatialVisitor &visitor)
{
	cout << "Spatial matches:" << endl;
	cout << "Nodes: " << visitor.visitedNodes() << " Data: " << visitor.visitedData() << endl << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Create a new RTree in memory
	auto storage = unique_ptr<IStorageManager>(StorageManager::createNewMemoryStorageManager());
	const double FillFactor = 0.7;
	const size_t IndexCapacity = 10;
	const size_t LeafCapacity = 10;
	const size_t Dimension = 2;
	{
		id_type indexId;
		auto spatialIndex = unique_ptr<ISpatialIndex>(RTree::createNewRTree(*storage, FillFactor, IndexCapacity, LeafCapacity, Dimension, RTree::RTreeVariant::RV_RSTAR, indexId));

		auto geometryFactory = unique_ptr<GeometryFactory>(new GeometryFactory);
		addPointsToIndex(geometryFactory.get(), spatialIndex.get());

		// Query region
		SpatialVisitor visitor;
		auto origin = geometryFactory->createPoint(0, 0);
		spatialIndex->intersectsWithQuery(origin, visitor);
		showStatistics(visitor);

		visitor.resetStatistics();
		spatialIndex->intersectsWithQuery(geometryFactory->createPoint(-1, -1), visitor);
		showStatistics(visitor);

		visitor.resetStatistics();
		spatialIndex->intersectsWithQuery(geometryFactory->createPoint(1, 1), visitor);
		showStatistics(visitor);

		// Open geodatabase
		GeodatabaseIndexLoader loader;
		loader.loadIntoIndex(spatialIndex.get(), L"..\\..\\testdata\\Querying.gdb");

		cout << *spatialIndex.get();
	}

	system("PAUSE");
	return 0;
}

