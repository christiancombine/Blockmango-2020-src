#pragma once
#include <Core.h>
#include <vector>
#include <map>
namespace LORD
{
	// parse write store game_res_config.txt
	class FileMD5Storage
	{
	public:
		struct FileMD5Data
		{
			FileMD5Data()
				:file_key("")
				, ori_md5("")
				, cmp_md5("")
			{
			}
			FileMD5Data(const String& file_key, const String& ori_md5, const String& cmp_md5)
			{
				this->file_key = file_key;
				this->ori_md5 = ori_md5;
				this->cmp_md5 = cmp_md5;
			}
			// 可能带用户id
			String ori_md5;
			String cmp_md5;
			String file_key;

		};
	public:
		bool hasFileKey(const String& file_key) const;
		bool hasCompressedMD5(const String& md5) const;
		// 有压缩md5就传压缩，没有就传原文件md5，带用户id（如果有）
		String getDownLoadMD5(const String& file_key) const;
		bool isCompressed(const String& file_key) const;
		String getCompressedMD5ByFileKey(const String& file_key) const;
		// 不带用户ID
		String getPureOriMD5ByFileKey(const String& file_key) const;
		LORD::FileMD5Storage::FileMD5Data getDataByFileKey(const String& file_key) const;

		void addData(const FileMD5Storage::FileMD5Data& data);
		void addData(const String& file_key, const String& ori_md5, const String& cmp_md5);
		String saveToFile();
		void loadFromFile(const String& res_config);
		void parse(const String& content);
		void getKeyToDownloadMD5Map(std::map<String, String>& key_to_md5);
		
	private:
		void storeToMap(const FileMD5Data& data);
		String getPureMD5(const String& val) const;
		void parseLineAndAdd(const String& line);

	private:
		std::map<String, FileMD5Storage::FileMD5Data> file_to_data_;
		std::map<String, std::set<String>> ori_md5_to_file_;
		std::map<String, std::set<String>> cmp_md5_to_file_;
		String cfg_dir_path_ = "";
	};



}

