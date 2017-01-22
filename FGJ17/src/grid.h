#ifndef GRID_H
#define GRID_H

#include <cstdint>
#include <cassert>
#include <map>
#include <vector>
#include "vec2.h"
#include "macros.h"

template<typename T>
class Grid
{
	typedef std::pair<int32_t, int32_t> GridKey;
	typedef std::pair<vec2, T> GridValue;

public:
	Grid(const int32_t cellDivisor = 32) :
		m_cellDivisor(cellDivisor),
		m_data()
	{

	}

	~Grid()
	{

	}

	void insertData(const vec2 & pos, T data)
	{
		const vec2 valPos = pos.round();
		const int32_t ix = static_cast<int32_t>(std::round(valPos.x / m_cellDivisor));
		const int32_t iy = static_cast<int32_t>(std::round(valPos.y / m_cellDivisor));
		const GridKey idx(ix, iy);
		const GridValue val(valPos, data);

		m_data[idx].push_back(val);
	}

	void deleteData(const vec2 & pos)
	{
		const vec2 valPos = pos.round();
		const int32_t ix = static_cast<int32_t>(std::round(valPos.x / m_cellDivisor));
		const int32_t iy = static_cast<int32_t>(std::round(valPos.y / m_cellDivisor));
		const GridKey idx(ix, iy);

		if (m_data.find(idx) != m_data.end())
		{
			for (size_t i = 0; i < m_data[idx].size(); i++)
			{
				if (m_data[idx][i].first == valPos)
					m_data[idx].erase(m_data[idx].begin() + i);
			}
		}
	}

	bool getData(const vec2 & pos, std::vector<T> & data)
	{
		const vec2 valPos = pos.round();
		const int32_t ix = static_cast<int32_t>(std::round(valPos.x / m_cellDivisor));
		const int32_t iy = static_cast<int32_t>(std::round(valPos.y / m_cellDivisor));
		const GridKey idx(ix, iy);
		bool data_found = false;

		if (m_data.find(idx) != m_data.end())
		{
			for (size_t i = 0; i < m_data[idx].size(); i++)
			{
				if (m_data[idx][i].first == valPos)
				{
					data_found = true;

					data.push_back(m_data[idx][i].second);
				}
			}
		}

		return data_found;
	}

	bool getNearestData(const vec2 & pos, const int32_t range, std::vector<T> & data)
	{
		const vec2 valPos = pos.round();
		const int32_t ix = static_cast<int32_t>(std::round(valPos.x / m_cellDivisor));
		const int32_t iy = static_cast<int32_t>(std::round(valPos.y / m_cellDivisor));
		bool data_found = false;
		std::vector<GridValue> nearestData;

		if (range <= 0)
		{
			const GridKey idx(ix, iy);

			if (m_data.find(idx) != m_data.end())
			{
				data_found = true;

				nearestData = m_data[idx];
			}
		}
		else
		{
			for (int32_t y = -range; y <= range; y++)
			{
				for (int32_t x = -range; x <= range; x++)
				{
					const int32_t iix = ix + x;
					const int32_t iiy = iy + y;
					const GridKey idx(iix, iiy);

					if (m_data.find(idx) != m_data.end())
					{
						data_found = true;

						nearestData.insert(nearestData.end(), m_data[idx].begin(), m_data[idx].end());
					}
				}
			}
		}

		if (data_found)
		{
			data.resize(nearestData.size());
			for (size_t i = 0; i < data.size(); i++)
			{
				data[i] = nearestData[i].second;
			}
		}

		return data_found;
	}

private:
	const int32_t m_cellDivisor;
	std::map<GridKey, std::vector<GridValue>> m_data;
};

#endif // GRID_H
