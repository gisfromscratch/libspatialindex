// FileGeodatabaseQuery.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "GeodatabaseIndexLoader.h"
#include "GeometryFactory.h"
#include "RowStream.h"
#include "SpatialVisitor.h"


using namespace FileGDBAPI;
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

static Geodatabase* applyOnTable(const wstring &geodatabasePath, function<void(Table*, wstring &tableName)> action)
{
	unique_ptr<Geodatabase> geodatabase(new Geodatabase);
	if (S_OK == OpenGeodatabase(geodatabasePath, *geodatabase))
	{
		vector<wstring> childDatasetPaths;
		if (S_OK == geodatabase->GetChildDatasets(L"\\", L"Feature Class", childDatasetPaths))
		{
			for_each(childDatasetPaths.begin(), childDatasetPaths.end(), [&](wstring &childDatasetPath) {
				unique_ptr<Table> table(new Table());
				if (S_OK == geodatabase->OpenTable(childDatasetPath, *table))
				{
					action(table.release(), childDatasetPath);
				}
			});
		}
		return geodatabase.release();
	}
	return nullptr;
}

static void showStatistics(SpatialVisitor &visitor)
{
	cout << "Spatial matches:" << endl;
	cout << "Nodes: " << visitor.visitedNodes() << " Data: " << visitor.visitedData() << endl << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (2 != argc)
	{
		wcerr << "  Usage: " << argv[0] << " <path_to_geodatabase_file>" << endl;
		system("PAUSE");
		return -1;
	}

	const wchar_t *pathToGeodatabase = argv[1];
	vector<Table*> tables;

	// Create a new RTree in memory
	auto storage = unique_ptr<IStorageManager>(StorageManager::createNewMemoryStorageManager());
	const double FillFactor = 0.7;
	const size_t IndexCapacity = 100;
	const size_t LeafCapacity = 100;
	const size_t Dimension = 2;
	{
		id_type indexId;
		// Open geodatabase
		Geodatabase *geodatabase = applyOnTable(pathToGeodatabase, [&](Table *table, wstring &tableName) {
			tables.push_back(table);

			RowStream rowStream(table);
			auto index = unique_ptr<ISpatialIndex>(RTree::createAndBulkLoadNewRTree(RTree::BulkLoadMethod::BLM_STR, rowStream, *storage, FillFactor, IndexCapacity, LeafCapacity, Dimension, RTree::RTreeVariant::RV_RSTAR, indexId));
			wcout << "Statistic for " << tableName << endl;
			cout << *index.get() << endl;
		});

		if (0 == tables.size())
		{
			wcerr << "  Geodatabase at " << pathToGeodatabase << " has no feature classes!" << endl;
		}
		else
		{
			for_each(tables.begin(), tables.end(), [&](Table *table) {
				if (nullptr != geodatabase)
				{
					if (nullptr != table)
					{
						geodatabase->CloseTable(*table);
						delete table;
						table = nullptr;
					}
				}
			});
			tables.clear();
		}

		if (nullptr != geodatabase)
		{
			CloseGeodatabase(*geodatabase);
			delete geodatabase;
			geodatabase = nullptr;
		}
	}

	system("PAUSE");
	return 0;
}

