#include "ImGuiImpl.h"
#include "FTSData.h"
#include "hooking.h"
#include <d3d11.h>
#include <DirectXMath.h>


inline void InitCurrentScopeData();

namespace ImGuiImpl
{
	std::vector<std::string> additionalKeywords;
	int additionalKeywords_count = 0;
	bool legacyFlag = true;

#define LF(f) (legacyFlag ? (f) : (f) / 1000.0f)
#define FL(f) (legacyFlag ? (f) : (f) * 1000.0f)

	std::unique_ptr<float[]> LFA(float arr[], size_t size)
	{
		std::unique_ptr<float[]> arrNew(new float[size]);

		for (size_t i = 0; i < size; i++) {
			arrNew[i] = LF(arr[i]);
		}

		return arrNew;
	}

	std::unique_ptr<float[]> FLA(float arr[], size_t size)
	{
		std::unique_ptr<float[]> arrNew(new float[size]);

		for (size_t i = 0; i < size; i++) {
			arrNew[i] = FL(arr[i]);
		}

		return arrNew;
	}

	using namespace ScopeData;
	using namespace DirectX;
	Hook::D3D::ScopeEffectShaderData scopeData;
	Hook::D3D* d3d;

	ScopeData::FTSData* currData;
	ScopeData::ScopeDataHandler* sdh;

	RE::PlayerCharacter* player;
	RE::PlayerCamera* pcam;
	bool bInitZoomData = false;
	RE::TESObjectWEAP::InstanceData* Imgui_InstanceData;

	void ImGuiImplClass::UpdateWeaponInstance(RE::TESObjectWEAP::InstanceData* instanceData)
	{
		Imgui_InstanceData = instanceData;
	}

	void ImGuiImplClass::UpdateImGuiData()
	{
		currData = sdh->GetCurrentFTSData();
		//ResetUIData(this, currData);
	}

	bool ImGuiImplClass::CheckAndInit()
	{
		if (!sdh)
			sdh = ScopeData::ScopeDataHandler::GetSingleton();

		if (!player)
			player = RE::PlayerCharacter::GetSingleton();

		if (!pcam)
			pcam = RE::PlayerCamera::GetSingleton();

		if (!sdh || !player || !pcam)
			return false;
		return true;
	}

	void StartOfImGuiRender()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void EndOfImGuiRender()
	{
		ImGui::End();
		ImGui::Render();
	}

	void KeyBindingSection(int& nvgComboKeyIndex, int& nvgMainKeyIndex, int& guiKeyIndex)
	{
		if (nvgComboKeyIndex != -1 && nvgMainKeyIndex != -1 && ImGui ::TreeNode("Key Binding")) {
			nvgComboKeyIndex = sdh->comboNVKey + 1;
			nvgMainKeyIndex = sdh->nvKey + 1;
			guiKeyIndex = sdh->guiKey + 1;

			if (ImGui::Combo("GUI Key", &guiKeyIndex, mainKey, 117)) {
				sdh->SetGuiKey(guiKeyIndex - 1);
			}

			if (ImGui::Combo("NVG Combo Key", &nvgComboKeyIndex, mainKey, 166)) {
				sdh->SetNVGHotKeyCombo(nvgComboKeyIndex - 1);
			}

			if (ImGui::Combo("NVG Main Key", &nvgMainKeyIndex, mainKey, 166)) {
				sdh->SetNVGHotKeyMain(nvgMainKeyIndex - 1);
			}

			ImGui::TreePop();
			ImGui::NewLine();
		}
	}

	void ImGuiImplClass::PlayerAim(bool isShow)
	{
		CheckAndInit();

		bool isInADS = player->gunState == RE::GUN_STATE::kSighted || player->gunState == RE::GUN_STATE::kFireSighted;
		RE::TESIdleForm* sightedForm;

		if (isShow)
		{
			player->SetInIronSightsImpl(true);
			//started
			sightedForm = (RE::TESIdleForm*)RE::TESForm::GetFormByID(0x4D32);
			player->currentProcess->PlayIdle(player, 0x35, sightedForm);
		}
		else
		{
			player->SetInIronSightsImpl(false);
			//ended
			sightedForm = (RE::TESIdleForm*)RE::TESForm::GetFormByID(0x4AD9);
			player->currentProcess->PlayIdle(player, 0x35, sightedForm);
		}
	}

	void ResetUIData(ImGuiImplClass* ins)
	{
		if (d3d->bRefreshChar) {

			InitCurrentScopeData();
			currData = sdh->GetCurrentFTSData();
			if (!currData)
				return;

			auto data = currData;
			legacyFlag = data->legacyMode;

			ins->bLegacyMode = data->legacyMode;
			ins->UsingSTS_UI = ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->UsingSTS;
			ins->scopeFrame_UI = data->scopeFrame;
			ins->IsCircle_UI = data->shaderData.IsCircle;
			ins->camDepth_UI = data->shaderData.camDepth;
			ins->ReticleSize_UI = data->shaderData.ReticleSize;
			ins->minZoom_UI = data->shaderData.minZoom;
			ins->maxZoom_UI = data->shaderData.maxZoom;

			memcpy(ins->PositionOffset_UI, data->shaderData.PositionOffset, 2 * sizeof(float));
			memcpy(ins->OriPositionOffset_UI, data->shaderData.OriPositionOffset, 2 * sizeof(float));
			memcpy(ins->Size_UI, data->shaderData.Size, 2 * sizeof(float));
			memcpy(ins->OriSize_UI, data->shaderData.OriSize, 2 * sizeof(float));
			memcpy(ins->Size_rect_UI, data->shaderData.rectSize, 4 * sizeof(float));

			ins->radius_UI = data->shaderData.parallax.radius;
			ins->relativeFogRadius_UI = data->shaderData.parallax.relativeFogRadius;
			ins->scopeSwayAmount_UI = data->shaderData.parallax.scopeSwayAmount;
			ins->maxTravel_UI = data->shaderData.parallax.maxTravel;

			ins->bEnableZMove = data->shaderData.bEnableZMove;
			ins->bEnableNVGEffect = data->shaderData.bCanEnableNV;
			ins->nvIntensity_UI = data->shaderData.nvIntensity;
			ins->baseWeaponPos_UI = data->shaderData.baseWeaponPos;
			ins->MovePercentage_UI = data->shaderData.movePercentage;

			ins->bDisableWhileBolt = data->shaderData.bBoltDisable;

			ins->Imgui_ZDO = data->zoomDataOverwrite;

			additionalKeywords_count = data->additionalKeywords.size();
			additionalKeywords = data->additionalKeywords;

			d3d->bRefreshChar = false;
		}
	}

	void ImGuiImplClass::ReloadData()
	{
		if (ImGui::Button("Reload Setting", { 120, 60 })) {
			if (auto data = sdh->GetCurrentFTSData()) {
				sdh->ReloadFTSData(data);
				InitCurrentScopeData();
				currData = sdh->GetCurrentFTSData();
				ResetUIData(this);
			}
		}
	}

	void ImGuiImplClass::SaveData()
	{
		if (ImGui::Button("Save Setting", { 100, 40 })) {

			bIsSaving = true;
			currData = sdh->GetCurrentFTSData();
			currData->legacyMode = bLegacyMode;
			Hook::D3D::bLegacyMode = bLegacyMode;

			currData->UsingSTS = UsingSTS_UI;
			currData->scopeFrame = scopeFrame_UI;
			currData->shaderData.IsCircle = IsCircle_UI;
			//_MESSAGE("currData->shaderData.bCanEnableNV: %i, bEnableNVGEffect: %i", currData->shaderData.bCanEnableNV, bEnableNVGEffect);
			currData->shaderData.bCanEnableNV = bEnableNVGEffect;
			currData->shaderData.baseWeaponPos = baseWeaponPos_UI;
			currData->shaderData.bEnableZMove = bEnableZMove;
			currData->shaderData.movePercentage = MovePercentage_UI;
			currData->shaderData.camDepth = camDepth_UI;
			currData->shaderData.ReticleSize = ReticleSize_UI;
			currData->shaderData.minZoom = minZoom_UI;
			currData->shaderData.maxZoom = maxZoom_UI;
			currData->shaderData.PositionOffset[0] = PositionOffset_UI[0];
			currData->shaderData.PositionOffset[1] = PositionOffset_UI[1];
			currData->shaderData.OriPositionOffset[0] = OriPositionOffset_UI[0];
			currData->shaderData.OriPositionOffset[1] = OriPositionOffset_UI[1];
			currData->shaderData.Size[0] = Size_UI[0];
			currData->shaderData.Size[1] = Size_UI[1];
			currData->shaderData.OriSize[0] = OriSize_UI[0];
			currData->shaderData.OriSize[1] = OriSize_UI[1];
			currData->shaderData.parallax.radius = radius_UI;
			currData->shaderData.parallax.relativeFogRadius = relativeFogRadius_UI;
			currData->shaderData.parallax.scopeSwayAmount = scopeSwayAmount_UI;
			currData->shaderData.parallax.maxTravel = maxTravel_UI;
			currData->shaderData.bBoltDisable = bDisableWhileBolt;
			currData->shaderData.nvIntensity = nvIntensity_UI;
			currData->shaderData.fovAdjust = fovBase_UI;

			currData->shaderData.rectSize[0] = Size_rect_UI[0];
			currData->shaderData.rectSize[1] = Size_rect_UI[1];
			currData->shaderData.rectSize[2] = Size_rect_UI[2];
			currData->shaderData.rectSize[3] = Size_rect_UI[3];

			currData->zoomDataOverwrite.enableZoomDateOverwrite = Imgui_ZDO.enableZoomDateOverwrite;
			currData->zoomDataOverwrite.x = Imgui_InstanceData->zoomData->zoomData.cameraOffset.x;
			currData->zoomDataOverwrite.y = Imgui_InstanceData->zoomData->zoomData.cameraOffset.y;
			currData->zoomDataOverwrite.z = Imgui_InstanceData->zoomData->zoomData.cameraOffset.z;
			currData->zoomDataOverwrite.fovMul = Imgui_InstanceData->zoomData->zoomData.fovMult;

			currData->additionalKeywords = additionalKeywords;

			sdh->SetCurrentFTSData(currData);
			sdh->WriteCurrentFTSData();
			ResetUIData(this);
			d3d->bRefreshChar = true;

			bIsSaving = false;
		}
	}

	void ImGuiImplClass::MapScopeShaderEffect()
	{
		Hook::D3D::bLegacyMode = bLegacyMode;

		scopeData.ScopeEffect_Offset = { LF(PositionOffset_UI[0]), LF(PositionOffset_UI[1]) };

		scopeData.ScopeEffect_OriPositionOffset = {  LF(OriPositionOffset_UI[0]),  LF(OriPositionOffset_UI[1]) };
		scopeData.ScopeEffect_Size = { LF(Size_UI[0]), LF(Size_UI[1]) };
		scopeData.ScopeEffect_OriSize = { OriSize_UI[0], OriSize_UI[1] };
		scopeData.ReticleSize = ReticleSize_UI;
		scopeData.parallax_Radius = radius_UI;
		scopeData.parallax_relativeFogRadius = relativeFogRadius_UI;
		scopeData.parallax_scopeSwayAmount = scopeSwayAmount_UI;
		scopeData.parallax_maxTravel = maxTravel_UI;

		scopeData.BaseWeaponPos = baseWeaponPos_UI;
		scopeData.MovePercentage = MovePercentage_UI;
		scopeData.EnableZMove = bEnableZMove;
		scopeData.EnableNV = bEnableNVGEffect;

		scopeData.isCircle = IsCircle_UI;
		scopeData.camDepth = camDepth_UI;
		scopeData.baseFovAdjustTarget = fovBase_UI;

		auto Size_rect_UI_A = LFA(Size_rect_UI, 4);
		scopeData.rect = { Size_rect_UI_A[0], Size_rect_UI_A[1], Size_rect_UI_A[2], Size_rect_UI_A[3] };

		scopeData.nvIntensity = nvIntensity_UI;

		d3d->MapScopeEffectBuffer(scopeData);
	}

	void ImGuiImplClass::MainMenuSection()
	{
		if (ImGui::TreeNode("Main Menu")) 
		{
			ImGui::Checkbox("Legacy Mode (Need Save to Work!!)", &bLegacyMode);
			Hook::D3D::bLegacyMode = bLegacyMode;
			ImGui::Checkbox("Disable Culling", &UsingSTS_UI);
			ImGui::Checkbox("Disable Scope Effect While Bolt", &bDisableWhileBolt);
			ImGui::DragInt("Effect Delay(ms)", (int*)&scopeFrame_UI, 1, 0, 5000);

			/*ImGui::Text("curr Base Fov Adjust %.3f", pcam->fovAdjustCurrent);
			ImGui::DragFloat("Base Fov Adjust ", &fovBase_UI, 0.05F);*/
			ImGui::NewLine();

			if (ImGui::TreeNode("Additional Keywords"))
			{
				ImGui::InputInt("Additional Keywords Count", &additionalKeywords_count);

				if (additionalKeywords_count < additionalKeywords.size())
					additionalKeywords.pop_back();

				additionalKeywords_count = max(0, min(100, additionalKeywords_count));

				for (int i = 0; i < additionalKeywords_count; i++) {
					std::string label = "keyword " + std::to_string(i);

					std::string text = i < additionalKeywords.size() ? additionalKeywords[i] : "";
					ImGui::InputTextWithHint(label.c_str(), "Must be saved to take effect", &text);

					if (i < additionalKeywords.size())
						additionalKeywords[i] = text;
					else
						additionalKeywords.push_back(text);
				}
				ImGui::TreePop();
			}

			ImGui::NewLine();

			ImGui::Checkbox("Enable ZoomData Overwrite", &Imgui_ZDO.enableZoomDateOverwrite);
			if (Imgui_ZDO.enableZoomDateOverwrite)
			{
				if (Imgui_InstanceData && Imgui_InstanceData->zoomData)
				{
					ImGui::DragFloat("fov mul", &Imgui_InstanceData->zoomData->zoomData.fovMult, 0.01F, 0, 30, "%.3f");
					ImGui::DragFloat("x", &Imgui_InstanceData->zoomData->zoomData.cameraOffset.x, 0.1F, -50, 50, "%.3f");
					ImGui::DragFloat("y", &Imgui_InstanceData->zoomData->zoomData.cameraOffset.y, 0.1F, -50, 50, "%.3f");
					ImGui::DragFloat("z", &Imgui_InstanceData->zoomData->zoomData.cameraOffset.z, 0.1F, -50, 50, "%.3f");
				}
				else
				{
					ImGui::Text("InstanceData ERROR!");
				}
				
			}
			ImGui::TreePop();
		}
	}

	void ImGuiImplClass::ShaderDataSection()
	{
		if (ImGui::TreeNode("Shader Data")) {
			ImGui::Checkbox("Circle Scope", &IsCircle_UI);
			ImGui::Checkbox("bEnable ZMove", &bEnableZMove);

			Hook::D3D::GameConstBuffer* gameConstBuffer = d3d->GetGameConstBuffer();

			if (bEnableZMove) {
				float currWeaponPos =
					powf(gameConstBuffer->weaponPos.x - gameConstBuffer->rootPos.x, 2) + powf(gameConstBuffer->weaponPos.y - gameConstBuffer->rootPos.y, 2) + powf(gameConstBuffer->weaponPos.z - gameConstBuffer->rootPos.z, 2);

				ImGui::Text("curr WeaponPos_UI %.3f", currWeaponPos);
				ImGui::DragFloat("Base Weapon Pos", &baseWeaponPos_UI, 0.05F);
				ImGui::DragFloat("ZMove Percentage", &MovePercentage_UI, 0.01F, -10, 10);
				ImGui::NewLine();
			}

			ImGui::DragFloat("Scope Depth", &camDepth_UI, 0.01F, 0, 15);
			ImGui::DragFloat("Reticle Size", &ReticleSize_UI, 0.01F, 0, 128);
			ImGui::NewLine();
			ImGui::DragFloat("Min Zoom", &minZoom_UI, 0.01F, 0, 15);
			ImGui::DragFloat("Max Zoom", &maxZoom_UI, 0.01F, 0, 15);
			ImGui::NewLine();

			if (bLegacyMode)
				ImGui::DragFloat2("Dest Pos Offset", PositionOffset_UI, 0.1F, -3840, 3840);
			else
				ImGui::DragFloat2("Dest Pos Offset", PositionOffset_UI, 0.1F, -1000, 1000, "%.2f");

			if (bLegacyMode) 
			{
				ImGui::DragFloat2("Dest Scope Size", Size_UI, 1.0F, 0, 3840, "%.4f");
			} else {
				if (IsCircle_UI)
					ImGui::DragFloat2("Dest Scope Size", Size_UI, 1.0F, 0, 3840, "%.4f");
				else
					ImGui::DragFloat4("Dest Scope Size", Size_rect_UI, 1.0F, -1200, 1200, "%.2f");
			}

			ImGui::NewLine();

			if (bLegacyMode)
				ImGui::DragFloat2("Ori Pos Offset", OriPositionOffset_UI, 0.1F, -3840, 3840);
			else
				ImGui::DragFloat2("Ori Pos Offset", OriPositionOffset_UI, 0.1F, -1000, 1000, "%.2f");

			//ImGui::DragFloat2("Ori Scope Size", OriSize_UI, 0.1F, 0, 3840);
			ImGui::NewLine();

			ImGui::Checkbox("Enable Night Vision", &bEnableNVGEffect);
			ImGui::DragFloat("Night Vision Intensity", &nvIntensity_UI, 0.1F, 0, 1000);
			ImGui::NewLine();
			ImGui::TreePop();
		}
	}

	void ImGuiImplClass::ParallaxDataSection()
	{
		if (ImGui::TreeNode("Parallax Data")) {
			ImGui::DragFloat("Radius", &radius_UI, 0.01F, 0, 20);
			ImGui::DragFloat("RelativeFog Radius", &relativeFogRadius_UI, 0.01F, 0, 20);
			ImGui::DragFloat("Scope Sway Amount", &scopeSwayAmount_UI, 0.01F, 0, 20);
			ImGui::DragFloat("Max Travel", &maxTravel_UI, 0.01F, 0, 20);
			ImGui::NewLine();
			ImGui::TreePop();
		}
	}

	ImGuiImplClass::ImGuiImplClass()
	{
		if (!sdh)
			sdh = ScopeData::ScopeDataHandler::GetSingleton();

		d3d = Hook::D3D::GetSington();
		currData = sdh->GetCurrentFTSData();
	}

	void ImGuiImplClass::RenderImgui()
	{
		if (bCanRender) {
			//StartOfImGuiRender();

			ImGui::Begin("Fake Throught Scope Adjusting Menu by XiFeiLi");

			if (!CheckAndInit()) {
				ImGui::TextUnformatted("Unable to access critical data!");
				ImGui::NewLine();
				EndOfImGuiRender();
				return;
			}

			ResetUIData(this);

			if (!currData) {
				ImGui::TextUnformatted("This is not FTS Supported Weapon, Consider make a Patch or Double Check your Patch!");
				ImGui::NewLine();
				EndOfImGuiRender();
				return;
			}

			KeyBindingSection(nvgComboKeyIndex, nvgMainKeyIndex, guiKeyIndex);

			ImGui::Checkbox("Edit Mode", &Hook::D3D::bEnableEditMode);

			if (Hook::D3D::bEnableEditMode)
			{
				ReloadData();

				ImGui::NewLine();

				MainMenuSection();

				ImGui::NewLine();

				ShaderDataSection();

				ImGui::NewLine();

				ParallaxDataSection();

				SaveData();

				MapScopeShaderEffect();
			}
		}
	}
	bool ImGuiImplClass::EnableImGuiRender(bool isShow, bool bIsInGame)
	{
		if (isShow && bIsInGame) {
			bCanRender = true;
			return true;
		}
		bCanRender = false;
		return false;
	}
}

