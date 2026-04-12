#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "Core.h"

namespace LORD
{
	class CsvReader
	{
	public:
		typedef std::map<std::string, int> ColumnIndexMap;

		CsvReader();
		virtual ~CsvReader();
		//¼ÓÔØCsvÎÄ¼þ
		bool LoadFile(const std::string & fileName);
		//¶ÁÈ¡Ò»ÐÐÊý¾Ý
		bool Read();
		void Close();
		//µÃµ½ÁÐÃû×Ö¶ÎµÄË÷Òý
		int GetColumnIndex(const std::string & columnName);
		int GetHeight();

		//��������ȡ����
		bool GetBool(const std::string& columnName, bool& ret, bool defaultValue = false);
		bool GetInt(const std::string& columnName, int& ret, int defaultValue = 0);
		bool GetFloat(const std::string& columnName, float& ret, float defaultValue = 0.f);
		bool GetString(const std::string& columnName, std::string& ret);
		bool GetString(const std::string& columnName, String& ret);

		//·Ö¸î×Ö·û´®Îª×Ö·ûÊý×é
		static void SplitString(const std::string & str, std::vector<std::string> & str_vec, char delimiter);

	private:
		ColumnIndexMap _columnIndex;    //×Ö¶ÎËù¶ÔÓ¦ÁÐµÄË÷Òý
		std::vector<std::string> _lineData;//CsvÎÄ¼þÒ»ÐÐÊý¾Ý
		std::ifstream _fi;
		bool _fileIsOpen;
		std::string _line;
		std::stringstream _ss;
	};
}
