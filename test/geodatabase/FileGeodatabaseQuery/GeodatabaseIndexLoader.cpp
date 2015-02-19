#include "stdafx.h"
#include "GeodatabaseIndexLoader.h"

#include <codecvt>
#include <locale>
#include <vector>


GeodatabaseIndexLoader::GeodatabaseIndexLoader(size_t indexCapacity, size_t leafCapacity) :
	_indexCapacity(indexCapacity),
	_leafCapacity(leafCapacity)
{
}


GeodatabaseIndexLoader::~GeodatabaseIndexLoader()
{
}


ISpatialIndex* GeodatabaseIndexLoader::loadIntoIndex(IStorageManager *storageManager, const wstring &geodatabasePath)
{
	const double FillFactor = 0.7;
	const size_t Dimension = 2;
	id_type indexId;
	auto index = unique_ptr<ISpatialIndex>(RTree::createNewRTree(*storageManager, FillFactor, _indexCapacity, _leafCapacity, Dimension, RTree::RTreeVariant::RV_RSTAR, indexId));

	Geodatabase geodatabase;
	if (S_OK != OpenGeodatabase(geodatabasePath, geodatabase))
	{
		return index.release();
	}
	{
		// Geodatabase instance seems to manage those paths lifetime!
		vector<wstring> childDatasetPaths;
		if (S_OK == geodatabase.GetChildDatasets(L"\\", L"Feature Class", childDatasetPaths))
		{
			for_each(childDatasetPaths.begin(), childDatasetPaths.end(), [&](wstring &childDatasetPath) {
				Table table;
				if (S_OK == geodatabase.OpenTable(childDatasetPath, table))
				{
					FileGDBAPI::Envelope extent;
					int rowCount;
					if (S_OK == table.GetRowCount(rowCount) && S_OK == table.GetExtent(extent))
					{
						TableStatistic statistic;
						statistic.Extent = extent;
						statistic.RowCount = rowCount;
						statistic.TableName = childDatasetPath;
						_statistics.push_back(statistic);

						EnumRows rows;
						if (S_OK == table.Search(L"*", L"1=1", true, rows))
						{
							Row row;
							ShapeBuffer buffer;
							while (S_OK == rows.Next(row))
							{
								int32 oid;
								if (S_OK == row.GetOID(oid) && S_OK == row.GetGeometry(buffer))
								{
									if (!buffer.IsEmpty())
									{
										GeometryType geometryType;
										if (S_OK == buffer.GetGeometryType(geometryType))
										{
											switch (geometryType)
											{
											case GeometryType::geometryPoint:
												{
													FileGDBAPI::PointShapeBuffer *pointBuffer = reinterpret_cast<FileGDBAPI::PointShapeBuffer*>(&buffer);
													FileGDBAPI::Point *point;
													if (S_OK == pointBuffer->GetPoint(point))
													{
														auto location = _geometryFactory.createPoint(point->x, point->y);
														index->insertData(0, 0, location, oid);
													}
												}
												break;

											case GeometryType::geometryPolyline:
											case GeometryType::geometryPolygon:
												{
													FileGDBAPI::MultiPartShapeBuffer *multiBuffer = reinterpret_cast<FileGDBAPI::MultiPartShapeBuffer*>(&buffer);
													FileGDBAPI::Point *points;
													int pointCount;
													if (S_OK == multiBuffer->GetNumPoints(pointCount) && S_OK == multiBuffer->GetPoints(points))
													{													
														for (auto pointIndex = 0; pointIndex < pointCount; pointIndex++)
														{
															auto location = _geometryFactory.createPoint(points[pointIndex].x, points[pointIndex].y);
															index->insertData(0, 0, location, oid);
														}
													}
												}
												break;

											default:
												break;
											}
										}
									}
								}
							}
							rows.Close();
						}
					}

					geodatabase.CloseTable(table);
				}
			});
		}
	}
	CloseGeodatabase(geodatabase);
	return index.release();
}


ostream& operator<<(ostream& os, const GeodatabaseIndexLoader& instance)
{
	os << "Statistics: [" << endl;
	for_each(instance._statistics.begin(), instance._statistics.end(), [&] (const TableStatistic &statistic) {
		auto extent = statistic.Extent;
		os << "            \tName:\t\t" << wstring_convert<codecvt_utf8<wchar_t>, wchar_t>().to_bytes(statistic.TableName) << endl;
		os << "            \tExtent: \t{ xmin:" << extent.xMin << ", xmax:" << extent.xMax << ", ymin:" << extent.yMin << ", ymax:" << extent.yMax << " }" << endl;
		os << "            \tRowCount:\t" << statistic.RowCount << endl;
	});
	os << "            ]" << endl;
	return os;
}
