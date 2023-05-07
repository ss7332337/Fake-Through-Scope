//#include "ReshadeImpl.h"
//#include "stb_image.h"
//#include "DDSTextureLoader11.h"
//#include "WICTextureLoader11.h"
////#include "DDSTextureLoader.h"
////#include <d3d12.h>
////#include <d3d11.h>
////#include <d3d11on12.h>
//#include <fstream>
//#include <istream>
//#include <DirectXMath.h>
//
//#include <stb_image_write.h>
//#include <DirectXMath.h>
//#include "FTSData.h"
//#include "REL/Relocation.h"
//#include <wrl/client.h>
//
//
//template <class T>
//using ComPtr = Microsoft::WRL::ComPtr<T>;
//
//extern inline void InitCurrentScopeData(RE::BGSKeyword* animFlavorKeyword);
//
//void InitializePlugin();
//
////typedef void (*_WorldToScreen)(RE::NiPoint3* in, RE::NiPoint3* out);
////extern REL::Relocation<_WorldToScreen> WorldToScreen_Internal;
//
//#include <iostream>
//#include <windows.h>
//#include <winnls.h>
//
//
//
//void SafeWriteBuf(uintptr_t addr, void* data, size_t len)
//{
//	UInt32 oldProtect;
//
//	VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
//	memcpy((void*)addr, data, len);
//	VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
//}
//
//void SafeWrite64(uintptr_t addr, UInt64 data)
//{
//	SafeWriteBuf(addr, &data, sizeof(data));
//}
//
//void* GetIATAddr(void* module, const char* searchDllName, const char* searchImportName)
//{
//	UInt8* base = (UInt8*)module;
//	IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)base;
//	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
//	IMAGE_IMPORT_DESCRIPTOR* importTable =
//		(IMAGE_IMPORT_DESCRIPTOR*)(base + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
//
//	for (; importTable->Characteristics; ++importTable) {
//		const char* dllName = (const char*)(base + importTable->Name);
//
//		if (!_stricmp(dllName, searchDllName)) {
//			// found the dll
//
//			IMAGE_THUNK_DATA* thunkData = (IMAGE_THUNK_DATA*)(base + importTable->OriginalFirstThunk);
//			uintptr_t* iat = (uintptr_t*)(base + importTable->FirstThunk);
//
//			for (; thunkData->u1.Ordinal; ++thunkData, ++iat) {
//				if (!IMAGE_SNAP_BY_ORDINAL(thunkData->u1.Ordinal)) {
//					IMAGE_IMPORT_BY_NAME* importInfo = (IMAGE_IMPORT_BY_NAME*)(base + thunkData->u1.AddressOfData);
//
//					if (!_stricmp((char*)importInfo->Name, searchImportName)) {
//						// found the import
//						return iat;
//					}
//				}
//			}
//
//			return NULL;
//		}
//	}
//
//	return NULL;
//}
//
//template <typename Func>
//inline void HookImportFunc(const char* dll, const char* nameFunc, Func& oldFn, std::uintptr_t hookFn)
//{
//	std::uintptr_t thunkAddress = (std::uintptr_t)GetIATAddr((UINT8*)GetModuleHandle(NULL), dll, nameFunc);
//	oldFn = (Func) * (std::uintptr_t*)thunkAddress;
//	SafeWrite64(thunkAddress, hookFn);
//}
//
//inline std::uintptr_t HookFunc(std::uintptr_t* vtbl, int index, std::uintptr_t hookFn, std::uintptr_t* oldFn)
//{
//	std::uintptr_t returnAddy = vtbl[index];
//	*oldFn = returnAddy;
//
//	SafeWrite64((std::uintptr_t)(vtbl + index), hookFn);
//
//	return returnAddy;
//}
//
//
//template <class T>
//void write_thunk_call(std::uintptr_t a_src)
//{
//	F4SE::AllocTrampoline(14);
//
//	auto& trampoline = F4SE::GetTrampoline();
//	T::func = trampoline.write_call<5>(a_src, T::thunk);
//}
//
//template <class T>
//void write_thunk_call6(std::uintptr_t a_src)
//{
//	F4SE::AllocTrampoline(14);
//
//	auto& trampoline = F4SE::GetTrampoline();
//	T::func = trampoline.write_call<6>(a_src, T::thunk);
//}
//
//template <class F, std::size_t idx, class T>
//void write_vfunc()
//{
//	REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[0] };
//	T::func = vtbl.write_vfunc(idx, T::thunk);
//}
//
//template <std::uintptr_t address, std::size_t idx, class T>
//void write_vfunc()
//{
//	REL::Relocation<std::uintptr_t> vtbl{ address };
//	T::func = vtbl.write_vfunc(idx, T::thunk);
//}
//
//template <std::size_t idx, class T>
//void write_vfunc(std::uintptr_t id)
//{
//	REL::Relocation<std::uintptr_t> vtbl{ id };
//	T::func = vtbl.write_vfunc(idx, T::thunk);
//}
//
//using namespace RE;
//
//namespace ReshadeImpl
//{
//
//	bool firstTime = true;
//	bool bEnableEditMode = false;
//	bool bChangeAimTexture = true;
//	bool isReloading = false;
//	bool isSetShaderData = false;
//	bool isActive_TAA = true;
//
//	int countnum = -1;
//	int countStride = -1;
//	uint32_t countIndexCount = 1;
//	int countpscdescByteWidth = -1;
//	int countindescByteWidth = -1;
//	int countvedescByteWidth = -1;
//	float scopeTimer = 0;
//
//	RE::NiNode* scopeNormalNode_i;
//	RE::NiNode* scopeAimingNode_i;
//	RE::NiNode* scopeNormalNode3rd_i;
//	RE::NiNode* scopeAimingNode3rd_i;
//
//	RE::NiAVObject* weaponNode;
//	RE::NiAVObject* rootNode;
//	RE::NiAVObject* camNode;
//	REL::Relocation<float*> ptr_deltaTime{ REL::ID(1013228) };
//
//	effect_technique et;
//	effect_technique eto;
//	effect_technique etop;
//	device* reshadeDevice = nullptr;
//	command_list* reshadeCmdlist = nullptr;
//
//	ScopeData::FTSData* currData = nullptr;
//#pragma region UIVar
//	bool UsingSTS_UI;
//	bool bDisableWhileBolt;
//	int scopeFrame_UI;
//	bool IsCircle_UI;
//	float camDepth_UI;
//	float ReticleSize_UI;
//	float minZoom_UI;
//	float maxZoom_UI;
//	float PositionOffset_UI[2];
//	float OriPositionOffset_UI[2];
//	float Size_UI[2];
//	float OriSize_UI[2];
//	float radius_UI;
//	float relativeFogRadius_UI;
//	float scopeSwayAmount_UI;
//	float maxTravel_UI;
//	float MovePercentage_UI;
//	float baseWeaponPos_UI;
//	float currWeaponPos_UI;
//	float nvIntensity_UI;
//	bool bEnableNVGEffect;
//
//	bool bRenderBeforeUI = true;
//	bool bReloadRenderMode = false;
//	bool bEnableZMove = false;
//
//	const char* const mainKey[] = {
//		"None",
//		"Unknown",
//		"VK_LBUTTON",
//		"VK_RBUTTON",
//		"VK_CANCEL",
//		"VK_MBUTTON",
//		"VK_XBUTTON1",
//		"VK_XBUTTON2",
//		"Unknown",
//		"VK_BACK",
//		"VK_TAB",
//		"Unknown",
//		"Unknown",
//		"VK_CLEAR",
//		"VK_RETURN",
//		"Unknown",
//		"Unknown",
//		"VK_SHIFT",
//		"VK_CONTROL",
//		"VK_MENU",
//		"VK_PAUSE",
//		"VK_CAPITAL",
//		"VK_KANA",
//		"Unknown",
//		"VK_JUNJA",
//		"VK_FINAL",
//		"VK_KANJI",
//		"Unknown",
//		"VK_ESCAPE",
//		"VK_CONVERT",
//		"VK_NONCONVERT",
//		"VK_ACCEPT",
//		"VK_MODECHANGE",
//		"VK_SPACE",
//		"VK_PRIOR",
//		"VK_NEXT",
//		"VK_END",
//		"VK_HOME",
//		"VK_LEFT",
//		"VK_UP",
//		"VK_RIGHT",
//		"VK_DOWN",
//		"VK_SELECT",
//		"VK_PRINT",
//		"VK_EXECUTE",
//		"VK_SNAPSHOT",
//		"VK_INSERT",
//		"VK_DELETE",
//		"VK_HELP",
//		"0",
//		"1",
//		"2",
//		"3",
//		"4",
//		"5",
//		"6",
//		"7",
//		"8",
//		"9",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"A",
//		"B",
//		"C",
//		"D",
//		"E",
//		"F",
//		"G",
//		"H",
//		"I",
//		"J",
//		"K",
//		"L",
//		"M",
//		"N",
//		"O",
//		"P",
//		"Q",
//		"R",
//		"S",
//		"T",
//		"U",
//		"V",
//		"W",
//		"X",
//		"Y",
//		"Z",
//		"VK_LWIN",
//		"VK_RWIN",
//		"VK_APPS",
//		"Unknown",
//		"VK_SLEEP",
//		"VK_NUMPAD0",
//		"VK_NUMPAD1",
//		"VK_NUMPAD2",
//		"VK_NUMPAD3",
//		"VK_NUMPAD4",
//		"VK_NUMPAD5",
//		"VK_NUMPAD6",
//		"VK_NUMPAD7",
//		"VK_NUMPAD8",
//		"VK_NUMPAD9",
//		"VK_MULTIPLY",
//		"VK_ADD",
//		"VK_SEPARATOR",
//		"VK_SUBTRACT",
//		"VK_DECIMAL",
//		"VK_DIVIDE",
//		"VK_F1",
//		"VK_F2",
//		"VK_F3",
//		"VK_F4",
//		"VK_F5",
//		"VK_F6",
//		"VK_F7",
//		"VK_F8",
//		"VK_F9",
//		"VK_F10",
//		"VK_F11",
//		"VK_F12",
//		"VK_F13",
//		"VK_F14",
//		"VK_F15",
//		"VK_F16",
//		"VK_F17",
//		"VK_F18",
//		"VK_F19",
//		"VK_F20",
//		"VK_F21",
//		"VK_F22",
//		"VK_F23",
//		"VK_F24",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"VK_NUMLOCK",
//		"VK_SCROLL",
//		"VK_OEM_NEC_EQUAL",
//		"VK_OEM_FJ_MASSHOU",
//		"VK_OEM_FJ_TOUROKU",
//		"VK_OEM_FJ_LOYA",
//		"VK_OEM_FJ_ROYA",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"Unknown",
//		"VK_LSHIFT",
//		"VK_RSHIFT",
//		"VK_LCONTROL",
//		"VK_RCONTROL",
//		"VK_LMENU",
//		"VK_RMENU"
//	};
//
//	static const char* current_item = NULL;
//	static const char* current_itemA = NULL;
//
//	bool isInitNVGComboKey = false;
//	bool isInitNVGMainKey = false;
//
//	int nvgComboKeyIndex = 0;
//	int nvgMainKeyIndex = 0;
//#pragma endregion
//	
//
//	ComPtr<ID3D11Device> mDevice = nullptr;
//	ComPtr<IDXGISwapChain> mSwapchain = nullptr;
//	ComPtr<ID3D11DeviceContext> mContext = nullptr;
//	ComPtr<ID3D11ShaderResourceView> mTextDDS_SRV = nullptr;
//	ComPtr<ID3D11Resource> mTextDDS = nullptr;
//	ComPtr<ID3D11Query> mQuery = nullptr;
//	ComPtr<ID3D11ShaderResourceView> mReplaceDDS_SRV = nullptr;
//	ComPtr<ID3D11Resource> mReplaceDDS = nullptr;
//	//ComPtr<ID3D12Device> mDevice12 = nullptr;
//	//ComPtr<ID3D12Shade> mTextDDS12_SRV = nullptr;
//	//ComPtr<ID3D12Resource> mTextDDS12 = nullptr;
//
//	using namespace ScopeData;
//	using namespace std;
//	using namespace reshade::api;
//	using namespace DirectX;
//
//	struct __declspec(uuid("7251932A-ADAF-4DFC-B5CB-9A4E8CD5D6EB")) device_data
//	{
//		effect_runtime* main_runtime = nullptr;
//		uint32_t offset_from_last_pass = 0;
//		uint32_t last_render_pass_count = 1;
//		uint32_t current_render_pass_count = 0;
//
//		effect_technique ScopeEffectTech;
//	};
//	struct __declspec(uuid("036CD16B-E823-4D6C-A137-5C335D6FD3E6")) command_list_data
//	{
//		bool has_multiple_rtvs = false;
//		resource_view current_main_rtv = { 0 };
//		uint32_t current_render_pass_index = 0;
//
//	};
//
//	
//
//#pragma region euv
//
//	effect_uniform_variable euv_EyeDir;
//	effect_uniform_variable euv_EyeDirLerp;
//	effect_uniform_variable euv_EyeTranslationLerp;
//	effect_uniform_variable euv_CameraRotation;
//	effect_uniform_variable euv_CameraRotation1;
//	effect_uniform_variable euv_CameraRotation2;
//
//	effect_uniform_variable euv_Size;
//	effect_uniform_variable euv_DeltaZoom;
//	effect_uniform_variable euv_IsCircle;
//
//	effect_uniform_variable euv_AimNodeSize;
//
//	effect_uniform_variable euv_pRadius;
//	effect_uniform_variable euv_pRelativeFogRadius;
//	effect_uniform_variable euv_pScopeSwayAmount;
//	effect_uniform_variable euv_pMaxTravel;
//
//	effect_uniform_variable euv_effectIndex;
//
//	effect_uniform_variable euv_Offset;
//	effect_uniform_variable euv_OriOffset;
//	effect_uniform_variable euv_CamDepth;
//	effect_uniform_variable euv_GameFov;
//
//	effect_uniform_variable euv_EnableMerge;
//	//OriSize
//	effect_uniform_variable euv_OriSize;
//
//	//NVG
//	effect_uniform_variable euv_EnableNightVision;
//	effect_uniform_variable euv_NVIntensity;
//
//	//ZMOVE
//	effect_uniform_variable euv_BaseWeaponPos;
//	effect_uniform_variable euv_CurrWeaponPos;
//	effect_uniform_variable euv_CurrRootPos;
//	effect_uniform_variable euv_MovePercentage;
//	effect_uniform_variable euv_EnableZMove;
//
//	
//#pragma endregion
//	
//	bool Impl::GetFirstTime()
//	{
//		return firstTime;
//	}
//	bool Impl::IsEditorMode()
//	{
//		return bEnableEditMode;
//	}
//	void InitDDSRes()
//	{
//		HRESULT hr = CreateDDSTextureFromFile(mDevice.Get(), L"Data/Textures/FTS/Empty.dds", mTextDDS.GetAddressOf(), mTextDDS_SRV.GetAddressOf());
//
//	}
//	void Impl::on_init_device(device* device)
//	{
//		device->create_private_data<device_data>();
//		
//		reshadeDevice = device;
//
//		
//		/*D3D11_QUERY_DESC qdesc;
//		qdesc.MiscFlags = 
//		mDevice->CreateQuery()*/
//
//		
//	}
//	void Impl::on_destroy_device(device* device)
//	{
//		device->destroy_private_data<device_data>();
//	}
//	void Impl::on_init_command_list(command_list* cmd_list)
//	{
//		cmd_list->create_private_data<command_list_data>();
//		reshadeCmdlist = cmd_list;
//		
//		//rtt->Initialize((ID3D11Device*)cmd_list->get_device()->get_native(), 300, 300);
//	}
//	void Impl::on_destroy_command_list(command_list* cmd_list)
//	{
//		cmd_list->destroy_private_data<command_list_data>();
//		reshadeCmdlist = nullptr;
//	}
//	void Impl::on_init_effect_runtime(effect_runtime* runtime)
//	{
//		auto& dev_data = runtime->get_device()->get_private_data<device_data>();
//		// Assume last created effect runtime is the main one
//		dev_data.main_runtime = runtime;
//	}
//	void Impl::on_destroy_effect_runtime(effect_runtime* runtime)
//	{
//		auto& dev_data = runtime->get_device()->get_private_data<device_data>();
//		if (runtime == dev_data.main_runtime)
//			dev_data.main_runtime = nullptr;
//	}
//
//	void Impl::InitPlayerAndCam(RE::PlayerCharacter* p, RE::PlayerCamera* pc)
//	{
//		instance().player = p;
//		instance().pcam = pc;
//	}
//
//
//	void Impl::on_begin_render_pass(command_list* cmd_list, uint32_t count, const render_pass_render_target_desc* rts, const render_pass_depth_stencil_desc*)
//	{
//		auto& data = cmd_list->get_private_data<command_list_data>();
//		data.has_multiple_rtvs = count > 1;
//		data.current_main_rtv = (count != 0) ? rts[0].view : resource_view{ 0 };
//	}
//
//	void Impl::on_bind_render_targets_and_depth_stencil(command_list* cmd_list, uint32_t count, const resource_view* rtvs, resource_view)
//	{
//		auto& data = cmd_list->get_private_data<command_list_data>();
//		const resource_view new_main_rtv = (count != 0) ? rtvs[0] : resource_view{ 0 };
//		data.has_multiple_rtvs = count > 1;
//		data.current_main_rtv = new_main_rtv;
//	}
//
//	bool IsNeedToBeCull(int indexCount = 0, int StrideCount = 0)
//	{
//		if (!ScopeData::ScopeDataHandler::GetSingleton())
//			return false;
//		if (!ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData())
//			return false;
//		if (ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->UsingSTS)
//			return false;
//
//		return true;
//	}
//
//	void Impl::on_d3dPresent(command_queue* queue, swapchain* swapchain, const rect* source_rect, const rect* dest_rect, uint32_t dirty_rect_count, const rect* dirty_rects)
//	{
//	}
//
//	RE::NiPoint3 crossProduct(const RE::NiPoint3& a, const RE::NiPoint3& b)
//	{
//		return RE::NiPoint3(a.y * b.z - a.z * b.y, -(a.x * b.z - a.z * b.x), a.x * b.y - a.y * b.x);
//	}
//
//
//	void rotate_vector_by_quaternion(const RE::NiPoint3& v, const RE::NiQuaternion& q, RE::NiPoint3& vprime)
//	{
//		RE::NiPoint3 u(q.x, q.y, q.z);
//		float s = q.w;
//		vprime = u * ((u * v) * 2.0f) + v * (s * s - (u * u)) + crossProduct(u, v) * 2.0f * s;
//	}
//
//	
//
//	void Impl::SetShaderData()
//	{
//		isSetShaderData = true;
//		
//	}
//	void Impl::SetShaderData(effect_runtime* runtime)
//	{
//
//		if (isSetShaderData) 
//		{
//			if (!runtime)
//				return;
//
//			device* const device = runtime->get_device();
//			auto& dev_data = device->get_private_data<device_data>();
//
//			//auto& data =  cmd_list->get_private_data<command_list_data>();
//
//			currData = ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
//
//			if (!currData)
//				return;
//
//			float sumZoom = currData->shaderData.minZoom;
//			{
//				et = runtime->find_technique("ScopeEffect.fx", "ScopeEffect");
//				dev_data.ScopeEffectTech = et;
//				runtime->set_technique_state(et, false);
//#pragma region FindUniformVar
//				//effect_uniform_variable testingVar = runtime->find_uniform_variable("ScopeEffect.fx", "testing");
//				euv_EyeDir = runtime->find_uniform_variable("ScopeEffect.fx", "eyeDirection");
//				euv_EyeDirLerp = runtime->find_uniform_variable("ScopeEffect.fx", "eyeDirectionLerp");
//				euv_EyeTranslationLerp = runtime->find_uniform_variable("ScopeEffect.fx", "eyeTranslationLerp");
//				euv_CameraRotation = runtime->find_uniform_variable("ScopeEffect.fx", "CameraRotation");
//
//				euv_Size = runtime->find_uniform_variable("ScopeEffect.fx", "ScopeEffect_Size");
//				euv_OriSize = runtime->find_uniform_variable("ScopeEffect.fx", "ScopeEffect_OriSize");
//
//				euv_DeltaZoom = runtime->find_uniform_variable("ScopeEffect.fx", "ScopeEffect_Zoom");
//				euv_IsCircle = runtime->find_uniform_variable("ScopeEffect.fx", "isCircle");
//				euv_CamDepth = runtime->find_uniform_variable("ScopeEffect.fx", "camDepth");
//				euv_GameFov = runtime->find_uniform_variable("ScopeEffect.fx", "GameFov");
//
//				euv_AimNodeSize = runtime->find_uniform_variable("ScopeEffect.fx", "ReticleSize");
//
//				euv_pRadius = runtime->find_uniform_variable("ScopeEffect.fx", "parallax_Radius");
//				euv_pRelativeFogRadius = runtime->find_uniform_variable("ScopeEffect.fx", "parallax_relativeFogRadius");
//				euv_pScopeSwayAmount = runtime->find_uniform_variable("ScopeEffect.fx", "parallax_scopeSwayAmount");
//				euv_pMaxTravel = runtime->find_uniform_variable("ScopeEffect.fx", "parallax_maxTravel");
//
//				//euv_effectIndex = runtime->find_uniform_variable("ScopeEffect.fx", "effectIndex");
//
//				euv_Offset = runtime->find_uniform_variable("ScopeEffect.fx", "ScopeEffect_Offset");
//				euv_OriOffset = runtime->find_uniform_variable("ScopeEffect.fx", "ScopeEffect_OriPositionOffset");
//				euv_EnableMerge = runtime->find_uniform_variable("ScopeEffect.fx", "EnableMerge");
//
//				euv_EnableNightVision = runtime->find_uniform_variable("ScopeEffect.fx", "EnableNV");
//				euv_NVIntensity = runtime->find_uniform_variable("ScopeEffect.fx", "nvIntensity");
//
//				euv_BaseWeaponPos = runtime->find_uniform_variable("ScopeEffect.fx", "BaseWeaponPos");
//				euv_CurrWeaponPos = runtime->find_uniform_variable("ScopeEffect.fx", "CurrWeaponPos");
//				euv_CurrRootPos = runtime->find_uniform_variable("ScopeEffect.fx", "CurrRootPos");
//				euv_MovePercentage = runtime->find_uniform_variable("ScopeEffect.fx", "MovePercentage");
//				euv_EnableZMove = runtime->find_uniform_variable("ScopeEffect.fx", "EnableZMove");
//;
//#pragma endregion
//				firstTime = false;
//			}
//			
//
//
//			if (runtime && currData) {
//				float fAc_zoom;
//				runtime->get_uniform_value_float(euv_DeltaZoom, &fAc_zoom, 1);
//				runtime->get_mouse_cursor_position(nullptr, nullptr, &instance().wheelDelta);
//				sumZoom = clamp(fAc_zoom + instance().wheelDelta * 0.1F, currData->shaderData.minZoom, currData->shaderData.maxZoom);
//			}
//
//			if (instance().player) {
//				if (instance().player->Get3D(false)) {
//					scopeNormalNode3rd_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->Get3D(false)->GetObjectByName("ScopeNormal");
//					scopeAimingNode3rd_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->Get3D(false)->GetObjectByName("ScopeAiming");
//
//					if (scopeNormalNode3rd_i && scopeAimingNode3rd_i) {
//						if (RE::PlayerCharacter::GetSingleton()->IsInThirdPerson()) {
//							scopeNormalNode3rd_i->SetAppCulled(false);
//							scopeAimingNode3rd_i->SetAppCulled(true);
//							return;
//						}
//					}
//				}
//
//				if (instance().player->firstPerson3D) {
//					scopeNormalNode_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->firstPerson3D->GetObjectByName("ScopeNormal");
//					scopeAimingNode_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->firstPerson3D->GetObjectByName("ScopeAiming");
//
//					if (scopeNormalNode_i && scopeAimingNode_i) {
//						if (instance().ActiveAreaCopy) {
//							if (IsNeedToBeCull()) {
//								scopeNormalNode_i->SetAppCulled(true);
//								scopeAimingNode_i->SetAppCulled(false);
//
//							} else {
//								scopeNormalNode_i->SetAppCulled(false);
//								//setbit(scopeAimingNode_i->flags.flags, 1);
//								scopeAimingNode_i->SetAppCulled(true);
//							}
//							//runtime->set_technique_state(et, true);
//						} else {
//							scopeNormalNode_i->SetAppCulled(false);
//							scopeAimingNode_i->SetAppCulled(true);
//							//runtime->set_technique_state(et, false);
//						}
//					}
//				}
//			}
//
//			if (instance().player && instance().ActiveAreaCopy) {
//				if (instance().player->firstPerson3D) {
//					{
//						RE::NiPoint3 virDirA = instance().virDir;
//
//						float temp3[3] = { virDirA.x, virDirA.y, virDirA.z };
//						runtime->set_uniform_value_float(euv_EyeDir, temp3, 3);
//
//						RE::NiPoint3 VirDirLerpA = instance().VirDirLerp;
//						float tempA[3] = { VirDirLerpA.x, VirDirLerpA.y, VirDirLerpA.z };
//						runtime->set_uniform_value_float(euv_EyeDirLerp, tempA, 3);
//
//						RE::NiPoint3 VirTransA = instance().VirTransLerp;
//						float tempB[3] = { VirTransA.x, VirTransA.y, VirTransA.z };
//						runtime->set_uniform_value_float(euv_EyeTranslationLerp, tempB, 3);
//
//						float tempC[3] = { instance().weaponPos.x, instance().weaponPos.y, instance().weaponPos.z };
//						float tempD[3] = { instance().rootPos.x, instance().rootPos.y, instance().rootPos.z };
//						runtime->set_uniform_value_float(euv_CurrWeaponPos, tempC, 3);
//						runtime->set_uniform_value_float(euv_CurrRootPos, tempD, 3);
//
//						float tempCamRotaR[9] = {
//							instance().camMat.entry[0].pt[0], instance().camMat.entry[0].pt[1], instance().camMat.entry[0].pt[2],
//							instance().camMat.entry[1].pt[0], instance().camMat.entry[1].pt[1], instance().camMat.entry[1].pt[2],
//							instance().camMat.entry[2].pt[0], instance().camMat.entry[2].pt[1], instance().camMat.entry[2].pt[2]
//						};
//
//						runtime->set_uniform_value_float(euv_CameraRotation, tempCamRotaR, 9);
//
//
//						if (currData) {
//							if (currData->shaderData.bCanEnableNV)
//								runtime->set_uniform_value_bool(euv_EnableNightVision, instance().bEnableNVG, 1);
//
//							if (!bEnableEditMode) {
//								sumZoom = clamp(sumZoom, currData->shaderData.minZoom, currData->shaderData.maxZoom);
//
//								runtime->set_uniform_value_float(euv_DeltaZoom, sumZoom, 1);
//								runtime->set_uniform_value_float(euv_Offset, currData->shaderData.PositionOffset, 2);
//								runtime->set_uniform_value_float(euv_OriOffset, currData->shaderData.OriPositionOffset, 2);
//								runtime->set_uniform_value_float(euv_OriSize, currData->shaderData.OriSize, 2);
//								runtime->set_uniform_value_float(euv_BaseWeaponPos, currData->shaderData.baseWeaponPos, 1);
//								runtime->set_uniform_value_float(euv_MovePercentage, currData->shaderData.movePercentage, 1);
//								runtime->set_uniform_value_bool(euv_EnableZMove, currData->shaderData.bEnableZMove, 1);
//
//								runtime->set_uniform_value_float(euv_GameFov, instance().pcam->firstPersonFOV, 2);
//
//								runtime->set_uniform_value_float(euv_NVIntensity, currData->shaderData.nvIntensity, 1);
//
//								float notfinishedSize[2]{ 0, 0 };
//
//								if (instance().bFinishAimAnim)
//									runtime->set_uniform_value_float(euv_Size, currData->shaderData.Size, 2);
//								else
//									runtime->set_uniform_value_float(euv_Size, notfinishedSize, 2);
//
//								runtime->set_uniform_value_float(euv_AimNodeSize, currData->shaderData.ReticleSize, 1);
//								runtime->set_uniform_value_float(euv_pRadius, currData->shaderData.parallax.radius, 1);
//								runtime->set_uniform_value_float(euv_pRelativeFogRadius, currData->shaderData.parallax.relativeFogRadius, 1);
//								runtime->set_uniform_value_float(euv_pScopeSwayAmount, currData->shaderData.parallax.scopeSwayAmount, 1);
//								runtime->set_uniform_value_float(euv_pMaxTravel, currData->shaderData.parallax.maxTravel, 1);
//								runtime->set_uniform_value_bool(euv_IsCircle, currData->shaderData.IsCircle);
//								runtime->set_uniform_value_float(euv_CamDepth, currData->shaderData.camDepth);
//								runtime->get_uniform_value_uint(euv_effectIndex, &dev_data.offset_from_last_pass, 1);
//							}
//						}
//					}
//				}
//			}
//
//			
//			if (!(*ScopeData::ScopeDataHandler::GetSingleton()->GetEnableRenderThroughUI())) {	
//				if (instance().ActiveAreaCopy) {
//					runtime->set_technique_state(et, true);
//					runtime->set_uniform_value_bool(euv_EnableMerge, true);
//				} else if (!instance().ActiveAreaCopy) {
//					runtime->set_technique_state(et, false);
//					if (currData)
//						runtime->set_uniform_value_float(euv_DeltaZoom, currData->shaderData.minZoom, 1);
//					runtime->set_uniform_value_bool(euv_EnableMerge, false);
//				}
//				isSetShaderData = false;
//			}
//			
//			
//			/*if (bRenderBeforeUI) {
//		
//			} 
//			else {
//				if (instance().ActiveAreaCopy) {
//					runtime->set_technique_state(etop, true);
//				} else if (!instance().ActiveAreaCopy) {
//					if (currData)
//						runtime->set_uniform_value_float(euv_DeltaZoom, currData->shaderData.minZoom, 1);
//					runtime->set_technique_state(etop, false);
//				}
//				runtime->set_technique_state(et, false);
//				runtime->set_technique_state(eto, false);
//			}*/
//		}
//
//		
//	}
//
//	void Impl::on_present(effect_runtime* runtime)
//	{
//
//		device* const device = runtime->get_device();
//		auto& dev_data = device->get_private_data<device_data>();
//
//		if (runtime != dev_data.main_runtime)
//			dev_data.main_runtime = runtime;
//
//
//		//isReloading = true;
//
//		if (instance().bStartScope) {
//			scopeTimer += *ptr_deltaTime *1000;
//			if (!instance().bStartScope) {
//				scopeTimer = 0;
//				return;
//			}
//			if (scopeTimer > ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->scopeFrame) {
//
//				instance().ActiveAreaCopy = true;
//
//				scopeTimer = 0;
//				instance().bStartScope = false;
//			}
//		} else {
//			scopeTimer = 0;
//		}
//		
//		
//	}
//
//
//	void Impl::on_draw_settings_Self(effect_runtime* runtime)
//	{
//			device* const device = runtime->get_device();
//			auto& dev_data = device->get_private_data<device_data>();
//
//			ScopeDataHandler* sdh = ScopeDataHandler::GetSingleton();
//
//
//			if (!ScopeDataHandler::GetSingleton()->GetCurrentFTSData()) {
//				ImGui::TextUnformatted("This is not FTS Supported Weapon, Make a Patch or Double Check your Patch!");
//				ImGui::NewLine();
//				return;
//			}
//
//			if (nvgComboKeyIndex != -1 && nvgMainKeyIndex != -1 && ImGui ::TreeNode("Key Binding")) {
//				nvgComboKeyIndex = sdh->comboNVKey + 1;
//				nvgMainKeyIndex = sdh->nvKey + 1;
//
//				if (ImGui::Combo("NVG Combo Key", &nvgComboKeyIndex, mainKey, 166)) {
//					sdh->SetNVGHotKeyCombo(nvgComboKeyIndex - 1);
//				}
//
//				if (ImGui::Combo("NVG Main Key", &nvgMainKeyIndex, mainKey, 166)) {
//					sdh->SetNVGHotKeyMain(nvgMainKeyIndex - 1);
//				}
//
//				ImGui::TreePop();
//				ImGui::NewLine();
//			}
//
//			if (!bIsUpscaler) 
//			{
//				ImGui::Text("Enable Render Before UI Require Reload Effect!");
//				if (ImGui::Checkbox("Enable Render Before UI", ScopeData::ScopeDataHandler::GetSingleton()->GetEnableRenderThroughUI())) 
//				{
//					ScopeData::ScopeDataHandler::GetSingleton()->SaveEnableRenderBeforeUI(0);
//					if (reshadeCmdlist) {
//						auto player = PlayerCharacter::GetSingleton();
//						player->SetInIronSightsImpl(false);
//						//ended
//						RE::TESIdleForm* sightedForm = (TESIdleForm*)TESForm::GetFormByID(0x4AD9);
//						player->currentProcess->PlayIdle(player, 0x35, sightedForm);
//
//						auto cmdlist_data = reshadeCmdlist->get_private_data<command_list_data>();
//
//						runtime->render_technique(dev_data.ScopeEffectTech, reshadeCmdlist, cmdlist_data.current_main_rtv);
//						
//					}
//					ImGui::NewLine();
//				}
//			}
//			
//
//			if (ImGui::Checkbox("Edit Mode", &bEnableEditMode))
//			{
//				auto player = PlayerCharacter::GetSingleton();
//				if (player) {
//					bool isInADS = player->gunState == GUN_STATE::kSighted || player->gunState == GUN_STATE::kFireSighted;
//					RE::TESIdleForm* sightedForm;
//
//					if (isInADS && !bEnableEditMode) {
//						player->SetInIronSightsImpl(false);
//						//ended
//						sightedForm = (TESIdleForm*)TESForm::GetFormByID(0x4AD9);
//						player->currentProcess->PlayIdle(player, 0x35, sightedForm);
//					}
//					else if (!isInADS && bEnableEditMode) {
//
//						player->SetInIronSightsImpl(true);
//						//started
//						sightedForm = (TESIdleForm*)TESForm::GetFormByID(0x4D32);
//						player->currentProcess->PlayIdle(player, 0x35, sightedForm);
//					}
//					
//				}
//				
//			}
//
//
//			if (ImGui::Button("Reload Setting", { 120, 60 })) {
//				if (ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData())
//				{
//					ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->ReloadFTSData();
//					InitCurrentScopeData(nullptr);
//					bReloadRenderMode = true;
//				}
//			}
//
//			if (bEnableEditMode) 
//			{
//
//				if (currData) 
//				{
//					if (instance().bRefreshChar) {
//						//currData = ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
//					//instance().effectIndex_UI	= ScopeDataHandler::GetSingleton()->GetEffectIndex();
//						UsingSTS_UI				= currData->UsingSTS;
//						scopeFrame_UI			= currData->scopeFrame;
//						IsCircle_UI				= currData->shaderData.IsCircle;
//						camDepth_UI				= currData->shaderData.camDepth;
//						ReticleSize_UI			= currData->shaderData.ReticleSize;
//						minZoom_UI				= currData->shaderData.minZoom;
//						maxZoom_UI				= currData->shaderData.maxZoom;
//						PositionOffset_UI[0]	= currData->shaderData.PositionOffset[0];
//						PositionOffset_UI[1]	= currData->shaderData.PositionOffset[1];
//						OriPositionOffset_UI[0] = currData->shaderData.OriPositionOffset[0];
//						OriPositionOffset_UI[1] = currData->shaderData.OriPositionOffset[1];
//						Size_UI[0]				= currData->shaderData.Size[0];
//						Size_UI[1]				= currData->shaderData.Size[1];
//						OriSize_UI[0]			= currData->shaderData.OriSize[0];
//						OriSize_UI[1]			= currData->shaderData.OriSize[1];
//
//						radius_UI				= currData->shaderData.parallax.radius;
//						relativeFogRadius_UI	= currData->shaderData.parallax.relativeFogRadius;
//						scopeSwayAmount_UI		= currData->shaderData.parallax.scopeSwayAmount;
//						maxTravel_UI			= currData->shaderData.parallax.maxTravel;
//
//						bEnableZMove			= currData->shaderData.bEnableZMove;
//						bEnableNVGEffect		= currData->shaderData.bCanEnableNV;
//						nvIntensity_UI			= currData->shaderData.nvIntensity;
//						baseWeaponPos_UI		= currData->shaderData.baseWeaponPos;
//						MovePercentage_UI		= currData->shaderData.movePercentage;
//						instance().bRefreshChar = false;
//					}
//
//					if (!instance().player)
//						return;
//
//					ImGui::NewLine();
//					if (ImGui::TreeNode("Main Menu"))
//					{
//						ImGui::Checkbox("Disable Culling", &UsingSTS_UI);
//						ImGui::Checkbox("Disable Scope Effect While Bolt Change", &bDisableWhileBolt);
//						ImGui::DragInt("Effect Delay(ms)", (int*)&scopeFrame_UI,1, 0, 5000);
//						ImGui::NewLine();
//						ImGui::TreePop();
//					}
//
//					if (ImGui::TreeNode("Shader Data"))
//					{
//						ImGui::Checkbox("Circle Scope", &IsCircle_UI);
//						ImGui::Checkbox("bEnable ZMove", &bEnableZMove);
//
//						if (bEnableZMove) {
//
//							float currWeaponPos = 
//								  powf(instance().weaponPos.x - instance().rootPos.x, 2) 
//								+ powf(instance().weaponPos.y - instance().rootPos.y, 2) 
//								+ powf(instance().weaponPos.z - instance().rootPos.z, 2);
//								 //powf(instance().weaponPos.z - instance().rootPos.z, 2);
//
//							ImGui::Text("curr WeaponPos_UI %.3f", currWeaponPos);
//							ImGui::DragFloat("Base Weapon Pos", &baseWeaponPos_UI, 0.05F);
//							ImGui::DragFloat("ZMove Percentage", &MovePercentage_UI, 0.01F, 0, 1);
//							ImGui::NewLine();
//						}
//						
//
//						ImGui::DragFloat("Scope Depth", &camDepth_UI, 0.01F, 0, 15);
//						ImGui::DragFloat("Reticle Size", &ReticleSize_UI, 0.01F, 0, 128);
//						ImGui::NewLine();
//						ImGui::DragFloat("Min Zoom", &minZoom_UI, 0.01F, 0, 15);
//						ImGui::DragFloat("Max Zoom", &maxZoom_UI, 0.01F, 0, 15);
//						ImGui::NewLine();
//						ImGui::DragFloat2("Dest Pos Offset", PositionOffset_UI, 0.1F, -3840, 3840);
//						ImGui::DragFloat2("Dest Scope Size", Size_UI, 0.1F, 0, 3840);
//						ImGui::NewLine();
//						ImGui::DragFloat2("Ori Pos Offset", OriPositionOffset_UI, 0.1F, -3840, 3840);
//						ImGui::DragFloat2("Ori Scope Size", OriSize_UI, 0.1F, 0, 3840);
//						ImGui::NewLine();
//
//						ImGui::Checkbox("Enable Night Vision", &bEnableNVGEffect);
//						ImGui::DragFloat("Night Vision Intensity", &nvIntensity_UI, 0.1F, 0, 1000);
//						ImGui::NewLine();
//						ImGui::TreePop();
//					}
//
//					if (ImGui::TreeNode("Parallax Data"))
//					{
//						ImGui::DragFloat("Radius", &radius_UI, 0.01F, 0, 15);
//						ImGui::DragFloat("RelativeFog Radius", &relativeFogRadius_UI, 0.01F, 0, 15);
//						ImGui::DragFloat("Scope Sway Amount", &scopeSwayAmount_UI, 0.01F, 0, 15);
//						ImGui::DragFloat("Max Travel", &maxTravel_UI, 0.01F, 0, 15);
//						ImGui::NewLine();
//						ImGui::TreePop();
//					}
//
//					{
//						runtime->set_uniform_value_float(euv_Offset, PositionOffset_UI, 2);
//						runtime->set_uniform_value_float(euv_OriOffset, OriPositionOffset_UI, 2);
//						runtime->set_uniform_value_float(euv_Size, Size_UI, 2);
//						runtime->set_uniform_value_float(euv_OriSize, OriSize_UI, 2);
//
//						runtime->set_uniform_value_float(euv_AimNodeSize, ReticleSize_UI, 1);
//						runtime->set_uniform_value_float(euv_pRadius, radius_UI, 1);
//						runtime->set_uniform_value_float(euv_pRelativeFogRadius, relativeFogRadius_UI, 1);
//						runtime->set_uniform_value_float(euv_pScopeSwayAmount, scopeSwayAmount_UI, 1);
//						runtime->set_uniform_value_float(euv_pMaxTravel, maxTravel_UI, 1);
//
//						runtime->set_uniform_value_float(euv_BaseWeaponPos, baseWeaponPos_UI, 3);
//						runtime->set_uniform_value_float(euv_MovePercentage, MovePercentage_UI, 1);
//						runtime->set_uniform_value_bool(euv_EnableZMove, bEnableZMove, 1);
//
//						runtime->set_uniform_value_bool(euv_IsCircle, IsCircle_UI);
//						runtime->set_uniform_value_float(euv_CamDepth, camDepth_UI);
//
//						//runtime->set_uniform_value_bool(euv_EnableNightVision, bEnableNVGEffect);
//						runtime->set_uniform_value_float(euv_NVIntensity, nvIntensity_UI);
//
//						dev_data.offset_from_last_pass = instance().effectIndex_UI;
//					}
//					
//					if (ImGui::Button("Save Setting", { 100, 40 })) {
//
//						dev_data.offset_from_last_pass = ScopeDataHandler::GetSingleton()->GetEffectIndex();
//						ScopeDataHandler::GetSingleton()->SetEffectIndex(instance().effectIndex_UI);
//						currData->UsingSTS = UsingSTS_UI;
//						currData->scopeFrame = scopeFrame_UI;
//						currData->shaderData.IsCircle = IsCircle_UI;
//						currData->shaderData.bCanEnableNV = bEnableNVGEffect;
//						currData->shaderData.baseWeaponPos = baseWeaponPos_UI;					
//						currData->shaderData.bEnableZMove = bEnableZMove;		
//						currData->shaderData.movePercentage = MovePercentage_UI;				
//						currData->shaderData.camDepth = camDepth_UI;		
//						currData->shaderData.ReticleSize = ReticleSize_UI;	
//						currData->shaderData.minZoom = minZoom_UI;		
//						currData->shaderData.maxZoom = maxZoom_UI;		
//						currData->shaderData.PositionOffset[0] = PositionOffset_UI[0];
//						currData->shaderData.PositionOffset[1] = PositionOffset_UI[1];
//						currData->shaderData.OriPositionOffset[0] = OriPositionOffset_UI[0];
//						currData->shaderData.OriPositionOffset[1] = OriPositionOffset_UI[1];
//						currData->shaderData.Size[0] = Size_UI[0];
//						currData->shaderData.Size[1] = Size_UI[1];
//						currData->shaderData.OriSize[0] = OriSize_UI[0];
//						currData->shaderData.OriSize[1] = OriSize_UI[1];
//						currData->shaderData.parallax.radius = radius_UI;
//						currData->shaderData.parallax.relativeFogRadius = relativeFogRadius_UI;
//						currData->shaderData.parallax.scopeSwayAmount = scopeSwayAmount_UI;
//						currData->shaderData.parallax.maxTravel = maxTravel_UI;
//
//
//
//						UsingSTS_UI				= NULL;
//						scopeFrame_UI			= NULL;
//						IsCircle_UI				= NULL;
//						camDepth_UI				= NULL;
//						ReticleSize_UI			= NULL;
//						minZoom_UI				= NULL;
//						maxZoom_UI				= NULL;
//						PositionOffset_UI[0]	= NULL;
//						PositionOffset_UI[1]	= NULL;
//						OriPositionOffset_UI[0] = NULL;
//						OriPositionOffset_UI[1] = NULL;
//						Size_UI[0]				= NULL;
//						Size_UI[1]				= NULL;
//						OriSize_UI[0]			= NULL;
//						OriSize_UI[1]			= NULL;
//						radius_UI				= NULL;
//						relativeFogRadius_UI	= NULL;
//						scopeSwayAmount_UI		= NULL;
//						maxTravel_UI			= NULL;
//						bEnableZMove			= NULL;
//						baseWeaponPos_UI		= NULL;
//						MovePercentage_UI		= NULL;
//						bEnableNVGEffect		= NULL;
//
//
//						ScopeData::ScopeDataHandler::GetSingleton()->WriteCurrentFTSData();
//						instance().bRefreshChar = true;
//					}
//				}
//				//else
//				//{
//				//	ImGui::Text("Can't read FTS data! Please ensure you are in game and have a FTS supported weapon in hand!");
//				//}
//			}
//
//	}
//
//
//	void Impl::on_reshade_begin_effects(effect_runtime* runtime, command_list* cmd_list, resource_view rtv, resource_view rtv_srgb)
//	{
//	
//		device* const device = cmd_list->get_device();
//		auto& dev_data = device->get_private_data<device_data>();
//		auto& data = cmd_list->get_private_data<command_list_data>();
//
//		
//		if (mTextDDS_SRV && bChangeAimTexture) {
//			bChangeAimTexture = false;
//			resource_view* rv = reinterpret_cast<resource_view*>(mTextDDS_SRV.GetAddressOf());
//			runtime->update_texture_bindings("RETICLETEXTURE", *rv);
//		}
//		if (!isReloading)
//			SetShaderData(runtime);
//		isReloading = false;
//
//	}
//	void Impl::StartScope(bool flag)
//	{
//		instance().bStartScope = flag;
//
//	}
//
//
//	void Impl::SetInterfaceTextRefresh(bool flag)
//	{
//		instance().bRefreshChar = flag;
//		bEnableEditMode = false;
//	}
//
//	
//
//	void Impl::on_reloaded_effects(reshade::api::effect_runtime* runtime)
//	{
//		firstTime = true;
//		isReloading = true;
//	}
//
//
//	void Impl::SetIndex(uint32_t tindex)
//	{
//		index = tindex;
//	}
//	
//	uint32_t Impl::GetIndex()
//	{
//		return index;
//	}
//
//	void Impl::ZoomIn()
//	{
//		wheelDelta += 1;
//	}
//	void Impl::ZoomOut()
//	{
//		wheelDelta -= 1;
//	}
//
//	void Impl::SetAreaCopy(bool flag){ActiveAreaCopy = flag;}
//	bool Impl::GetAreaCopy(){return ActiveAreaCopy;}
//	void Impl::InitOffset(bool flag){IssueInit = flag;}
//	void Impl::SetRenderPassIndex(int passIndex) {instance().effectIndex_UI = passIndex;}
//	void Impl::SetRenderEffect(bool flag){instance().bEnableEffect = flag;}
//	void Impl::SetFinishAimAnim(bool flag){instance().bFinishAimAnim = flag;}
//	void Impl::SetNVG(bool flag) { instance().bEnableNVG = flag; }
//	void Impl::SetTestFlag(bool flag){instance().bTestingFlag = flag;}
//	void Impl::SetVirDir(RE::NiPoint3 p) { instance().virDir = p; }
//	void Impl::SetLastVirDir(RE::NiPoint3 p) { instance().lastVirDir = p; }
//	void Impl::SetVirDirLerp(RE::NiPoint3 p) { instance().VirDirLerp = p; }
//	void Impl::SetVirTransLerp(RE::NiPoint3 p) { instance().VirTransLerp = p; }
//	void Impl::SetWeaponPos(RE::NiPoint3 p) { instance().weaponPos = p; }
//	void Impl::SetRootPos(RE::NiPoint3 p) { instance().rootPos = p; }
//	void Impl::SetCamMat(RE::NiMatrix3 p) { instance().camMat = p; }		
//
//
//	void Impl::UpdateTexture()
//	{
//
//		if (reshadeDevice->get_api() == device_api::d3d12)
//			return;
//
//		auto& dev_data = reshadeDevice->get_private_data<device_data>();
//		
//		currData = ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData();
//
//		if (!currData)
//			return;
//
//		std::string tempstr = ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->ZoomNodePath;
//		std::wstring widestr = std::wstring(tempstr.begin(), tempstr.end());
//		const wchar_t* widecstr = widestr.c_str();
//
//		auto runtime = dev_data.main_runtime;
//		//auto cmdqueue = runtime->get_command_queue();
//
//		//if (runtime->get_device()->get_api() == device_api::d3d12) {
//		//	//mDevice12 = reinterpret_cast<ID3D12Device*>(device->get_native());
//
//		//	HRESULT hr{ S_OK };
//		//	hr = D3D11On12CreateDevice(
//		//		reinterpret_cast<IUnknown*> (runtime->get_device()->get_native()),
//		//		D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
//		//		NULL,
//		//		1u,
//		//		reinterpret_cast<IUnknown**>(&(*cmdqueue)),
//		//		1,
//		//		0,
//		//		&mDevice,
//		//		nullptr,
//		//		nullptr);
//		//	assert(SUCCEEDED(hr));
//		//} else {
//		//	
//		//}
//		//InitDDSRes();
//
//		mDevice = reinterpret_cast<ID3D11Device*>(runtime->get_device()->get_native());
//
//		HRESULT hr = CreateDDSTextureFromFile(mDevice.Get(), widecstr, mTextDDS.GetAddressOf(), mTextDDS_SRV.GetAddressOf());
//
//		if (hr != S_OK) {
//			CreateDDSTextureFromFile(mDevice.Get(), L"Data/Textures/FTS/Empty.dds", mTextDDS.GetAddressOf(), mTextDDS_SRV.GetAddressOf());
//		}
//
//		
//		
//
//		if (reshadeDevice && dev_data.main_runtime && mTextDDS_SRV.GetAddressOf()) {
//			resource_view* rv = reinterpret_cast<resource_view*>(mTextDDS_SRV.GetAddressOf());
//			dev_data.main_runtime->update_texture_bindings("RETICLETEXTURE", *rv);
//		}
//		
//		bReloadRenderMode = true;
//		
//	}
//
//#pragma region RenderBeforeUI
//	struct ImageSpaceEffectTemporalAA_Render
//	{
//		static void thunk(uint64_t This, uint64_t a2, uint64_t a3)
//		{
//			if (*ScopeData::ScopeDataHandler::GetSingleton()->GetEnableRenderThroughUI() && isActive_TAA) {
//				if (reshadeDevice && reshadeCmdlist && !firstTime) {
//					auto dev_data = reshadeDevice->get_private_data<device_data>();
//					effect_runtime* runtime = dev_data.main_runtime;
//					auto cmdlist_data = reshadeCmdlist->get_private_data<command_list_data>();
//
//					if (!ReshadeImpl::Impl::instance().bIsUpscaler) {
//						if (runtime && &(dev_data.ScopeEffectTech.handle) && ReshadeImpl::Impl::instance().GetAreaCopy())
//							runtime->render_technique(dev_data.ScopeEffectTech, reshadeCmdlist, cmdlist_data.current_main_rtv);
//						runtime->render_effects(reshadeCmdlist, cmdlist_data.current_main_rtv);
//					}
//				}
//			}
//			func(This, a2, a3);
//		}
//		static inline REL::Relocation<decltype(thunk)> func;
//	};
//
//	struct ImageSpaceEffectTemporalAA_IsActive
//	{
//		static bool thunk(uint64_t a1)
//		{
//			isActive_TAA = func(a1);
//			return isActive_TAA;
//		}
//		static inline REL::Relocation<decltype(thunk)> func;
//	};
//
//	struct ImageSpaceEffectBokehDepthOfField_Render
//	{
//		static void thunk(uint64_t This, uint64_t a2, uint64_t a3)
//		{
//			func(This, a2, a3);
//		}
//		static inline REL::Relocation<decltype(thunk)> func;
//	};
//
//	struct ImageSpaceEffectBokehDepthOfField_IsActive
//	{
//		static bool thunk(uint64_t a1)
//		{
//			if (*ScopeData::ScopeDataHandler::GetSingleton()->GetEnableRenderThroughUI() && !isActive_TAA) {
//				if (reshadeDevice && reshadeCmdlist && !firstTime) {
//					auto dev_data = reshadeDevice->get_private_data<device_data>();
//					effect_runtime* runtime = dev_data.main_runtime;
//					auto cmdlist_data = reshadeCmdlist->get_private_data<command_list_data>();
//
//					if (!ReshadeImpl::Impl::instance().bIsUpscaler) {
//						if (runtime && &(dev_data.ScopeEffectTech.handle) && ReshadeImpl::Impl::instance().GetAreaCopy())
//							runtime->render_technique(dev_data.ScopeEffectTech, reshadeCmdlist, cmdlist_data.current_main_rtv);
//						runtime->render_effects(reshadeCmdlist, cmdlist_data.current_main_rtv);
//					}
//				}
//			}
//
//			return func(a1);
//		}
//		static inline REL::Relocation<decltype(thunk)> func;
//	};
//
//	void Impl::SetIsUpscaler(bool flag)
//	{
//		bIsUpscaler = flag;
//	}
//
//	bool Impl::bIsUpscaler = false;
//#pragma endregion
//
//
//
//
//	void Impl::InitReshadeImpl(HMODULE module)
//	{
//		reshade::register_addon((HMODULE)F4SE::WinAPI::GetCurrentModule());
//
//		reshade::register_event<reshade::addon_event::init_device>(on_init_device);
//		reshade::register_event<reshade::addon_event::destroy_device>(on_destroy_device);
//		reshade::register_event<reshade::addon_event::init_command_list>(on_init_command_list);
//		reshade::register_event<reshade::addon_event::destroy_command_list>(on_destroy_command_list);
//		reshade::register_event<reshade::addon_event::init_effect_runtime>(on_init_effect_runtime);
//		reshade::register_event<reshade::addon_event::destroy_effect_runtime>(on_destroy_effect_runtime);
//
//		reshade::register_event<reshade::addon_event::begin_render_pass>(on_begin_render_pass);
//		reshade::register_event<reshade::addon_event::bind_render_targets_and_depth_stencil>(on_bind_render_targets_and_depth_stencil);
//
//		reshade::register_event<reshade::addon_event::reshade_present>(on_present);
//		//reshade::register_event<reshade::addon_event::present>(on_d3dPresent);
//		reshade::register_event<reshade::addon_event::reshade_begin_effects>(on_reshade_begin_effects);
//		//reshade::register_event<reshade::addon_event::draw_indexed>(on_draw_indexed);
//
//		reshade::register_event<reshade::addon_event::reshade_reloaded_effects>(on_reloaded_effects);
//		//reshade::register_overlay(nullptr, on_draw_settings);
//		reshade::register_overlay("FakeThroughScope", on_draw_settings_Self);
//
//		write_vfunc<0x1, ImageSpaceEffectTemporalAA_Render>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
//		write_vfunc<0x1, ImageSpaceEffectBokehDepthOfField_Render>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());
//		write_vfunc<0x8, ImageSpaceEffectTemporalAA_IsActive>(RE::VTABLE::ImageSpaceEffectTemporalAA[0].address());
//		write_vfunc<0x8, ImageSpaceEffectBokehDepthOfField_IsActive>(RE::VTABLE::ImageSpaceEffectBokehDepthOfField[0].address());
//
//		//ImGuiIO& io = ImGui::GetIO();
//		//ImFont* font = (&ImGui::GetIO())->Fonts->AddFontFromMemoryTTF((void*)aliFont_data, aliFont_size, 17.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
//	}
//
//	
//}
//
