//#pragma once
//#include "imgui.h"
//#include "reshade.hpp"
//#include <d3d11.h>
//
//
//namespace ReshadeImpl
//{
//	using namespace reshade::api;
//
//	class Impl
//	{
//	public:
//
//		static Impl& instance()
//		{
//			static Impl instance;
//			return instance;
//		}
//
//	public:
//		void InitReshadeImpl(HMODULE module);
//
//		void SetIndex(uint32_t index);
//		uint32_t GetIndex();
//
//		void ZoomIn();
//		void ZoomOut();
//
//		void SetAreaCopy(bool flag);
//		bool GetAreaCopy();
//		void InitOffset(bool flag);
//
//		void UpdateTexture();
//
//		static void SetRenderEffect(bool);
//		static void SetFinishAimAnim(bool);
//
//		static void SetInterfaceTextRefresh(bool);
//		static void SetRenderPassIndex(int);
//
//		static void SetTestFlag(bool);
//		static void StartScope(bool);
//		static void SetNVG(bool);
//
//		static void InitPlayerAndCam(RE::PlayerCharacter*, RE::PlayerCamera*);
//		static void SetVirDir(RE::NiPoint3 p);
//		static void SetLastVirDir(RE::NiPoint3 p);	
//		static void SetVirDirLerp(RE::NiPoint3 p);	
//		static void SetVirTransLerp(RE::NiPoint3 p);
//		static void SetWeaponPos(RE::NiPoint3 p);	
//		static void SetRootPos(RE::NiPoint3 p);		
//		static void SetCamMat(RE::NiMatrix3 p);		
//		static void SetShaderData();
//		static void SetShaderData(effect_runtime* runtime);
//		static bool GetFirstTime();
//		static bool IsEditorMode();
//		static void SetIsUpscaler(bool);
//
//
//
//	private:
//		Impl() noexcept = default;
//		~Impl() noexcept = default;
//
//		static void on_init_device(device* device);
//		static void on_destroy_device(device* device);
//		static void on_init_command_list(command_list* cmd_list);
//		static void on_destroy_command_list(command_list* cmd_list);
//		static void on_init_effect_runtime(effect_runtime* runtime);
//		static void on_destroy_effect_runtime(effect_runtime* runtime);
//		static void on_begin_render_pass(command_list* cmd_list, uint32_t count, const render_pass_render_target_desc* rts, const render_pass_depth_stencil_desc*);
//		static void on_bind_render_targets_and_depth_stencil(command_list* cmd_list, uint32_t count, const resource_view* rtvs, resource_view);
//		static bool on_draw_indexed(command_list* cmd_list, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
//		static void on_present(effect_runtime* runtime);
//		static void on_d3dPresent(command_queue* queue, swapchain* swapchain, const rect* source_rect, const rect* dest_rect, uint32_t dirty_rect_count, const rect* dirty_rects);
//		static void on_reshade_begin_effects(effect_runtime* runtime, command_list* cmd_list, resource_view rtv, resource_view rtv_srgb);
//		static void on_draw_settings_Self(effect_runtime* runtime);
//		static void on_reloaded_effects(reshade::api::effect_runtime* runtime);
//		
//
//		
//	private:
//		uint32_t index;
//		uint32_t range;
//		int16_t wheelDelta;
//		int effectIndex_UI = 1;
//		int InitalEffectPassCount;
//	private:
//
//		bool bStartScope = false;
//		bool bEnableNVG = false;
//		bool IssueInit = false;
//		bool bSaveSetting = false;
//		bool bEnableEffect = false;
//		bool bFinishAimAnim = false;
//		bool bTestingFlag = false;
//		bool bRefreshChar = true;
//		
//	public:
//		static bool bIsUpscaler;
//		bool ActiveAreaCopy = false;
//	private:
//		float lerpSize[2]{ 0, 0 };
//		std::vector<RE::NiNode*> cullingNodeList;
//
//	private:
//		RE::NiPoint3 virDir;
//		RE::NiPoint3 lastVirDir;
//		RE::NiPoint3 VirDirLerp;
//		RE::NiPoint3 VirTransLerp;
//		RE::NiPoint3 weaponPos;
//		RE::NiPoint3 rootPos;
//		RE::NiMatrix3 camMat;
//		RE::PlayerCharacter* player;
//		RE::PlayerCamera* pcam;	
//
//	};
//}
//
