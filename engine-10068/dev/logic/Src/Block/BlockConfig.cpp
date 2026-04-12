#include "BlockConfig.h"
#include "rapidjson/schema.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "Trigger/TriggerConfigParser.h"
#include "Util/RapidJsonObject.h"
#include "Util/CSVReader.h"

#define RETURN_ON_FAIL(expr, msg, ...) if(!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace BLOCKMAN
{
	using namespace LORD;
	using namespace rapidjson;

	static char blockJsonSchema[] = R"JSON_SCHEMA(
{
	"type":"object",
	"description":"schema for Blockman block config json",
	"additionalProperties":false,
	"required":[
		"texture"
	],
	"properties":{
		"texture":{
			"type":[
				"string",
				"object"
			],
			"additionalProperties":false,
			"required":[
				"top",
				"bottom",
				"left",
				"right",
				"front",
				"back"
			],
			"properties":{
				"top":{
					"type":"string"
				},
				"bottom":{
					"type":"string"
				},
				"left":{
					"type":"string"
				},
				"right":{
					"type":"string"
				},
				"front":{
					"type":"string"
				},
				"back":{
					"type":"string"
				}
			}
		},
		"jigsawTexture":{
			"type":[
				"string",
				"object"
			],
			"additionalProperties":false,
			"required":[
				"jigsaw1",
				"jigsaw2",
				"jigsaw3",
				"jigsaw4"
			],
			"properties":{
				"jigsaw1":{
					"type":"string"
				},
				"jigsaw2":{
					"type":"string"
				},
				"jigsaw3":{
					"type":"string"
				},
				"jigsaw4":{
					"type":"string"
				}
			}
		},
		"jigsawBeginBlockId": {                                                     
        	"type": "number",                                            
        	"minimum": 0,                                                  
        	"maximum": 10000                                                
        },
		"color":{
			"type":"string",
			"pattern":"[0-9a-fA-F]{6}"
		},
		"blockColor":{
			"type":"string",
			"pattern":"[0-9a-fA-F]{6}"
		},
		"lightEmitted":{
			"type": [
				"string",
				"object"
			],
			"pattern":"[0-9a-fA-F]{3}",
			"additionalProperties":false,
			"required":[
				"r", "g", "b"
			],
			"properties":{
				"r":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				},
				"g":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				},
				"b":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				}
			}
		},
		"lightDecay":{
			"type": [
				"string",
				"object"
			],
			"pattern":"[0-9a-fA-F]{3}",
			"additionalProperties":false,
			"required":[
				"r", "g", "b"
			],
			"properties":{
				"r":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				},
				"g":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				},
				"b":{
					"type":"integer",
					"minimum":0,
					"maximum":15
				}
			}
		},
		"lightStrength":{
			"type":"number",
			"minimum":1,
			"maximum":250
		},
		"state":{
			"enum":[
				"GAS",
				"SOLID"
			]
		},
		"transparent":{
			"type":"boolean"
		},
		"requiresNoTool":{
			"type":"boolean"
		},
		"transparentMode":{
			"enum":[
				"ALPHA_BLEND",
				"MULTIPLY",
				"LIGHT_MULTIPLY"
			]
		},
		"transparentGridMode": {
			"type":"boolean"
		},
		"renderable":{
			"type":"boolean"
		},
		"collisionBox":{
			"type":[
				"object",
				"null"
			],
			"additionalProperties":false,
			"required":[
				"min",
				"max"
			],
			"properties":{
				"min":{
					"$ref":"#/definitions/coordinate"
				},
				"max":{
					"$ref":"#/definitions/coordinate"
				}
			}
		},
		"resistance":{
			"type":"number",
			"minimum":0,
			"maximum":10000
		},
		"hardness": {                                                     
        	"type": "number",                                            
        	"minimum": 0,                                                  
        	"maximum": 10000                                                
        },
		"flammable":{
			"type":"boolean"
		},
		"triggers":{
			"type":"array",
			"items":{
				"type":"object",
				"additionalProperties":true,
				"required":[
					"type",
					"actions"
				],
				"properties":{
					"type":{
						"type":"string"
					},
					"actions":{
						"type":"array",
						"items":{
							"type":"object",
							"additionalProperties":true,
							"required":[
								"type"
							],
							"properties":{
								"type":{
									"type":"string"
								}
							}
						}
					}
				}
			}
		},
		"blockFaceToPlayer": {
			"type":"boolean"
		},
		"blockAnim": {
			"type": "object",
			"properties":{
				"animType": {
					"type": "string"
				},
				"frames": {
					"type": "string"
				},
				"speed": {
					"type": "number",
					"minimum": -10000,                                                  
        			"maximum": 10000  
				},
				"frameSize": {
					"type": "number",
					"minimum": 1,                                                  
        			"maximum": 10000   
				},
				"top":{
					"$ref":"#/definitions/anim"
				},
				"bottom":{
					"$ref":"#/definitions/anim"
				},
				"left":{
					"$ref":"#/definitions/anim"
				},
				"right":{
					"$ref":"#/definitions/anim"
				},
				"front":{
					"$ref":"#/definitions/anim"
				},
				"back":{
					"$ref":"#/definitions/anim"
				}
			}
		}
	},
	"definitions":{
		"coordinate":{
			"type":"object",
			"additionalProperties":false,
			"required":[
				"x",
				"y",
				"z"
			],
			"properties":{
				"x":{
					"type":"number"
				},
				"y":{
					"type":"number"
				},
				"z":{
					"type":"number"
				}
			}
		},
		"anim": {
			"type":"object",
			"properties":{
				"animType": {
					"type": "string"
				},
				"speed": {
					"type": "number",
					"minimum": -10000,                                                  
        			"maximum": 10000   
				},
				"frameSize": {
					"type": "number",
					"minimum": 1,                                                  
        			"maximum": 10000   
				}
			}
		}
	}
}
)JSON_SCHEMA";

	static Document createSchemaJson()
	{
		Document doc;
		doc.Parse(blockJsonSchema);
		LordAssert(!doc.HasParseError());
		return doc;
	}

	static bool validateJson(const Document & doc)
	{
		static SchemaDocument schema(createSchemaJson());
		SchemaValidator validator(schema);
		if (!doc.Accept(validator))
		{
			StringBuffer sb;
			validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
			LordLogError("Invalid schema: %s", sb.GetString());
			LordLogError("Invalid keyword: %s", validator.GetInvalidSchemaKeyword());
			sb.Clear();
			validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
			LordLogError("Invalid document: %s", sb.GetString());
			return false;
		}
		return true;
	}

	AnimType parsingAnimType(const LORD::String type)
	{
		if (type == "frame" || type == "1")
		{
			return AnimType::FRAME;
		}
		else if (type == "rolling" || type == "2")
		{
			return AnimType::ROLLING;
		}
		else if (type == "vertical_rolling" || type == "3")
		{
			return AnimType::VERTICAL_ROLLING;
		}
		else if (type == "rotation" || type == "4")
		{
			return AnimType::ROTATION;
		}
		else
		{
			return AnimType::NONE;
		}
	}

	static void parsingBlockAnim(const char* surface, BlocksAnimConfig& config, const Document& json)
	{
		const auto& face = json["blockAnim"].GetObject()[surface].GetObject();
		if (face.HasMember("animType"))
		{
			config.animType = parsingAnimType(face["animType"].GetString());
		}
		if (face.HasMember("speed"))
		{
			config.speed = face["speed"].GetInt();
		}
		if (face.HasMember("frameSize"))
		{
			config.frameSize = face["frameSize"].GetInt();
		}
	}

	bool BlockConfig::readJson(BlockConfig & output, const String& configPath)
	{
		if (!PathUtil::IsFileExist(configPath) && validateTextureConfig(output.texture))
		{
			return true;
		}

		RETURN_ON_FAIL(PathUtil::IsFileExist(configPath), "cannot read block.json: json file does not exist: %s", configPath.c_str());
		std::ifstream ifs(configPath.c_str());
		IStreamWrapper is(ifs);
		Document json;
		RETURN_ON_FAIL(!json.ParseStream(is).HasParseError(), "cannot read block.json: failed to parse json");
		RETURN_ON_FAIL(validateJson(json), "cannot read block.json: block json is invalid");

		if (json.HasMember("texture"))
		{
			if (json["texture"].IsString())
			{
				output.texture.top
					= output.texture.bottom
					= output.texture.left
					= output.texture.right
					= output.texture.front
					= output.texture.back 
					= json["texture"].GetString();
			}
			else
			{
				const auto& textureJson = json["texture"].GetObject();
				output.texture.top = textureJson["top"].GetString();
				output.texture.bottom = textureJson["bottom"].GetString();
				output.texture.left = textureJson["left"].GetString();
				output.texture.right = textureJson["right"].GetString();
				output.texture.front = textureJson["front"].GetString();
				output.texture.back = textureJson["back"].GetString();
				if (!validateTextureConfig(output.texture))
				{
					LordLogError("cannot read block.json: texture config is invalid");
					return false;
				}
			}
		}
		
		if (json.HasMember("blockFaceToPlayer"))
		{
			output.blockFaceToPlayer = json["blockFaceToPlayer"].GetBool();
		}

		if (json.HasMember("blockAnim"))
		{
			const auto& blocksAnimJson = json["blockAnim"].GetObject();
			if (blocksAnimJson.HasMember("animType"))
			{
				output.anim.top.animType
					= output.anim.bottom.animType
					= output.anim.left.animType
					= output.anim.right.animType
					= output.anim.front.animType
					= output.anim.back.animType
					= parsingAnimType(blocksAnimJson["animType"].GetString());
			}
			if (blocksAnimJson.HasMember("speed"))
			{
				output.anim.top.speed
					= output.anim.bottom.speed
					= output.anim.left.speed
					= output.anim.right.speed
					= output.anim.front.speed
					= output.anim.back.speed
					= blocksAnimJson["speed"].GetInt();
			}
			if (blocksAnimJson.HasMember("frameSize"))
			{
				output.anim.top.frameSize
					= output.anim.bottom.frameSize
					= output.anim.left.frameSize
					= output.anim.right.frameSize
					= output.anim.front.frameSize
					= output.anim.back.frameSize
					= blocksAnimJson["frameSize"].GetInt();
			}

			if (blocksAnimJson.HasMember("top"))
			{
				parsingBlockAnim("top", output.anim.top, json);
			}

			if (blocksAnimJson.HasMember("bottom"))
			{
				parsingBlockAnim("bottom", output.anim.bottom, json);
			}

			if (blocksAnimJson.HasMember("left"))
			{
				parsingBlockAnim("left", output.anim.left, json);
			}

			if (blocksAnimJson.HasMember("right"))
			{
				parsingBlockAnim("right", output.anim.right, json);
			}

			if (blocksAnimJson.HasMember("front"))
			{
				parsingBlockAnim("front", output.anim.front, json);
			}

			if (blocksAnimJson.HasMember("back"))
			{
				parsingBlockAnim("back", output.anim.back, json);
			}			
		}
		if (json.HasMember("jigsawTexture"))
		{
			if (json["jigsawTexture"].IsString())
			{
				output.jigsawTexture.jigsaw1
					= output.jigsawTexture.jigsaw2
					= output.jigsawTexture.jigsaw3
					= output.jigsawTexture.jigsaw4
					= json["jigsawTexture"].GetString();
			}
			else
			{
				const auto& textureJson = json["jigsawTexture"].GetObject();
				output.jigsawTexture.jigsaw1 = textureJson["jigsaw1"].GetString();
				output.jigsawTexture.jigsaw2 = textureJson["jigsaw2"].GetString();
				output.jigsawTexture.jigsaw3 = textureJson["jigsaw3"].GetString();
				output.jigsawTexture.jigsaw4 = textureJson["jigsaw4"].GetString();
				if (!validateJigsawTextureConfig(output.jigsawTexture))
				{
					LordLogError("cannot read block.json: jigsaw texture config is invalid");
					return false;
				}
			}
		}
		if (json.HasMember("jigsawBeginBlockId"))
		{
			output.jigsawBeginBlockId = json["jigsawBeginBlockId"].GetInt();
		}

		if (json.HasMember("blockColor"))
		{
			String colorString = json["blockColor"].GetString();
			int r = StringUtil::ParseHex<int>(colorString.substr(0, 2));
			int g = StringUtil::ParseHex<int>(colorString.substr(2, 2));
			int b = StringUtil::ParseHex<int>(colorString.substr(4, 2));
			output.blockColor = {
				static_cast<Real>(r) / 255,
				static_cast<Real>(g) / 255,
				static_cast<Real>(b) / 255,
			};
		}

		if (json.HasMember("color"))
		{
			String colorString = json["color"].GetString();
			int r = StringUtil::ParseHex<int>(colorString.substr(0, 2));
			int g = StringUtil::ParseHex<int>(colorString.substr(2, 2));
			int b = StringUtil::ParseHex<int>(colorString.substr(4, 2));
			output.color = {
				static_cast<Real>(r) / 255,
				static_cast<Real>(g) / 255,
				static_cast<Real>(b) / 255,
			};
		}
		if (json.HasMember("lightEmitted"))
		{
			if (json["lightEmitted"].IsString())
			{
				String colorString = json["lightEmitted"].GetString();
				output.lightEmitted.red = StringUtil::ParseHex<int>(colorString.substr(0, 1));
				output.lightEmitted.green = StringUtil::ParseHex<int>(colorString.substr(1, 1));
				output.lightEmitted.blue = StringUtil::ParseHex<int>(colorString.substr(2, 1));
			}
			else
			{
				const auto& colorJson = json["lightEmitted"].GetObject();
				output.lightEmitted.red = colorJson["r"].GetInt();
				output.lightEmitted.green = colorJson["g"].GetInt();
				output.lightEmitted.blue = colorJson["b"].GetInt();
			}
		}
		if (json.HasMember("lightDecay"))
		{
			if (json["lightDecay"].IsString())
			{
				String colorString = json["lightDecay"].GetString();
				output.lightDecay.red = StringUtil::ParseHex<int>(colorString.substr(0, 1));
				output.lightDecay.green = StringUtil::ParseHex<int>(colorString.substr(1, 1));
				output.lightDecay.blue = StringUtil::ParseHex<int>(colorString.substr(2, 1));
			}
			else
			{
				const auto& colorJson = json["lightDecay"].GetObject();
				output.lightDecay.red = colorJson["r"].GetInt();
				output.lightDecay.green = colorJson["g"].GetInt();
				output.lightDecay.blue = colorJson["b"].GetInt();
			}
		}

		if (json.HasMember("lightStrength"))
		{
			output.lightStrength = json["lightStrength"].GetInt() & 0xff;
		}

		if (json.HasMember("state"))
		{
			String stateString = json["state"].GetString();
			if (stateString == "SOLID")
			{
				output.state = BlockState::SOLID;
			}
			else if (stateString == "GAS")
			{
				output.state = BlockState::GAS;
			}
			else
			{
				LordLogError("cannot read block.json: unknown state value %s", stateString.c_str());
				return false;
			}
		}
		if (json.HasMember("transparent"))
		{
			output.transparent = json["transparent"].GetBool();
		}
		if (json.HasMember("requiresNoTool"))
		{
			output.requiresNoTool = json["requiresNoTool"].GetBool();
		}
		if (json.HasMember("transparentMode"))
		{
			String modeString = json["transparentMode"].GetString();
			if (modeString == "ALPHA_BLEND")
			{
				output.transparentMode = TransparentMode::ALPHA_BLEND;
			}
			else if (modeString == "MULTIPLY")
			{
				output.transparentMode = TransparentMode::MULTIPLY;
			}
			else if (modeString == "LIGHT_MULTIPLY")
			{
				output.transparentMode = TransparentMode::LIGHT_MULTIPLY;
			}
			else
			{
				LordLogError("cannot read block.json: unknown transparent mode value %s", modeString.c_str());
				return false;
			}
		}
		if (json.HasMember("transparentGridMode"))
		{
			output.transparentGridMode = json["transparentGridMode"].GetBool();
		}
		if (json.HasMember("renderable"))
		{
			output.renderable = json["renderable"].GetBool();
		}
		if (json.HasMember("collisionBox"))
		{
			const auto& boxMinJson = json["collisionBox"].GetObject()["min"].GetObject();
			const auto& boxMaxJson = json["collisionBox"].GetObject()["max"].GetObject();
			output.collisionBox = {
				{
					boxMinJson["x"].GetFloat(),
					boxMinJson["y"].GetFloat(),
					boxMinJson["z"].GetFloat()
				},
				{
					boxMaxJson["x"].GetFloat(),
					boxMaxJson["y"].GetFloat(),
					boxMaxJson["z"].GetFloat()
				}
			};
		}
		if (json.HasMember("resistance"))
		{
			output.resistance = json["resistance"].GetFloat();
		}
		if (json.HasMember("hardness"))
		{
			output.hardness = json["hardness"].GetFloat();
		}
		if (json.HasMember("flammable"))
		{
			output.flammable = json["flammable"].GetBool();
		}
		if (json.HasMember("triggers"))
		{
			for (const auto& triggerJson : json["triggers"].GetArray())
			{
				if (auto trigger = TriggerConfigParser::Instance()->parse({ triggerJson.GetObject() }))
				{
					output.triggers.push_back(trigger);
				}
			}
		}
		return true;
	}

	bool BlockConfig::readCsvToMap(std::unordered_map<int, BlockConfig>& configMap, const LORD::String& configPath)
	{
		CsvReader pReader;

		std::string name = PathUtil::ConcatPath(configPath, "CustomBlocks-base.csv").c_str();
		if (!pReader.LoadFile(name))
		{
			LordLogInfo("cannot read CustomBlocks-base.csv: csv file does not exist: %s", name.c_str());
			return false;
		}

		pReader.Read();

		while (pReader.Read())
		{
			BlockConfig config;

			int id;
			pReader.GetInt("blockId", id);

			pReader.GetInt("jigsawBeginBlockId", config.jigsawBeginBlockId, -1);
			if (config.jigsawBeginBlockId > 10000 || config.jigsawBeginBlockId < -1)
			{
				LordLogError("cannot read CustomBlocks-base.csv: jigsawBeginBlockId value should be 0 minus %d. blockId is %d", config.jigsawBeginBlockId, id);
				continue;
			}

			pReader.GetString("jigsaw1", config.jigsawTexture.jigsaw1);
			pReader.GetString("jigsaw2", config.jigsawTexture.jigsaw2);
			pReader.GetString("jigsaw3", config.jigsawTexture.jigsaw3);
			pReader.GetString("jigsaw4", config.jigsawTexture.jigsaw4);

			pReader.GetBool("blockFaceToPlayer", config.blockFaceToPlayer);

			int lightStrength = 0;
			pReader.GetInt("lightStrength", lightStrength);
			config.lightStrength = lightStrength & 0xff;

			std::string stateString;
			pReader.GetString("state", stateString);
			if (stateString == "SOLID" || stateString == "1")
			{
				config.state = BlockState::SOLID;
			}
			else if (stateString == "GAS" || stateString == "2")
			{
				config.state = BlockState::GAS;
			}
			else
			{
				LordLogError("cannot read CustomBlocks-base.csv: unknown state value %s. blockId is %d", stateString.c_str(), id);
				continue;
			}

			String colorString;
			pReader.GetString("color", colorString);
			if (colorString.length() == 6)
			{
				int r = StringUtil::ParseHex<int>(colorString.substr(0, 2));
				int g = StringUtil::ParseHex<int>(colorString.substr(2, 2));
				int b = StringUtil::ParseHex<int>(colorString.substr(4, 2));
				config.color = {
					static_cast<Real>(r) / 255,
					static_cast<Real>(g) / 255,
					static_cast<Real>(b) / 255,
				};
			}

			String blockColorString;
			pReader.GetString("blockColor", blockColorString);
			if (blockColorString.length() == 6)
			{
				int r = StringUtil::ParseHex<int>(blockColorString.substr(0, 2));
				int g = StringUtil::ParseHex<int>(blockColorString.substr(2, 2));
				int b = StringUtil::ParseHex<int>(blockColorString.substr(4, 2));
				config.blockColor = {
					static_cast<Real>(r) / 255,
					static_cast<Real>(g) / 255,
					static_cast<Real>(b) / 255,
				};
			}

			std::string lightEmitted;
			pReader.GetString("lightEmitted", lightEmitted);
			if (lightEmitted != "")
			{
				std::vector<std::string> temp;
				CsvReader::SplitString(lightEmitted, temp, '#');
				if (temp.size() != 3)
				{
					LordLogError("cannot read CustomBlocks-base.csv: unknown lightEmitted value %s. blockId is %d", lightEmitted.c_str(), id);
					continue;
				}
				auto r = StringUtil::ParseInt(temp[0].c_str());
				auto g = StringUtil::ParseInt(temp[0].c_str());
				auto b = StringUtil::ParseInt(temp[0].c_str());
				if (r > 15 || g > 15 || b > 15)
				{
					LordLogError("cannot read CustomBlocks-base.csv: lightEmitted value should be 0 minus 15 %s. blockId is %d", lightEmitted.c_str(), id);
					continue;
				}
				config.lightEmitted.red = r;
				config.lightEmitted.green = g;
				config.lightEmitted.blue = b;

			}

			pReader.GetBool("transparent", config.transparent, false);

			std::string modeString;
			pReader.GetString("transparentMode", modeString);
			if (modeString != "")
			{
				if (modeString == "ALPHA_BLEND" || modeString == "1")
				{
					config.transparentMode = TransparentMode::ALPHA_BLEND;
				}
				else if (modeString == "MULTIPLY" || modeString == "2")
				{
					config.transparentMode = TransparentMode::MULTIPLY;
				}
				else if (modeString == "LIGHT_MULTIPLY" || modeString == "3")
				{
					config.transparentMode = TransparentMode::LIGHT_MULTIPLY;
				}
				else
				{
					LordLogError("cannot read CustomBlocks-base.csv: unknown transparent mode value %s.  blockId is %d", modeString.c_str(), id);
					continue;
				}
			}

			pReader.GetBool("transparentGridMode", config.transparentGridMode, false);

			pReader.GetFloat("resistance", config.resistance, 15.0);

			pReader.GetFloat("hardness", config.hardness, 15.0);

			pReader.GetBool("renderable", config.renderable, true);

			pReader.GetBool("flammable", config.flammable, false);

			pReader.GetBool("requiresNoTool", config.requiresNoTool, true);

			std::string min, max;
			pReader.GetString("collisionBox_min", min);
			pReader.GetString("collisionBox_max", max);
			std::vector<std::string> tempMin, tempMax;
			CsvReader::SplitString(min, tempMin, '#');
			CsvReader::SplitString(max, tempMax, '#');

			if (tempMin.size() != 3 || tempMax.size() != 3)
			{
				LordLogError("cannot read CustomBlocks-base.csv: unknown collision box value %s, %s. blockId is %d", min.c_str(), max.c_str(), id);
				continue;
			}

			config.collisionBox = {
				{
					StringUtil::ParseFloat(tempMin[0].c_str()),
					StringUtil::ParseFloat(tempMin[1].c_str()),
					StringUtil::ParseFloat(tempMin[2].c_str())
				},
				{
					StringUtil::ParseFloat(tempMax[0].c_str()),
					StringUtil::ParseFloat(tempMax[1].c_str()),
					StringUtil::ParseFloat(tempMax[2].c_str())
				}
			};

			configMap[id] = config;
		}

		name = PathUtil::ConcatPath(configPath, "CustomBlocks-face.csv").c_str();
		if (!pReader.LoadFile(name))
		{
			LordLogError("cannot read CustomBlocks-base.csv: csv file does not exist: %s", name.c_str());
			return false;
		}

		pReader.Read();

		
		constexpr int defaultFrameSize = 64;
		while (pReader.Read())
		{
			int id;
			pReader.GetInt("blockId", id);

			if (configMap.find(id) == configMap.end())
			{
				continue;
			}

			auto& config = configMap[id];
			std::string face;
			pReader.GetString("face", face);

			if (face == "all")
			{
				std::string temp;
				pReader.GetString("type", temp);
				config.texture.top
					= config.texture.bottom
					= config.texture.left
					= config.texture.right
					= config.texture.front
					= config.texture.back
					= temp.c_str();

				temp.clear();
				pReader.GetString("animType", temp);
				config.anim.top.animType
					= config.anim.bottom.animType
					= config.anim.left.animType
					= config.anim.right.animType
					= config.anim.front.animType
					= config.anim.back.animType
					= parsingAnimType(temp.c_str());

				int speed = 0;
				pReader.GetInt("speed", speed);
				config.anim.top.speed
					= config.anim.bottom.speed
					= config.anim.left.speed
					= config.anim.right.speed
					= config.anim.front.speed
					= config.anim.back.speed
					= speed;

				int frameSize = 0;
				pReader.GetInt("frameSize", frameSize);
				config.anim.top.frameSize
					= config.anim.bottom.frameSize
					= config.anim.left.frameSize
					= config.anim.right.frameSize
					= config.anim.front.frameSize
					= config.anim.back.frameSize
					= frameSize;
			}

			if (face == "top")
			{
				pReader.GetString("type", config.texture.top);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.top.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.top.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
			else if (face == "bottom")
			{
				pReader.GetString("type", config.texture.bottom);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.bottom.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.bottom.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
			else if (face == "left")
			{
				pReader.GetString("type", config.texture.left);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.left.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.left.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
			else if (face == "right")
			{
				pReader.GetString("type", config.texture.right);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.right.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.right.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
			else if (face == "front")
			{
				pReader.GetString("type", config.texture.front);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.front.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.front.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
			else if (face == "back")
			{
				pReader.GetString("type", config.texture.back);
				std::string animType;
				pReader.GetString("animType", animType);
				config.anim.back.animType = parsingAnimType(animType.c_str());
				pReader.GetInt("speed", config.anim.back.speed);
				pReader.GetInt("frameSize", config.anim.top.frameSize, defaultFrameSize);
			}
		}

		return true;
	}

	bool BlockConfig::validateTextureConfig(const TextureConfig & texture)
	{
		auto pos = texture.top.find_last_of(':');
		if (pos == String::npos)
		{
			return texture.bottom.find(':') == String::npos
				&& texture.left.find(':') == String::npos
				&& texture.right.find(':') == String::npos
				&& texture.front.find(':') == String::npos
				&& texture.back.find(':') == String::npos;
		}
		else
		{
			const auto& nameSpace = texture.top.substr(0, pos);
			return texture.bottom.substr(0, texture.bottom.find_last_of(':')) == nameSpace
				&& texture.left.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.right.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.front.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.back.substr(0, texture.left.find_last_of(':')) == nameSpace;
		}
	}

	bool BlockConfig::validateJigsawTextureConfig(const JigsawTextureConfig & texture)
	{
		auto pos = texture.jigsaw1.find_last_of(':');
		if (pos == String::npos)
		{
			return texture.jigsaw2.find(':') == String::npos
				&& texture.jigsaw3.find(':') == String::npos
				&& texture.jigsaw4.find(':') == String::npos;
		}
		else
		{
			const auto& nameSpace = texture.jigsaw1.substr(0, pos);
			return texture.jigsaw2.substr(0, texture.jigsaw2.find_last_of(':')) == nameSpace
				&& texture.jigsaw3.substr(0, texture.jigsaw3.find_last_of(':')) == nameSpace
				&& texture.jigsaw4.substr(0, texture.jigsaw4.find_last_of(':')) == nameSpace;
		}
	}
}