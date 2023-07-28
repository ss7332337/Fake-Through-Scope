#pragma once

#include "imgui.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <misc/cpp/imgui_stdlib.h>
#include "FTSData.h"


const char* const mainKey[] = {
	"None",
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

namespace ImGuiImpl
{

	class ImGuiImplClass
	{
	public:
		ImGuiImplClass();
		~ImGuiImplClass(){};

	public:
		
		bool bCanRender;


		ScopeData::ZoomDataOverwrite Imgui_ZDO;
		ScopeData::ZoomDataOverwrite ori_ZDO;


		bool bLegacyMode;
		bool UsingSTS_UI;
		int scopeFrame_UI;
		float fovBase_UI;
		bool IsCircle_UI;
		float camDepth_UI;
		float ReticleSize_UI;
		float minZoom_UI;
		float maxZoom_UI;
		float PositionOffset_UI[2];
		float OriPositionOffset_UI[2];
		float Size_UI[2];
		float Size_rect_UI[4];
		float OriSize_UI[2];
		float radius_UI;
		float relativeFogRadius_UI;
		float scopeSwayAmount_UI;
		float maxTravel_UI;

		bool bEnableFG;
		bool bEnableZMove;
		bool bEnableNVGEffect;
		float nvIntensity_UI;
		float baseWeaponPos_UI;
		float MovePercentage_UI;

		int nvgComboKeyIndex = 0;
		int nvgMainKeyIndex = 0;
		int guiKeyIndex = 0;
		bool bDisableWhileBolt = false;

		void MapScopeShaderEffect();
		

	public:
		void RenderImgui();
		bool EnableImGuiRender(bool, bool);
		bool CheckAndInit();
		void ReloadData();
		void SaveData();
		void MainMenuSection();
		void ShaderDataSection();
		void ParallaxDataSection();
		void PlayerAim(bool);

		void UpdateWeaponInstance(RE::TESObjectWEAP::InstanceData*);
		void UpdateImGuiData();
	};

}

