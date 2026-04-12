#pragma once

#include <vector>
#include <Any/Any.h>
#include "Util/MacroUtil.h"

namespace BLOCKMAN
{
	class VarList
	{
	public:
		VarList() = default;

		template <class T>
		VarList& operator<<(const T& data)
		{
			m_vecData.emplace_back(data);
			return *this;
		}

        VarList& operator<<(const VarList& data)
        {
            m_vecData.insert(m_vecData.end(), data.m_vecData.begin(), data.m_vecData.end());
            return *this;
        }

		VarList& operator=(VarList& other)
		{
			m_vecData.swap(other.m_vecData);
			return *this;
		}

		template <class T>
		T get(size_t index)
		{
			if (index < 0 || index >= m_vecData.size())
			{
				assert(0);
				throw std::out_of_range("VarList get out_of_range");
			}
			return any_cast<T>(m_vecData[index]);
		}

		size_t size() {
			return m_vecData.size();
		};

		void clear()
		{
			m_vecData.clear();
		}

	private:
		std::vector<Any> m_vecData;
	};
}