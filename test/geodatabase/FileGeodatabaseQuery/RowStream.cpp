#include "stdafx.h"
#include "RowStream.h"


RowStream::RowStream(Table *table) :
	_table(table),
	_nextData(nullptr),
	_rowCount(0)
{
	cout << "RowStream: Intializing" << endl;
	rewind();
}


RowStream::~RowStream()
{
	if (nullptr != _nextData)
	{
		delete _nextData;
		_nextData = nullptr;
	}
}


IData* RowStream::getNext()
{
	auto current = _nextData;
	if (nullptr == current)
	{
		return nullptr;
	}

	readNextRow();
	return current;
}


bool RowStream::hasNext()
{
	if (nullptr == _nextData)
	{
		cout << "RowStream: " << _rowCount << " rows processed" << endl;
	}
	return nullptr != _nextData;
}


uint32_t RowStream::size()
{
	return 0;
}


void RowStream::rewind()
{
	if (nullptr != _nextData)
	{
		delete _nextData;
		_nextData = nullptr;
	}

	_rows.Close();
	if (S_OK != _table->Search(L"*", L"1=1", true, _rows))
	{
		return;
	}

	readNextRow();
}


void RowStream::readNextRow()
{
	_nextData = nullptr;

	Row nextRow;
	if (S_OK == _rows.Next(nextRow))
	{
		ShapeBuffer buffer;
		int32 oid;
		if (S_OK == nextRow.GetOID(oid) && S_OK == nextRow.GetGeometry(buffer))
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
								auto lowerLeft = _geometryFactory.createPoint(point->x - Eps, point->y - Eps);
								auto upperRight = _geometryFactory.createPoint(point->x + Eps, point->y + Eps);
								Region region(lowerLeft, upperRight);
								_nextData = new RTree::Data(0, 0, region, oid);
								_rowCount++;
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
								double xmin, ymin, xmax, ymax;
								for (auto pointIndex = 0; pointIndex < pointCount; pointIndex++)
								{
									auto point = points[pointIndex];
									if (0 == pointIndex)
									{
										xmin = point.x;
										ymin = point.y;
										xmax = point.x;
										ymax = point.y;
									}
									else
									{
										if (point.x < xmin)
										{
											xmin = point.x;
										}
										if (point.y < ymin)
										{
											ymin = point.y;
										}
										if (xmax < point.x)
										{
											xmax = point.x;
										}
										if (ymax < point.y)
										{
											ymax = point.y;
										}
									}
								}

								auto lowerLeft = _geometryFactory.createPoint(xmin - Eps, ymin - Eps);
								auto upperRight = _geometryFactory.createPoint(xmax + Eps, ymax + Eps);
								Region region(lowerLeft, upperRight);
								_nextData = new RTree::Data(0, 0, region, oid);
								_rowCount++;
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
}
