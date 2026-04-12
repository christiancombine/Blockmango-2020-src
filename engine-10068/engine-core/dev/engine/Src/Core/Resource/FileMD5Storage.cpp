#include "Core.h"
#include "FileMD5Storage.h"
#include <Object/Root.h>
#include <Util/PathUtil.h>
#include "DataStream.h"
#define MD5_SEPERATOR "---"

namespace LORD
{

	bool FileMD5Storage::hasFileKey(const String& file_key) const
	{
		return (file_to_data_.find(file_key) != file_to_data_.end());
	}

	bool FileMD5Storage::hasCompressedMD5(const String& md5) const
	{
		return (cmp_md5_to_file_.find(md5) != cmp_md5_to_file_.end());
	}

	LORD::String FileMD5Storage::getDownLoadMD5(const String& file_key) const
	{
		if (!hasFileKey(file_key)) { return ""; }

		const FileMD5Data& data = file_to_data_.at(file_key);
		if (!data.cmp_md5.empty())
		{
			return data.cmp_md5;
		}
		return data.ori_md5;
	}

	bool FileMD5Storage::isCompressed(const String& file_key) const
	{
		if (!hasFileKey(file_key)) { return false; }
		return !(file_to_data_.at(file_key).cmp_md5.empty());
	}

	void FileMD5Storage::addData(const String& file_key, const String& ori_md5, const String& cmp_md5)
	{
		if (file_to_data_.find(file_key) == file_to_data_.end())
		{
			file_to_data_[file_key] = FileMD5Data();
		}
		FileMD5Data& data = file_to_data_[file_key];
		data.file_key = file_key;
		data.ori_md5 = ori_md5;
		data.cmp_md5 = cmp_md5;

		storeToMap(data);
	}

	void FileMD5Storage::addData(const FileMD5Storage::FileMD5Data& data)
	{
		if (data.file_key.empty()) { return; }
		file_to_data_[data.file_key] = data;
	}

	String FileMD5Storage::saveToFile()
	{
		if (cfg_dir_path_.empty())
		{
			cfg_dir_path_ = Root::Instance()->getGamePath();
		}

		auto res_config = StringUtil::Format("%sgame_res_config.txt", cfg_dir_path_.c_str());

		std::fstream f;
		f.open(res_config.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		LORD::DataStream* filePtr(LordNew LORD::FileStreamDataStream(&f));
		if (filePtr)
		{
			for (auto pair : file_to_data_)
			{
				const FileMD5Data& data = pair.second;
				LORD::String ss = StringUtil::Format("%s=%s---%s\r\n", 
					pair.first.c_str(), 
					data.ori_md5.c_str(),
					data.cmp_md5.c_str());
				filePtr->write(ss.c_str(), ss.size());
			}
		}
		f.close();
		return res_config;
	}

	void FileMD5Storage::loadFromFile(const String& res_config)
	{
		FILE* file = fopen(res_config.c_str(), "rb");
		if (!file) return;

		fseek(file, 0, SEEK_END);
		auto size = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* data = static_cast<char*>(LordMalloc(size));
		fread(data, sizeof(char), size, file);
		fclose(file);
		String md5s = String(data, size);
		LordFree(data);

		StringArray lines = StringUtil::Split(md5s, "\r\n");
		for (auto line : lines)
		{
			parseLineAndAdd(line);
		}
		return;
	}

	void FileMD5Storage::parse(const String& content)
	{
		StringArray lines = StringUtil::Split(content, "\r\n");
		for (auto line : lines)
		{
			parseLineAndAdd(line);
		}
	}

	void FileMD5Storage::getKeyToDownloadMD5Map(std::map<String, String>& res_to_md5)
	{
		for (auto pair : file_to_data_)
		{
			const String& file = pair.first;
			const String& ori_md5 = pair.second.ori_md5;
			const String& cmp_md5 = pair.second.cmp_md5;
			if (!file.empty())
			{
				if (!cmp_md5.empty())
				{
					res_to_md5[file] = cmp_md5;
				}
				else if (!ori_md5.empty())
				{
					res_to_md5[file] = ori_md5;
				}
			}
		}
	}

	LORD::String FileMD5Storage::getCompressedMD5ByFileKey(const String& file_key) const
	{
		if (!hasFileKey(file_key)) { return ""; }
		return file_to_data_.at(file_key).cmp_md5;
	}

	LORD::String FileMD5Storage::getPureOriMD5ByFileKey(const String& file_key) const
	{
		if (!hasFileKey(file_key)) { return ""; }
		return getPureMD5(file_to_data_.at(file_key).ori_md5);
	}

	LORD::FileMD5Storage::FileMD5Data FileMD5Storage::getDataByFileKey(const String& file_key) const
	{
		if (hasFileKey(file_key)) { return file_to_data_.at(file_key); }
		return FileMD5Data();
	}

	void FileMD5Storage::storeToMap(const FileMD5Data& data)
	{
		const String& file_key = data.file_key;

		ori_md5_to_file_[getPureMD5(data.ori_md5)].insert(file_key);
		cmp_md5_to_file_[getPureMD5(data.cmp_md5)].insert(file_key);
	}

	String FileMD5Storage::getPureMD5(const String& val) const
	{
		String md5 = val;
		StringArray str_arr = StringUtil::Split(val, String(1, PathUtil::SLASH));
		if (str_arr.size() >= 1)
		{
			md5 = str_arr.back();
		}
		return md5;
	}

	void FileMD5Storage::parseLineAndAdd(const String& line)
	{
		StringArray tempVec = StringUtil::Split(line, "=");
		if (tempVec.size() == 2)
		{
			const String& file_key = tempVec[0];
			StringArray md5_pair = StringUtil::Split(tempVec[1], MD5_SEPERATOR);
			if (md5_pair.size() >= 1)
			{
				FileMD5Data& data = file_to_data_[file_key];
				data.file_key = file_key;
				data.ori_md5 = md5_pair.front();
				if (md5_pair.size() == 2)
				{
					data.cmp_md5 = md5_pair.back();
				}
				storeToMap(data);
			}
		}
	}

}