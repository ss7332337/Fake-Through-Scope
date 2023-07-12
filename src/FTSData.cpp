#include "FTSData.h"

#include <io.h>
#include <fstream>
#include <iostream>

using namespace std;

//bool __fastcall GetInstanceKeywordStr(RE::TESObjectWEAP::InstanceData* a_instance, std::string* a_prefix);

ScopeData::FTSData::FTSData(json j, std::string pathO)
{
	path = pathO;
	
	keywordName = j["keywordEditorID"].is_null() ? "FTS_Default" : j["keywordEditorID"];
	animFlavorEditorID = j["AnimFlavorKeywordEditorID"].is_null() ? "FTS_NONE" : j["AnimFlavorKeywordEditorID"];
	ReloadFTSData();
}

namespace ScopeData
{
	

	void ScopeDataHandler::TestingJson()
	{
		
	}

	void ScopeDataHandler::WriteCurrentFTSData()
	{
		if (currentData) 
		{
			std::string temp = currentData->path;

			std::ifstream f(temp);
			json j = json::parse(f);

			//j["keywordEditorID"] = currentData->keywordData.keywordName;
			j["LegacyMode"] = currentData->legacyMode;
			j["UsingSTS"] = currentData->UsingSTS;

			j["scopeFrame"] = currentData->scopeFrame;

			j["ShaderData"]["IsCircle"] = currentData->shaderData.IsCircle;
			j["ShaderData"]["EnableZMove"] = currentData->shaderData.bEnableZMove;
			j["ShaderData"]["EnableNV"] = currentData->shaderData.bCanEnableNV;
			j["ShaderData"]["bBoltDisable"] = currentData->shaderData.bBoltDisable;

			j["ShaderData"]["fovAdjust"] = currentData->shaderData.fovAdjust;
			j["ShaderData"]["BaseWeaponPos"] = currentData->shaderData.baseWeaponPos;
			j["ShaderData"]["ZMovePercentage"] = currentData->shaderData.movePercentage;
			j["ShaderData"]["NVIntensity"]  = currentData->shaderData.nvIntensity;

			j["ShaderData"]["CamDepth"] = currentData->shaderData.camDepth;
			j["ShaderData"]["ReticleSize"] = currentData->shaderData.ReticleSize;

			j["ShaderData"]["MinZoom"] = currentData->shaderData.minZoom;
			j["ShaderData"]["MaxZoom"] = currentData->shaderData.maxZoom;

			j["ShaderData"]["Size"]["x"] = currentData->shaderData.Size[0];
			j["ShaderData"]["Size"]["y"] = currentData->shaderData.Size[1];

			j["ShaderData"]["OriSize"]["x"] = currentData->shaderData.OriSize[0];
			j["ShaderData"]["OriSize"]["y"] = currentData->shaderData.OriSize[1];

			j["ShaderData"]["PositionOffset"]["x"] = currentData->shaderData.PositionOffset[0];
			j["ShaderData"]["PositionOffset"]["y"] = currentData->shaderData.PositionOffset[1];

			j["ShaderData"]["OriPositionOffset"]["x"] = currentData->shaderData.OriPositionOffset[0] ;
			j["ShaderData"]["OriPositionOffset"]["y"] = currentData->shaderData.OriPositionOffset[1] ;

			j["ShaderData"]["Parallax"]["radius"] = currentData->shaderData.parallax.radius ;
			j["ShaderData"]["Parallax"]["relativeFogRadius"] = currentData->shaderData.parallax.relativeFogRadius ;
			j["ShaderData"]["Parallax"]["scopeSwayAmount"] = currentData->shaderData.parallax.scopeSwayAmount ;
			j["ShaderData"]["Parallax"]["maxTravel"] = currentData->shaderData.parallax.maxTravel ;

			std::ofstream o(temp);
			o <<  j << std::endl;

			f.close();
			o.close();
		}
	}

	void FTSData::ReloadFTSData()
	{
		std::ifstream f(path);
		json j = json::parse(f);
		legacyMode = j["LegacyMode"].is_null() ? true : (bool)j["LegacyMode"];
		version = j["Version"].is_null() ? (int)currentFTSDataVerion : (int)j["Version"];
		UsingSTS = j["UsingSTS"].is_null() ? false : (bool)j["UsingSTS"];
		ZoomNodePath = j["ReticleTexturePath"].is_null() ? "Data/Textures/FTS/Default.dds" : j["ReticleTexturePath"];
		//UnalignedMode = j["UnalignedMode"].is_null() ? true : (bool)j["UnalignedMode"];
		scopeFrame = j["scopeFrame"].is_null() ? 1 : (unsigned int)j["scopeFrame"];

		shaderData.IsCircle = j["ShaderData"]["IsCircle"].is_null() ? true : (bool)j["ShaderData"]["IsCircle"];
		shaderData.bEnableZMove = j["ShaderData"]["EnableZMove"].is_null() ? true : (bool)j["ShaderData"]["EnableZMove"];
		shaderData.bCanEnableNV = j["ShaderData"]["EnableNV"].is_null() ? true : (bool)j["ShaderData"]["EnableNV"];
		shaderData.bBoltDisable = j["ShaderData"]["bBoltDisable"].is_null() ? true : (bool)j["ShaderData"]["bBoltDisable"];
		shaderData.fovAdjust = j["ShaderData"]["fovAdjust"].is_null() ? 0.0F : (float)j["ShaderData"]["fovAdjust"];

		shaderData.baseWeaponPos = j["ShaderData"]["BaseWeaponPos"].is_null() ? 0 : (float)j["ShaderData"]["BaseWeaponPos"];
		shaderData.movePercentage = j["ShaderData"]["ZMovePercentage"].is_null() ? 0 : (float)j["ShaderData"]["ZMovePercentage"];

		shaderData.camDepth = j["ShaderData"]["CamDepth"].is_null() ? 1 : (float)j["ShaderData"]["CamDepth"];
		shaderData.nvIntensity = j["ShaderData"]["NVIntensity"].is_null() ? 3 : (float)j["ShaderData"]["NVIntensity"];
		shaderData.ReticleSize = j["ShaderData"]["ReticleSize"].is_null() ? 4 : (float)j["ShaderData"]["ReticleSize"];

		shaderData.minZoom = j["ShaderData"]["MinZoom"].is_null() ? 1 : (float)j["ShaderData"]["MinZoom"];
		shaderData.maxZoom = j["ShaderData"]["MaxZoom"].is_null() ? 4 : (float)j["ShaderData"]["MaxZoom"];

		shaderData.Size[0] = j["ShaderData"]["Size"]["x"].is_null() ? 100.0F : (float)j["ShaderData"]["Size"]["x"];
		shaderData.Size[1] = j["ShaderData"]["Size"]["y"].is_null() ? 100.0F : (float)j["ShaderData"]["Size"]["y"];

		shaderData.OriSize[0] = j["ShaderData"]["OriSize"]["x"].is_null() ? 100.0F : (float)j["ShaderData"]["OriSize"]["x"];
		shaderData.OriSize[1] = j["ShaderData"]["OriSize"]["y"].is_null() ? 100.0F : (float)j["ShaderData"]["OriSize"]["y"];

		shaderData.PositionOffset[0] = j["ShaderData"]["PositionOffset"]["x"].is_null() ? 0 : (float)j["ShaderData"]["PositionOffset"]["x"];
		shaderData.PositionOffset[1] = j["ShaderData"]["PositionOffset"]["y"].is_null() ? 0 : (float)j["ShaderData"]["PositionOffset"]["y"];

		shaderData.OriPositionOffset[0] = j["ShaderData"]["OriPositionOffset"]["x"].is_null() ? 0 : (float)j["ShaderData"]["OriPositionOffset"]["x"];
		shaderData.OriPositionOffset[1] = j["ShaderData"]["OriPositionOffset"]["y"].is_null() ? 0 : (float)j["ShaderData"]["OriPositionOffset"]["y"];

		shaderData.parallax.radius = j["ShaderData"]["Parallax"]["radius"].is_null() ? 2 : (float)j["ShaderData"]["Parallax"]["radius"];
		shaderData.parallax.relativeFogRadius = j["ShaderData"]["Parallax"]["relativeFogRadius"].is_null() ? 9 : (float)j["ShaderData"]["Parallax"]["relativeFogRadius"];
		shaderData.parallax.scopeSwayAmount = j["ShaderData"]["Parallax"]["scopeSwayAmount"].is_null() ? 3 : (float)j["ShaderData"]["Parallax"]["scopeSwayAmount"];
		shaderData.parallax.maxTravel = j["ShaderData"]["Parallax"]["maxTravel"].is_null() ? 0.75F : (float)j["ShaderData"]["Parallax"]["maxTravel"];

		ScopeData::ScopeDataHandler::GetSingleton()->SetCurrentFTSData(this);

		f.close();

		ScopeData::ScopeDataHandler::GetSingleton()->UpdateFTSData(path);
	}

	void ScopeDataHandler::ReloadCurrentFTSData()
	{

	}

	void ScopeDataHandler::SetCurrentFTSData(FTSData* data)
	{
		currentData = data;
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
		json data = json::parse(f);
		if (data.empty())
			return false;

		FTSData* tempdata = new FTSData(data, path);
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
		
		if (isUpscaler)
			bEnableRenderBeforeUI = false;
		return &bEnableRenderBeforeUI;
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
