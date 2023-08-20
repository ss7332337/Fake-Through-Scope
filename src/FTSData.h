#pragma once

#include <nlohmann/json.hpp>
#include <unordered_set>

#define currentFTSDataVerion 1

using json = nlohmann::json;
namespace ScopeData
{
	std::vector<std::string_view> splitSV(std::string_view strv, std::string_view delims = " ");

	struct CullingData
	{
		CullingData(int a, int b)
		{
			IndexCount = a;
			StrideCount = b;
		}
		int IndexCount;
		int StrideCount;
	};

	struct Parallax
	{
		float radius = 2;
		float relativeFogRadius = 9;
		float scopeSwayAmount = 3;
		float maxTravel = 1;
	};

	struct ZoomDataOverwrite
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float fovMul = 1;
		bool enableZoomDateOverwrite = false;
	};

	struct ShaderData
	{
		bool IsCircle = true;
		bool bEnableZMove = false;
		bool bCanEnableNV = false;
		bool bBoltDisable = false;

		float nvIntensity = 3;
		float baseWeaponPos = 0;
		float movePercentage = 0;

		float camDepth = 1.0F;

		float minZoom = 1.0F;
		float maxZoom = 4.0F;

		float PositionOffset[2] = {0.0F,0.0F};
		float OriPositionOffset[2] = { 0.0F, 0.0F };

		float Size[2] = { 200.0F, 0.0F };
		float OriSize[2] = { 200.0F, 0.0F };
		float rectSize[4] = { 235.0f, 200.0f, 775.0f, 760.0f };

		float ReticleSize = 4;
		float reticle_Offset[2] = { 0.0f, 0.0f };

		float fovAdjust = 0;
		Parallax parallax;
	};

	
	class FTSData
	{
	public:
		bool legacyMode = true;
		std::string path = "";
		int version = 1;
		std::string keywordName = "FTS_Default";
		std::string animFlavorEditorID = "FTS_NONE";
		std::string additionalKeywordsStr;
		std::vector<std::string> additionalKeywords = std::vector<std::string>();

		bool containAlladditionalKeywords = true;

		bool UsingSTS = false;
		unsigned int scopeFrame = 1;
		std::string ZoomNodePath;

		ShaderData shaderData;
		ZoomDataOverwrite zoomDataOverwrite;

		FTSData(std::string pathO);
		//FTSData(json j, std::string pathO);
		//void ReloadFTSData();
	};



	class ScopeDataHandler
	{
	public:


		static ScopeDataHandler* GetSingleton();
		
		void ReloadFTSData(FTSData*);
		void ReadDefaultScopeDataFile();
		void ReadCustomScopeDataFiles(std::string path);
		void TestingJson();

		void WriteCurrentFTSData();
		void ReloadCurrentFTSData();

		void SetCurrentFTSData(FTSData* data, bool containsAllAdditionkeyword = true);
		FTSData* GetCurrentFTSData();

		int GetEffectIndex();
		void SetEffectIndex(int);

		bool* GetEnableRenderThroughUI();
		void SaveEnableRenderBeforeUI(bool);

		int GetBaseRenderCount();
		void SetBaseRenderCount(int);

		void SetNVGHotKeyCombo(int);
		void SetNVGHotKeyMain(unsigned int keycode);
		void SetGuiKey(unsigned int keycode);

		void SetIsUpscaler(bool);
		void UpdateFTSData(std::string path);
		const char* GetNVGComboKeyStr();

		//bool ZoomDataWrite(RE::TESObjectWEAP::InstanceData* GetSington);
		void ReloadZoomData(std::string path);

		std::multimap<std::string, FTSData*>* GetScopeDataMap();

	private:
		
		bool ReadScopeData(std::string path);
		ScopeDataHandler() = default;
		ScopeDataHandler(const ScopeDataHandler&) = delete;
		ScopeDataHandler(ScopeDataHandler&&) = delete;
		~ScopeDataHandler() = default;

		ScopeDataHandler& operator=(const ScopeDataHandler&) = delete;
		ScopeDataHandler& operator=(ScopeDataHandler&&) = delete;

	public:
		bool bEnableRenderBeforeUI = false;
		int comboNVKey = -1;
		int nvKey = -1;
		int guiKey = -1;

	private:
		int baseRenderCount = 0;
		int PassRenderIndex = 1;
		
		bool isUpscaler = false;
		std::vector<std::string> files;
		//CSimpleIniA iniForZoom;
		std::multimap<std::string, FTSData*> ScopeDataMap;
		FTSData* currentData;
		std::string currentPath;
	};

}
