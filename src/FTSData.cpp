#include "FTSData.h"

#include <io.h>
#include <fstream>
#include <iostream>

using namespace std;

//bool __fastcall GetInstanceKeywordStr(RE::TESObjectWEAP::InstanceData* a_instance, std::string* a_prefix);
namespace ScopeData
{
	FTSData::FTSData(std::string pathO)
	{
		path = pathO;
	}

#pragma region jsonRead

	void from_json(const json& j, Parallax& p)
	{
		p.radius = j.value("radius", 0.0F);
		p.relativeFogRadius = j.value("relativeFogRadius", 0.0F);
		p.scopeSwayAmount = j.value("scopeSwayAmount", 0.0F);
		p.maxTravel = j.value("maxTravel", 0.0F);
	}

	void from_json(const json& j, ZoomDataOverwrite& z)
	{
		z.x = j.value("x", 0.0F);
		z.y = j.value("y", 0.0F);
		z.z = j.value("z", 0.0F);
		z.fovMul = j.value("fovMul", 1.0F);
		z.enableZoomDateOverwrite = j.value("enableZoomDateOverwrite", false);
	}

	void from_json(const json& j, ShaderData& s)
	{
		s.IsCircle = j.value("IsCircle", true);
		s.bEnableZMove = j.value("EnableZMove", false);
		s.bCanEnableNV = j.value("EnableNV", false);
		s.bBoltDisable = j.value("bBoltDisable", false);
		s.nvIntensity = j.value("nvIntensity", 3.0F);
		s.baseWeaponPos = j.value("BaseWeaponPos", 0.0F);
		s.movePercentage = j.value("ZMovePercentage", 0.0F);
		s.camDepth = j.value("CamDepth", 1.0F);
		s.minZoom = j.value("MinZoom", 1.0F);
		s.maxZoom = j.value("MaxZoom", 4.0F);

		json defaultArrayOffset = { 0.0F, 0.0F };
		json defaultArraySize = { 400.0F, 400.0F };
		json defaultRectArray = { 235.0F, 260.0F, 775.0F, 760.0F };

		if (j.contains("PositionOffset"))
		{
			auto positionOffsetJson = j.value("PositionOffset", defaultArrayOffset);
			positionOffsetJson.at("x").get_to(s.PositionOffset[0]);
			positionOffsetJson.at("y").get_to(s.PositionOffset[1]);
		}
		
		if (j.contains("OriPositionOffset"))
		{
			auto oriPositionOffsetJson = j.value("OriPositionOffset", defaultArrayOffset);
			oriPositionOffsetJson.at("x").get_to(s.OriPositionOffset[0]);
			oriPositionOffsetJson.at("y").get_to(s.OriPositionOffset[1]);
		}
		
		if (j.contains("OriPositionOffset")) 
		{
			auto sizeJson = j.value("Size", defaultArraySize);
			sizeJson.at("x").get_to(s.Size[0]);
			sizeJson.at("y").get_to(s.Size[1]);
		}
		
		if (j.contains("OriSize"))
		{
			auto oriSizeJson = j.value("OriSize", defaultArraySize);
			oriSizeJson.at("x").get_to(s.OriSize[0]);
			oriSizeJson.at("y").get_to(s.OriSize[1]);
		}

		if (j.contains("rectSize"))
		{
			auto rectSizeJson = j.value("rectSize", defaultRectArray);

			rectSizeJson.at("x").get_to(s.rectSize[0]);
			rectSizeJson.at("y").get_to(s.rectSize[1]);
			rectSizeJson.at("z").get_to(s.rectSize[2]);
			rectSizeJson.at("w").get_to(s.rectSize[3]);
		}
		

		s.ReticleSize = j.value("ReticleSize", 4.0F);
		s.fovAdjust = j.value("fovAdjust", 0.0F);
		s.parallax = j.value("Parallax", Parallax());
	}

	void from_json(const json& j, FTSData& f)
	{
		f.keywordName = j.value("keywordEditorID", "FTS_Default");
		f.animFlavorEditorID = j.value("AnimFlavorKeywordEditorID", "FTS_NONE");
		f.additionalKeywordsStr = j.value("AdditionalKeywords", "");

		std::stringstream ss(f.additionalKeywordsStr);
		std::string token;
		while (getline(ss, token, ','))
		{
			f.additionalKeywords.push_back(token);
		}

		f.legacyMode = j.value("LegacyMode", true);
		f.version = j.value("Version", 1);
		f.UsingSTS = j.value("UsingSTS", false);
		f.scopeFrame = j.value("scopeFrame", 1);
		f.ZoomNodePath = j.value("ReticleTexturePath", "");

		f.shaderData = j.value("ShaderData", ShaderData());
		f.zoomDataOverwrite = j.value("ZoomDataOverwrite", ZoomDataOverwrite());
	}

#pragma endregion

#pragma region jsonWrite

	void to_json(json& j, const Parallax& p)
	{
		j = json{
			{ "radius", p.radius },
			{ "relativeFogRadius", p.relativeFogRadius },
			{ "scopeSwayAmount", p.scopeSwayAmount },
			{ "maxTravel", p.maxTravel }
		};
	}

	void to_json(json& j, const ZoomDataOverwrite& z)
	{
		j = json{
			{ "x", z.x },
			{ "y", z.y },
			{ "z", z.z },
			{ "fovMul", z.fovMul },
			{ "enableZoomDateOverwrite", z.enableZoomDateOverwrite }
		};
	}

	void to_json(json& j, const ShaderData& s)
	{
		j = json{
			{ "IsCircle", s.IsCircle },
			{ "EnableZMove", s.bEnableZMove },
			{ "EnableNV", s.bCanEnableNV },
			{ "bBoltDisable", s.bBoltDisable },
			{ "nvIntensity", s.nvIntensity },
			{ "BaseWeaponPos", s.baseWeaponPos },
			{ "ZMovePercentage", s.movePercentage },
			{ "CamDepth", s.camDepth },
			{ "MinZoom", s.minZoom },
			{ "MaxZoom", s.maxZoom },
			{ "PositionOffset", { { "x", s.PositionOffset[0] }, { "y", s.PositionOffset[1] } } },
			{ "OriPositionOffset", { { "x", s.OriPositionOffset[0] }, { "y", s.OriPositionOffset[1] } } },
			{ "Size", { { "x", s.Size[0] }, { "y", s.Size[1] } } },
			{ "OriSize", { { "x", s.OriSize[0] }, { "y",s.OriSize[1] } } },
			{ "rectSize", { { "x", s.rectSize[0] }, { "y", s.rectSize[1] }, { "z", s.rectSize[2] }, { "w", s.rectSize[3] } } },

			{ "ReticleSize", s.ReticleSize },
			{ "fovAdjust", s.fovAdjust },
			//
			{ "Parallax", s.parallax }
		};
	}

	// 为FTSData类型定义to_json函数
	void to_json(json& j, const FTSData& f)
	{
		std::ostringstream oss;
		for (int i = 0; i < f.additionalKeywords.size(); i++)
		{
			oss << f.additionalKeywords[i];
			if (i < f.additionalKeywords.size() - 1)
				oss << ",";
		}
		std::string tempkeyStr = oss.str();

		j = json{
			{ "keywordEditorID", f.keywordName },
			{ "AnimFlavorKeywordEditorID", f.animFlavorEditorID },
			{ "AdditionalKeywords", tempkeyStr },
			{ "LegacyMode", f.legacyMode },
			{ "path", f.path },
			{ "Version", f.version },
			{ "UsingSTS", f.UsingSTS },
			{ "scopeFrame", f.scopeFrame },
			{ "ReticleTexturePath", f.ZoomNodePath },
			//
			{ "ShaderData", f.shaderData },
			{ "ZoomDataOverwrite", f.zoomDataOverwrite }
		};
	}

#	pragma endregion


	void ScopeDataHandler::TestingJson()
	{
		
	}

	void ScopeDataHandler::WriteCurrentFTSData()
	{
		if (currentData) 
		{
			std::string temp = currentData->path;

			std::ifstream f(temp);
			json j = *currentData;

			std::ofstream o(temp);
			o <<  j << std::endl;

			f.close();
			o.close();
		}
	}

	void ScopeDataHandler::ReloadFTSData(FTSData* data)
	{
		std::ifstream f(data->path);

		json j = json::parse(f);
		
		j.get_to(*data);

		ScopeData::ScopeDataHandler::GetSingleton()->SetCurrentFTSData(data);

		ScopeData::ScopeDataHandler::GetSingleton()->UpdateFTSData(data->path);

		f.close();
	}

	void ScopeDataHandler::ReloadCurrentFTSData()
	{

	}

	void ScopeDataHandler::SetCurrentFTSData(FTSData* data,bool containsAllAdditionkeyword)
	{
		currentData = data;
		if (data)
			data->containAlladditionalKeywords = containsAllAdditionkeyword;
	}

	FTSData* ScopeDataHandler::GetCurrentFTSData()
	{
		return currentData;
	}
	
	ScopeDataHandler* ScopeDataHandler::GetSingleton()
	{
		static ScopeDataHandler singleton;
		return std::addressof(singleton);
	}

	bool ScopeDataHandler::ReadScopeData(string path)
	{
		if (path.find("__folder_managed_by_vortex") != std::string::npos)
			return false;

		std::ifstream f(path);

		FTSData* tempdata = new FTSData(path);

		ReloadFTSData(tempdata);

		ScopeDataMap.emplace(tempdata->keywordName.data(), tempdata);

		//try {
		//	
		//} catch (nlohmann::detail::exception& e) {
		//	reshade::log_message(4, e.what());
		//	string strRet = "{\"result\":0}";
		//}
		
		return true;
	}

	int ScopeDataHandler::GetEffectIndex()
	{
		return PassRenderIndex;
	}

	void ScopeDataHandler::SetEffectIndex(int renderIndex)
	{

		PassRenderIndex = renderIndex;

		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);

		data["RenderPassIndex"] = PassRenderIndex;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
	}


	void ScopeDataHandler::SetIsUpscaler(bool flag)
	{
		isUpscaler = flag;
	}

	bool* ScopeDataHandler::GetEnableRenderThroughUI()
	{
		
		return &isUpscaler;
	}

	void ScopeDataHandler::SaveEnableRenderBeforeUI(bool flag)
	{
		//bEnableRenderBeforeUI = flag;

		if (isUpscaler)
			return;


		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);

		data["EnableRenderBeforeUI"] = bEnableRenderBeforeUI;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
	}

	int ScopeDataHandler::GetBaseRenderCount()
	{
		return baseRenderCount;
	}

	void ScopeDataHandler::SetBaseRenderCount(int renderIndex)
	{
		baseRenderCount = renderIndex;

		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);

		data["BaseRenderCount"] = baseRenderCount;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
	}

	int ComboKeyToInt(std::string comboStr)
	{
		std::for_each(
			comboStr.begin(),
			comboStr.end(),
			[](char& c) {
				c = ::tolower(c);
			});

		int comboNVKey;

		if (comboStr.compare("shift") == 0)
			comboNVKey = 160;
		else if (comboStr.compare("rshift") == 0)
			comboNVKey = 161;
		else if (comboStr.compare("ctrl") == 0)
			comboNVKey = 162;
		else if (comboStr.compare("rctrl") == 0)
			comboNVKey = 163;
		else if (comboStr.compare("alt") == 0)
			comboNVKey = 164;
		else if (comboStr.compare("ralt") == 0)
			comboNVKey = 165;
		else
			comboNVKey = -1;
		return comboNVKey;
	}

	void ScopeDataHandler::ReadDefaultScopeDataFile()
	{
		/*string path = "Data\\F4SE\\Plugins\\FTS.json";

		ReadScopeData(path);*/

		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);
		PassRenderIndex = data["RenderPassIndex"].is_null() ? 1 : (int)data["RenderPassIndex"];
		bEnableRenderBeforeUI = data["EnableRenderBeforeUI"].is_null() ? 0 : (bool)(data["EnableRenderBeforeUI"]);
		baseRenderCount = data["BaseRenderCount"].is_null() ? 10 : (int)data["BaseRenderCount"];
		

		//std::string comboStr = data["ComboNVKey"].is_null() ? "null" : (std::string)data["ComboNVKey"];
		
		if (!data["ComboNVKey"].is_null()) 
		{
			if (data["ComboNVKey"].is_string()) 
				comboNVKey = ComboKeyToInt((std::string)data["ComboNVKey"]);

			else if (data["ComboNVKey"].is_number_integer())
				comboNVKey = (int)data["ComboNVKey"];
		}
		else comboNVKey = 0;


		nvKey = data["NvKey"].is_null() ? 0 : (int)data["NvKey"];
		guiKey = data["guiKey"].is_null() ? 117 : (int)data["guiKey"];

		if (data["guiKey"].is_null())
			data["guiKey"] = 117;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();

	}

	const char* ScopeDataHandler::GetNVGComboKeyStr()
	{

		const char* comboNVKeya;

		if (this->comboNVKey == 160 )
			comboNVKeya = "Shift";
		else if (this->comboNVKey == 161)
			comboNVKeya = "RShift";
		else if (this->comboNVKey == 162)
			comboNVKeya = "Ctrl";
		else if (this->comboNVKey == 163)
			comboNVKeya = "RCtrl";
		else if (this->comboNVKey == 164)
			comboNVKeya = "Alt";
		else if (this->comboNVKey == 165)
			comboNVKeya = "RAlt";
		else
			comboNVKeya = "None";
		return comboNVKeya;
	}

	void ScopeDataHandler::SetNVGHotKeyCombo(int comboKey)
	{
		comboNVKey = comboKey;


		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);

		data["ComboNVKey"] = comboKey;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
		
	}

	void ScopeDataHandler::SetNVGHotKeyMain(unsigned int mainkeycode)
	{

		nvKey = mainkeycode;

		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);

		data["NvKey"] = nvKey;

		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
	}

	void ScopeDataHandler::SetGuiKey(unsigned int mainkeycode)
	{
		guiKey = mainkeycode;

		string path = "Data\\F4SE\\Plugins\\FTSConfig.json";
		std::ifstream f(path);
		json data = json::parse(f);
		data["guiKey"] = guiKey;
		std::ofstream o(path);
		o << data << std::endl;
		f.close();
		o.close();
	}

	void ScopeDataHandler::ReadCustomScopeDataFiles(std::string path)
	{
		intptr_t hFile = 0;
		string p ="\\";
		struct _finddata_t fileinfo;

		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
			do {
				if ((fileinfo.attrib & _A_SUBDIR)) {
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						ScopeDataHandler::ReadCustomScopeDataFiles(p.assign(path).append("\\").append(fileinfo.name));
				} else {
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}

		for (auto& i : files) {
			ReadScopeData(i);
		}
	}

	void ScopeDataHandler::ReloadZoomData(std::string path)
	{
		ReadCustomScopeDataFiles(path);
		ReadDefaultScopeDataFile();
	
	}

	std::multimap<std::string, FTSData*>* ScopeDataHandler::GetScopeDataMap()
	{
		return &ScopeDataMap;
	}


	void ScopeDataHandler::UpdateFTSData(std::string path)
	{
		std::ifstream f(path);
		json j = json::parse(f);
	
		if (j["Version"].is_null() || (int)(j["Version"] == currentFTSDataVerion)) {
			f.close();
			return;
		}


		if ((int)(j["Version"] != currentFTSDataVerion)) {

		}


		j["Version"] = (int)currentFTSDataVerion;



		std::ofstream o(path);
		o << j << std::endl;
		f.close();
		o.close();
	}



	std::vector<std::string_view> splitSV(std::string_view strv, std::string_view delims)
	{
		std::vector<std::string_view> output;
		size_t first = 0;

		while (first < strv.size()) {
			const auto second = strv.find_first_of(delims, first);

			if (first != second)
				output.emplace_back(strv.substr(first, second - first));

			if (second == std::string_view::npos)
				break;

			first = second + 1;
		}

		return output;
	}

	//vector<string> ScopeDataHandler::GetFilesName()
	//{
	//	return files;
	//}
}
