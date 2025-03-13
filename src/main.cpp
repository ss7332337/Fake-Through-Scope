#include "FTSData.h"
#include "ImGuiImpl.h"
#include <hooking.h>

using namespace RE;
using namespace BSScript;
using namespace std;


bool bNeedToUpdateFTSData = true;
bool bChangeAnimFlag = false;
bool nvgFlag = false;
bool hasCombo = false;
bool hasNvgCommit = false;
bool InGameFlag = false;
bool IsHooked = false;
bool bHasStartedScope = false;
bool testingFlag = false;
bool hasUpdateSighted = false;
bool hasEjectShellCasing = false;

bool bEnableScope = false;
float gameDeltaZoom = 1;
float scopeTimer = 0;

//ReshadeImpl::Impl* reshadeImpl;
ScopeData::ScopeDataHandler* sdh;
PlayerCharacter* player;
PlayerCamera* pcam;

RE::BGSKeyword* an_45;
RE::BGSKeyword* AnimsXM2010_scopeKH45;
RE::BGSKeyword* AnimsXM2010_scopeKM;
RE::BGSKeyword* AX50_toounScope_K;
RE::BGSKeyword* AnimsAX50_scopeKH45;
RE::BGSKeyword* QMW_AnimsQBZ191M_on;
RE::BGSKeyword* QMW_AnimsQBZ191M_off;
RE::BGSKeyword* QMW_AnimsRU556M_on;
RE::BGSKeyword* QMW_AnimsRU556M_off;
RE::BGSKeyword* Tull_SideAimKeyword;
RE::BGSKeyword* Tull_SupportKeyword;
RE::BGSKeyword* AX50_toounScope_L;
RE::BGSKeyword* AnimsAX50_scopeK;

RE::NiAVObject* scopeNode;
RE::NiAVObject* camNode;
RE::NiAVObject* weaponNode;
RE::NiAVObject* rootNode;

RE::NiNode* scopeNormalNode3rd_i;
RE::NiNode* scopeAimingNode3rd_i;

RE::NiNode* scopeNormalNode_i;
RE::NiNode* scopeAimingNode_i;
RE::TESObjectWEAP::InstanceData* weaponInstanceData;

PlayerControls* pc;
HMODULE Upscaler;

NiPoint4 lastPosition;
NiPoint4 currPosition;
//float* ptr_deltaTime;
BSTimer* uiTimer;
//float deltaTime;

REL::Relocation<uintptr_t> ptr_PCUpdateMainThread{ REL::ID(633524), 0x22D };
uintptr_t PCUpdateMainThreadOrig;


BGSKeyword* ChangeAnimFlavorKeyword = nullptr;
ScopeData::FTSData* currentData;
const char* customPath = "Data\\F4SE\\Plugins\\FTS";

using namespace Hook;

D3D* hookIns;
ImGuiImpl::ImGuiImplClass* imgui_Impl;


char* _MESSAGE(const char* fmt, ...);


template <class Ty>
Ty SafeWrite64Function(uintptr_t addr, Ty data)
{
	DWORD oldProtect;
	void* _d[2];
	memcpy(_d, &data, sizeof(data));
	size_t len = sizeof(_d[0]);

	VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	Ty olddata;
	memset(&olddata, 0, sizeof(Ty));
	memcpy(&olddata, (void*)addr, len);
	memcpy((void*)addr, &_d[0], len);
	VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
	return olddata;
}


TESForm* GetFormFromMod(std::string modname, uint32_t formid)
{
	if (!modname.length() || !formid)
		return nullptr;
	TESDataHandler* dh = TESDataHandler::GetSingleton();
	TESFile* modFile = nullptr;
	for (auto it = dh->files.begin(); it != dh->files.end(); ++it) {
		TESFile* f = *it;
		if (strcmp(f->filename, modname.c_str()) == 0) {
			modFile = f;
			break;
		}
	}
	if (!modFile)
		return nullptr;
	uint8_t modIndex = modFile->compileIndex;
	uint32_t id = formid;
	if (modIndex < 0xFE) {
		id |= ((uint32_t)modIndex) << 24;
	} else {
		uint16_t lightModIndex = modFile->smallFileCompileIndex;
		if (lightModIndex != 0xFFFF) {
			id |= 0xFE000000 | (uint32_t(lightModIndex) << 12);
		}
	}
	return TESForm::GetFormByID(id);
}


DWORD StartHooking(LPVOID)
{
	//Sleep(100);
	hookIns = Hook::D3D::GetSington();
	//hookIns->Hook();

	imgui_Impl = ImGuiImpl::ImGuiImplClass::GetSington();
	return 0;
}

bool compareByEditorID(const string& a, const BGSKeyword* b)
{
	return a.c_str() == b->formEditorID || a.empty();
}

bool IssueChangeAnim(std::monostate, BGSKeyword* keyword)
{
	bChangeAnimFlag = true;
	ChangeAnimFlavorKeyword = keyword;

	return true;
}

int testDegree = 0;

bool TestButton(std::monostate)
{
	/*NiPoint3 pos, dir;
	NiPoint3 right = NiPoint3(-1, 0, 0);
	player-> GetEyeVector(pos, dir, true);
	NiPoint3 heading = Normalize(NiPoint3(dir.x, dir.y, 0));

	testDegree += 5;

	NiMatrix3 rot = scopeNode->parent->parent->world.rotate * GetRotationMatrix33(heading, testDegree * toRad) * Transpose(scopeNode->parent->parent->world.rotate);
	scopeNode->parent->local.rotate = rot;*/
	//auto camStat = &BSGraphics::State::GetSingleton();
	//camStat->cameraState.camViewData.inv1stPersonProjMat = {};

	return true;
}

bool IsInADS(Actor* a)
{
	return (a->gunState == GUN_STATE::kSighted || a->gunState == GUN_STATE::kFireSighted);
}


bool IsSideAim()
{
	bool temp = false;
	if (player) 
	{
		if (an_45) {
			temp |= player->HasKeyword(an_45);
		}
		if (AnimsXM2010_scopeKH45) {
			temp |= player->HasKeyword(AnimsXM2010_scopeKH45);
		}
		if (AnimsXM2010_scopeKM) {
			temp |= player->HasKeyword(AnimsXM2010_scopeKM);
		} 
		if (AnimsAX50_scopeKH45) {
			temp |= player->HasKeyword(AnimsAX50_scopeKH45);
		} 
		if (Tull_SideAimKeyword) {
			temp |= player->HasKeyword(Tull_SideAimKeyword);
		} 
		if (AX50_toounScope_K) {
			temp |= player->HasKeyword(AX50_toounScope_K);
		}
		if (AX50_toounScope_L) {
			temp |= player->HasKeyword(AX50_toounScope_L);
		}
		if (AnimsAX50_scopeK) {
			temp |= player->HasKeyword(AnimsAX50_scopeK);
		}
	}
	return temp;
}

BGSKeyword* IsMagnifier()
{

	if (player) {
		if (QMW_AnimsQBZ191M_on) {
			if (player->HasKeyword(QMW_AnimsQBZ191M_on)) {
				_MESSAGE("QMW_AnimsQBZ191M_on");
				return QMW_AnimsQBZ191M_on;
			}
		} else if (QMW_AnimsQBZ191M_off) {
			if (player->HasKeyword(QMW_AnimsQBZ191M_off))
				return QMW_AnimsQBZ191M_off;
		} else if (QMW_AnimsRU556M_off) {
			if (player->HasKeyword(QMW_AnimsRU556M_off))
				return QMW_AnimsRU556M_off;
		} else if (QMW_AnimsRU556M_on) {
			if (player->HasKeyword(QMW_AnimsRU556M_on))
				return QMW_AnimsRU556M_on;
		} 
	}
	return nullptr;
}

inline void InitCurrentScopeData()
{
	if (player->currentProcess) {
		auto& eventEquipped = player->currentProcess->middleHigh->equippedItems;

		if (eventEquipped.size() > 0 && eventEquipped[0].item.instanceData &&
			((TESObjectWEAP::InstanceData*)eventEquipped[0].item.instanceData.get())->type == WEAPON_TYPE::kGun) {
			TESObjectWEAP* eventWep = ((TESObjectWEAP*)eventEquipped[0].item.object);
			TESObjectWEAP::InstanceData* eventInstance = (TESObjectWEAP::InstanceData*)eventEquipped[0].item.instanceData.get();

			if (!eventInstance || !eventInstance->GetKeywordData()) {
				sdh->SetCurrentFTSData(nullptr);
				return;
			}

			weaponInstanceData = eventInstance;
			if (weaponInstanceData->zoomData) 
			{
				imgui_Impl->UpdateWeaponInstance(weaponInstanceData);
			}

			BSScrapArray<const BGSKeyword*> playerkeywords;
			BSScrapArray<const BGSKeyword*> weaponKeywords;
			player->CollectAllKeywords(playerkeywords, nullptr);
			eventInstance->keywords->CollectAllKeywords(weaponKeywords, nullptr);

			string ftsPrefix = "FTS_";

			auto result = std::find_if(weaponKeywords.begin(), weaponKeywords.end(), [&ftsPrefix](const BGSKeyword* p) {
				return !std::strncmp(p->formEditorID.c_str(), ftsPrefix.c_str(), ftsPrefix.length());
			});

			if (result == weaponKeywords.end()) {
				sdh->SetCurrentFTSData(nullptr);
				return;
			}

			auto ScopeDataMap = sdh->GetScopeDataMap();

			auto it = ScopeDataMap->find((*result)->formEditorID.c_str());  // 返回一个迭代器

			if (it != ScopeDataMap->end()) {
				auto ftsData = it->second;
				auto qbzAni = IsMagnifier();

				bool additionalKeywordsResult = true;
				for (const auto& s : ftsData->additionalKeywords) {
					if (!eventInstance->keywords->HasKeywordString(s))
					{
						additionalKeywordsResult=false;
						break;
					}
				}

				if (!additionalKeywordsResult) 
				{
					sdh->SetCurrentFTSData(ftsData, false);
					return;
				}

				auto animFlavorKeyword = qbzAni ? std::string(qbzAni->formEditorID.c_str()) : std::string(it->second->animFlavorEditorID.c_str());

				if (animFlavorKeyword.empty() || std::strcmp(animFlavorKeyword.c_str(), "FTS_NONE") == 0) {
					sdh->SetCurrentFTSData(ftsData);
					return;
				} 
				else 
				{
					BGSKeyword* tempKeyword = (BGSKeyword*)TESForm::GetFormByEditorID(animFlavorKeyword.c_str());
					
					if (player->HasKeyword(tempKeyword))
					{
						sdh->SetCurrentFTSData(ftsData);
						_MESSAGE("HasKeyword");
					}
					else
						sdh->SetCurrentFTSData(nullptr);
				}
			}
		} 
		else
			sdh->SetCurrentFTSData(nullptr);
	}
}

bool IsShowMenu = false;

/// <summary>
/// The hook method is from Bingle
/// </summary>
class InputEventReceiverOverride : public BSInputEventReceiver
{
public:
	typedef void (InputEventReceiverOverride::*FnPerformInputProcessing)(const InputEvent* a_queueHead);

	//using Virtual-Key Codes
	void ProcessButtonEvent(ButtonEvent* evn)
	{

			if (evn->eventType != INPUT_EVENT_TYPE::kButton) {
				if (evn->next)
					ProcessButtonEvent((ButtonEvent*)evn->next);
				return;
			}

			uint32_t id = evn->idCode;
			if (evn->device == INPUT_DEVICE::kMouse)
				id += 0x100;
			if (evn->device == INPUT_DEVICE::kGamepad)
				id += 0x10000;

			//if (evn->device == INPUT_DEVICE::kKeyboard && id == VK_OEM_PERIOD && evn->QJustPressed()) {
			//	std::monostate mono;
			//	//TestButton(mono);
			//}


			if (evn->device == INPUT_DEVICE::kKeyboard) {


				if (currentData) {
					if (sdh->comboNVKey == -1) {
						if (id == (uint32_t)sdh->nvKey && evn->QJustPressed()) {
							nvgFlag = !nvgFlag;
							hookIns->SetNVG((int)nvgFlag);
						}
					} else {
						if (id == (uint32_t)(sdh->comboNVKey) && evn->heldDownSecs > 0 && evn->value == 1) {
							hasCombo = true;
						}

						if (id == (uint32_t)(sdh->comboNVKey) && evn->value == 0) {
							hasCombo = false;
						}

						if (hasCombo && id == (uint32_t)sdh->nvKey && evn->QJustPressed()) {
							nvgFlag = !nvgFlag;
							hookIns->SetNVG((int)nvgFlag);
						}
					}
				}
			}
	
		
	
	}


	void HookedPerformInputProcessing(const InputEvent* a_queueHead)
	{
		if (!UI::GetSingleton()->menuMode 
			&& !UI::GetSingleton()->GetMenuOpen("LooksMenu")
			&& !UI::GetSingleton()->GetMenuOpen("ScopeMenu") 
			&& a_queueHead) {
			ProcessButtonEvent((ButtonEvent*)a_queueHead);
			//ProcessButtonEventForMouse((MouseMoveEvent*)a_queueHead);
		}
		
		FnPerformInputProcessing fn = fnHash.at(*(uint64_t*)this);
		if (fn) {
			(this->*fn)(a_queueHead);
		}
	}

	void HookSink()
	{
		uint64_t vtable = *(uint64_t*)this;
		auto it = fnHash.find(vtable);
		if (it == fnHash.end()) {
			FnPerformInputProcessing fn = SafeWrite64Function(vtable, &InputEventReceiverOverride::HookedPerformInputProcessing);
			fnHash.insert(std::pair<uint64_t, FnPerformInputProcessing>(vtable, fn));
		}
	}

	void UnHookSink()
	{
		uint64_t vtable = *(uint64_t*)this;
		auto it = fnHash.find(vtable);
		if (it == fnHash.end())
			return;
		SafeWrite64Function(vtable, it->second);
		fnHash.erase(it);
	}

protected:
	static unordered_map<uint64_t, FnPerformInputProcessing> fnHash;
};
unordered_map<uint64_t, InputEventReceiverOverride::FnPerformInputProcessing> InputEventReceiverOverride::fnHash;


bool IsNeedToBeCull(int indexCount = 0, int StrideCount = 0)
{
	if (!ScopeData::ScopeDataHandler::GetSingleton())
		return false;
	if (!ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData())
		return false;
	if (ScopeData::ScopeDataHandler::GetSingleton()->GetCurrentFTSData()->UsingSTS)
		return false;
	if (hasUpdateSighted)
		return false;

	return true;
}

void HandleScopeNode()
{
	if (!currentData || currentData->UsingSTS)
		return;

	if (player) {
		if (player->Get3D(false)) {
			scopeNormalNode3rd_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->Get3D(false)->GetObjectByName("ScopeNormal");
			scopeAimingNode3rd_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->Get3D(false)->GetObjectByName("ScopeAiming");

			if (scopeNormalNode3rd_i && scopeAimingNode3rd_i) {
				if (RE::PlayerCharacter::GetSingleton()->IsInThirdPerson()) {
					scopeNormalNode3rd_i->SetAppCulled(false);
					scopeAimingNode3rd_i->SetAppCulled(true);
					return;
				}
			}
		}

		if (player->firstPerson3D) {
			scopeNormalNode_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->firstPerson3D->GetObjectByName("ScopeNormal");
			scopeAimingNode_i = (RE::NiNode*)RE::PlayerCharacter::GetSingleton()->firstPerson3D->GetObjectByName("ScopeAiming");

			if (scopeNormalNode_i && scopeAimingNode_i) {
				if (bEnableScope) {
					if (IsNeedToBeCull()) {
						scopeNormalNode_i->SetAppCulled(true);
						scopeAimingNode_i->SetAppCulled(false);

					} else {
						scopeNormalNode_i->SetAppCulled(false);
						scopeAimingNode_i->SetAppCulled(true);
					}
				} else {
					scopeNormalNode_i->SetAppCulled(false);
					scopeAimingNode_i->SetAppCulled(true);
				}
			}
		}
	}
}

bool isUpdateContext = false;

float timerA = 0;
bool bFirstTimeZoomData = false;

BSScrapArray<const BGSKeyword*> lastKeywords = BSScrapArray<const BGSKeyword*>();

void HookedUpdate()
{
	typedef void (*FnUpdate)();
	FnUpdate fn = (FnUpdate)PCUpdateMainThreadOrig;
	if (!fn) return;

	if (InGameFlag&& player && player->Get3D(true)) 
	{
		if (!bHasStartedScope && !imgui_Impl->bIsSaving)
		{
			//需要优化，尝试haskeyword可不可行
			BSScrapArray<const BGSKeyword*> currkeywords;
			player->CollectAllKeywords(currkeywords, nullptr);
			if (lastKeywords.empty() || !std::equal(lastKeywords.begin(), lastKeywords.end(), currkeywords.begin())) {
				InitCurrentScopeData();
				lastKeywords = currkeywords;
			}
		}

		if (currentData)
		{
			if (!bFirstTimeZoomData) {
				if (!hookIns->bEnableEditMode) {
					auto tempZDO = currentData->zoomDataOverwrite;
					if (tempZDO.enableZoomDateOverwrite && weaponInstanceData) {
						weaponInstanceData->zoomData->zoomData.fovMult = tempZDO.fovMul;
						weaponInstanceData->zoomData->zoomData.cameraOffset = { tempZDO.x, tempZDO.y, tempZDO.z };
					}
				}
				bFirstTimeZoomData = true;
			}

			scopeNode = player->Get3D(true)->GetObjectByName("FTS:CenterPoint");
			camNode = player->Get3D(true)->GetObjectByName("Camera");

			pc = PlayerControls::GetSingleton();
			NiPoint3 tempOut;

			pcam = PlayerCamera::GetSingleton();

			NiPointer<bhkCharacterController> con = player->currentProcess->middleHigh->charController;
			uintptr_t charProxy = *(uintptr_t*)((uintptr_t)con.get() + 0x470);
			hkTransform* charProxyTransform = (hkTransform*)(charProxy + 0x40);

			if (scopeNode && camNode) {
				tempOut = hookIns->WorldToScreen(camNode, scopeNode, 90);
				currPosition = charProxyTransform->translation;
				NiPoint4 VirTransLerp = {
					currPosition.x - lastPosition.x,
					currPosition.y - lastPosition.y,
					currPosition.z - lastPosition.z,
					currPosition.w - lastPosition.w
				};
				lastPosition = currPosition;

				NiPoint3 virDir = scopeNode->world.translate - camNode->world.translate;
				NiPoint3 lastVirDir = scopeNode->previousWorld.translate - camNode->previousWorld.translate;
				NiPoint3 VirDirLerp = NiPoint3(virDir - lastVirDir);

				NiPoint3 weaponPos = scopeNode->world.translate;
				NiPoint3 rootPos = camNode->world.translate;

				D3D::GameConstBuffer gcb;

				gcb.virDir = virDir;

				gcb.lastVirDir = lastVirDir;
				gcb.VirDirLerp = VirDirLerp;
				gcb.VirTransLerp = { VirTransLerp.x, VirTransLerp.y, VirTransLerp.z };
				gcb.weaponPos = weaponPos;
				gcb.rootPos = rootPos;

				gcb.camMat = camNode->world.rotate;
				gcb.ftsLocalMat = scopeNode->local.rotate;
				gcb.ftsWorldMat = scopeNode->world.rotate;
				gcb.ftsScreenPos = tempOut;

				if (bHasStartedScope) {
					scopeTimer += uiTimer->delta * 1000;
					if (!bHasStartedScope) {
						scopeTimer = 0;
						return (*fn)();
					}
					if (scopeTimer >= sdh->GetCurrentFTSData()->scopeFrame) {
						bEnableScope = true;
						hookIns->SetScopeEffect(true);
						scopeTimer = 0;
						bHasStartedScope = false;
					}
				} else {
					scopeTimer = 0;
				}

				hookIns->SetGameConstData(gcb);
				HandleScopeNode();

				//PauseMenu
				//WorkshopMenu
				//CursorMenu

				if (IsSideAim() || RE::UI::GetSingleton()->GetMenuOpen("PauseMenu") || RE::UI::GetSingleton()->GetMenuOpen("WorkshopMenu") || RE::UI::GetSingleton()->GetMenuOpen("CursorMenu")) {
					hookIns->EnableRender(false);
					hookIns->QueryRender(false);
				} else {
					if (IsInADS(player)) {
						if (!hookIns->bEnableEditMode) {
							auto tempZDO = currentData->zoomDataOverwrite;
							if (tempZDO.enableZoomDateOverwrite && weaponInstanceData) {
								weaponInstanceData->zoomData->zoomData.fovMult = tempZDO.fovMul;
								weaponInstanceData->zoomData->zoomData.cameraOffset = { tempZDO.x, tempZDO.y, tempZDO.z };
							}
						}

						hookIns->EnableRender(true);
						hookIns->QueryRender(true);
					}
				}

				if (!IsInADS(player)) {
					hookIns->EnableRender(false);
				}
			}
		}

	}

	 (*fn)();
}



/// <summary>
/// The hook method is from Bingle
/// </summary>
class EquipWatcher : public BSTEventSink<TESEquipEvent>
{
public:
	virtual BSEventNotifyControl ProcessEvent(const TESEquipEvent& evn, BSTEventSource<TESEquipEvent>* a_source)
	{
		Actor* a = evn.actor->As<Actor>();

		if (a == player) {

			TESForm* item = TESForm::GetFormByID(evn.baseObject);
			/*reshade::log_message(4, "" + evn.formId);
			reshade::log_message(4, "Player!");*/
			
			if (evn.equipped) {
				
			}

			if (evn.equipped && item && item->formType == ENUM_FORMTYPE::kWEAP) {
				hookIns->QueryRender(false);
				sdh->SetCurrentFTSData(nullptr);
				InitCurrentScopeData();
				hookIns->SetInterfaceTextRefresh(true);
				bChangeAnimFlag = false;
				hookIns->QueryChangeReticleTexture();
				hookIns->ResetZoomDelta();
			}
		}
		
		return BSEventNotifyControl::kContinue;
	}
	F4_HEAP_REDEFINE_NEW(EquipWatcher);
};

/// <summary>
/// The hook method is from Bingle
/// </summary>
using std::unordered_map;
class AnimationGraphEventWatcher
{
public:
	typedef BSEventNotifyControl (AnimationGraphEventWatcher::*FnProcessEvent)(BSAnimationGraphEvent& evn, BSTEventSource<BSAnimationGraphEvent>* dispatcher);

	BSEventNotifyControl HookedProcessEvent(BSAnimationGraphEvent& evn, BSTEventSource<BSAnimationGraphEvent>* src)
	{
		FnProcessEvent fn = fnHash.at(*(uint64_t*)this);
		string prefix = "";

	//	_MESSAGE("evn.animEvent: %s; evn.argument: %s", evn.animEvent.c_str(), evn.argument.c_str());

		if (IsInADS(player)) {

			if (!IsSideAim() && !player->IsInThirdPerson() && bNeedToUpdateFTSData) {

				if (bChangeAnimFlag) {
					InitCurrentScopeData();
				}

				currentData = sdh->GetCurrentFTSData();

				if (currentData && !bHasStartedScope) {

					hookIns->StartScope(true);
					hookIns->SetFinishAimAnim(true);
					bHasStartedScope = true;
				}

				bNeedToUpdateFTSData = false;
			}

			if (sdh->GetCurrentFTSData() && sdh->GetCurrentFTSData()->shaderData.bBoltDisable) 
			{
				if (hasEjectShellCasing) {
					hookIns->SetScopeEffect(true);
					hasEjectShellCasing = false;
					hasUpdateSighted = false;
				}

				if (strcmp(evn.tag.c_str(), "UpdateSighted") == 0) {
					hookIns->SetScopeEffect(false);
					hasUpdateSighted = true;
				}

				if (strcmp(evn.tag.c_str(), "initiateBoltStart") == 0) {
					hookIns->SetScopeEffect(false);
					hasUpdateSighted = true;
				}

				/*if (hasUpdateSighted && strcmp(evn.animEvent.c_str(), "ReloadEnd") == 0) {
					hasUpdateSighted = false;
					hasEjectShellCasing = true;
				}*/
				if (hasUpdateSighted && strcmp(evn.tag.c_str(), "initiateStart") == 0) {
					hasUpdateSighted = false;
					hasEjectShellCasing = true;
				}
			}	
		} 
		else{

			hasUpdateSighted = false;
			hasEjectShellCasing = false;
			hookIns->StartScope(false);
			bNeedToUpdateFTSData = true;
			hookIns->SetScopeEffect(false);
			bHasStartedScope = false;	
			bEnableScope = false;
		}


		return fn ? (this->*fn)(evn, src) : BSEventNotifyControl::kContinue;
	}

	void HookSink()
	{
		uint64_t vtable = *(uint64_t*)this;
		auto it = fnHash.find(vtable);
		if (it == fnHash.end()) {
			FnProcessEvent fn = SafeWrite64Function(vtable + 0x8, &AnimationGraphEventWatcher::HookedProcessEvent);
			fnHash.insert(std::pair<uint64_t, FnProcessEvent>(vtable, fn));
		}
	}

	void UnHookSink()
	{
		uint64_t vtable = *(uint64_t*)this;
		auto it = fnHash.find(vtable);
		if (it == fnHash.end())
			return;
		SafeWrite64Function(vtable + 0x8, it->second);
		fnHash.erase(it);
	}

protected:
	static unordered_map<uint64_t, FnProcessEvent> fnHash;
};
unordered_map<uint64_t, AnimationGraphEventWatcher::FnProcessEvent> AnimationGraphEventWatcher::fnHash;

bool RegisterFuncs(BSScript::IVirtualMachine* vm)
{
	stl::zstring fileName = "FakeThroughScope";
	//vm->BindNativeMethod(fileName, "TestButton", TestButton);

	vm->BindNativeMethod(fileName, "OnChangeAnimFlavor", IssueChangeAnim);


	return true;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
	_MESSAGE("MainThread");
	

	while (!BSGraphics::RendererData::GetSingleton() 
		|| !BSGraphics::RendererData::GetSingleton()->renderWindow 
		|| !BSGraphics::RendererData::GetSingleton()->renderWindow->hwnd 
		|| !BSGraphics::RendererData::GetSingleton()->renderWindow->swapChain 
		|| !RE::BSGraphics::RendererData::GetSingleton()->device
		|| !RE::BSGraphics::RendererData::GetSingleton()->context) {
		Sleep(10);
	}



	hookIns = Hook::D3D::GetSington();
	hookIns->ImplHookDX11_Init(hModule, BSGraphics::RendererData::GetSingleton()->renderWindow->hwnd);

	imgui_Impl = ImGuiImpl::ImGuiImplClass::GetSington();
	hookIns->SetImGuiImplClass(imgui_Impl);
	//ImplHookDX11_Init(hModule, BSGraphics::RendererData::GetSingleton()->renderWindow->hwnd);

	return S_OK;
}

void TestingThread()
{
	
	while (true) {
		auto bstimer = UI::GetSingleton()->uiTimer;
		auto gameVm  = GameVM::GetSingleton()->profiler.timer;
		if (&bstimer && GameVM::GetSingleton() && &gameVm)
		{
			Sleep(100);
		}
	}
}

void InitializePlugin()
{
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, (HMODULE)REX::W32::GetCurrentModule(), 0, NULL);
#ifdef _DEBUG
	HANDLE hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TestingThread, (HMODULE)REX::W32::GetCurrentModule(), 0, NULL);
#endif
	uiTimer = &UI::GetSingleton()->uiTimer;

	logger::warn("ptr_PCUpdateMainThread.address: {}", ptr_PCUpdateMainThread.address());
	logger::warn("ptr_PCUpdateMainThread.offset: {}", ptr_PCUpdateMainThread.offset());
	logger::warn("threadID: {}", RE::Main::GetSingleton()->threadID);

	hookIns->EnableRender(true);

	player = PlayerCharacter::GetSingleton();

	pcam = PlayerCamera::GetSingleton();
	((InputEventReceiverOverride*)((uint64_t)pcam + 0x38))->HookSink();
	
	an_45 = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("an_45d");
	AnimsXM2010_scopeKH45 = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AnimsXM2010_scopeKH45");
	AX50_toounScope_K = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AX50_toounScope_K");
	AnimsXM2010_scopeKM = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AnimsXM2010_scopeKM");
	AnimsAX50_scopeKH45 = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AnimsAX50_scopeKH45");
	AX50_toounScope_L = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AX50_toounScope_L");
	AnimsAX50_scopeK = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("AnimsAX50_scopeK");
	Tull_SideAimKeyword = (BGSKeyword*)GetFormFromMod("Tull_Framework.esp", 0x804);
	Tull_SupportKeyword = (BGSKeyword*)GetFormFromMod("Tull_Framework.esp", 0x80F);

	QMW_AnimsQBZ191M_on = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("QMW_AnimsQBZ191M_on");
	QMW_AnimsQBZ191M_off = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("QMW_AnimsQBZ191M_off");
	QMW_AnimsRU556M_on = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("QMW_AnimsRU556M_on");
	QMW_AnimsRU556M_off = (RE::BGSKeyword*)RE::TESForm::GetFormByEditorID("QMW_AnimsRU556M_off");

	sdh = ScopeData::ScopeDataHandler::GetSingleton();

	((AnimationGraphEventWatcher*)((uint64_t)PlayerCharacter::GetSingleton() + 0x38))->HookSink();
	

	EquipWatcher* ew = new EquipWatcher();
	TESEquipEvent::GetSingleton()->RegisterSink(ew);
	hookIns->QueryChangeReticleTexture();
	sdh->ReadCustomScopeDataFiles(customPath);
	sdh->ReadDefaultScopeDataFile();
	
}

void ResetScopeStatus()
{
	RE::ControlMap::GetSingleton()->ignoreKeyboardMouse = false;

	hookIns->InitPlayerData(player, pcam);
	InitCurrentScopeData();
	hookIns->SetScopeEffect(false);
	hookIns->QueryChangeReticleTexture();

	if (sdh->GetCurrentFTSData()) {
		gameDeltaZoom = sdh->GetCurrentFTSData()->shaderData.minZoom;
	}

	InGameFlag = true;
	hookIns->SetIsInGame(InGameFlag);
	hookIns->SetInterfaceTextRefresh(true);

}




F4SE_EXPORT bool F4SEAPI F4SEPlugin_Query(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = "FakeThroughScope";
	a_info->version = 1;

	a_info->infoVersion = F4SE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;


#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}.{}.{}"), Version::PROJECT, Version::MAJOR, Version::MINOR, Version::PATCH);


	if (a_f4se->IsEditor()) {
		logger::critical("loaded in editor");
		return false;
	}

	const auto ver = a_f4se->RuntimeVersion();
	if ((REL::Module::IsF4() && ver < F4SE::RUNTIME_1_10_163) ||
		(REL::Module::IsVR() && ver < F4SE::RUNTIME_LATEST_VR)) {
		logger::critical("unsupported runtime v{}", ver.string());
		return false;
	}

	logger::info("FakeThroughScope Loaded!");

	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StartHooking, NULL, NULL, NULL);
	/*hookIns = Hook::D3D::GetSington();
	imgui_Impl = new ImGuiImpl::ImGuiImplClass();

	hookIns->SetImGuiImplClass(imgui_Impl);

	Hook::D3D::Register();*/

	
	F4SE::AllocTrampoline(8 * 8);
	

	return true;
}


F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
#ifdef _DEBUG
	//while (!IsDebuggerPresent()) {
	//}
	//Sleep(1000);
#endif

	F4SE::Init(a_f4se);

	F4SE::Trampoline& trampoline = F4SE::GetTrampoline();
	PCUpdateMainThreadOrig = trampoline.write_call<5>(ptr_PCUpdateMainThread.address(), &HookedUpdate);

	const F4SE::PapyrusInterface* papyrus = F4SE::GetPapyrusInterface();
	bool succ = papyrus->Register(RegisterFuncs);
	if (succ) {
		logger::warn("succ.");
	}

	const F4SE::MessagingInterface* message = F4SE::GetMessagingInterface();
	message->RegisterListener([](F4SE::MessagingInterface::Message* msg) -> void {
		if (msg->type == F4SE::MessagingInterface::kPostLoad) {
			//if (MH_CreateHookApiEx(L"dxgi.dll", "D3D11CreateDeviceAndSwapChain", Hooked_D3D11CreateDeviceAndSwapChain, (LPVOID*)&fpOriginalD3D11CreateDeviceAndSwapChain, NULL) != MH_OK) {
			//	_MESSAGE("Failed to hook D3D11CreateDeviceAndSwapChain");
			//}

			//if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
			//	// 处理错误
			//	_MESSAGE("Failed to enable hooks");
			//}

		} else if (msg->type == F4SE::MessagingInterface::kGameDataReady) {
			InitializePlugin();

		} else if (msg->type == F4SE::MessagingInterface::kPostLoadGame) {
			ResetScopeStatus();
		} else if (msg->type == F4SE::MessagingInterface::kNewGame) {
			ResetScopeStatus();
		} else if (msg->type == F4SE::MessagingInterface::kPostSaveGame) {
			//reshadeImpl->SetRenderEffect(true);
		} else if (msg->type == F4SE::MessagingInterface::kGameLoaded) {
			//reshadeImpl->SetRenderEffect(false);
		}
	});

	return true;
}

F4SE_EXPORT constinit auto F4SEPlugin_Version = []() noexcept {
	F4SE::PluginVersionData data{};

	data.AuthorName(Version::AUTHOR);
	data.PluginName(Version::PROJECT);
	data.PluginVersion(Version::VERSION);

	data.UsesAddressLibrary(true);
	data.IsLayoutDependent(true);
	data.UsesSigScanning(false);
	data.HasNoStructUse(false);

	data.CompatibleVersions({ F4SE::RUNTIME_1_10_163 });
	return data;
}();


//
//extern "C" DLLEXPORT bool F4SEAPI F4SEPlugin_Load(const F4SE::LoadInterface* a_f4se)
//{
//
//}
