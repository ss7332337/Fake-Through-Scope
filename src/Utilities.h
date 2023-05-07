#pragma once
#include "RE/Bethesda/TESDataHandler.h"
#include <Windows.h>
#pragma region Utilities
using namespace RE;
namespace RE
{
	enum class EShaderPropertyFlag : std::uint64_t
	{
		kSpecular = 1i64 << 0,
		kSkinned = 1i64 << 1,
		kTempRefraction = 1i64 << 2,
		kVertexAlpha = 1i64 << 3,
		kGrayscaleToPaletteColor = 1i64 << 4,
		kGrayscaleToPaletteAlpha = 1i64 << 5,
		kFalloff = 1i64 << 6,
		kEnvMap = 1i64 << 7,
		kRGBFalloff = 1i64 << 8,
		kCastShadows = 1i64 << 9,
		kFace = 1i64 << 10,
		kUIMaskRects = 1i64 << 11,
		kModelSpaceNormals = 1i64 << 12,
		kRefractionClamp = 1i64 << 13,
		kMultiTextureLandscape = 1i64 << 14,
		kRefraction = 1i64 << 15,
		kRefractionFalloff = 1i64 << 16,
		kEyeReflect = 1i64 << 17,
		kHairTint = 1i64 << 18,
		kScreendoorAlphaFade = 1i64 << 19,
		kLocalMapClear = 1i64 << 20,
		kFaceGenRGBTint = 1i64 << 21,
		kOwnEmit = 1i64 << 22,
		kProjectedUV = 1i64 << 23,
		kMultipleTextures = 1i64 << 24,
		kTesselate = 1i64 << 25,
		kDecal = 1i64 << 26,
		kDynamicDecal = 1i64 << 27,
		kCharacterLight = 1i64 << 28,
		kExternalEmittance = 1i64 << 29,
		kSoftEffect = 1i64 << 30,
		kZBufferTest = 1i64 << 31,
		kZBufferWrite = 1i64 << 32,
		kLODLandscape = 1i64 << 33,
		kLODObjects = 1i64 << 34,
		kNoFade = 1i64 << 35,
		kTwoSided = 1i64 << 36,
		kVertexColors = 1i64 << 37,
		kGlowMap = 1i64 << 38,
		kTransformChanged = 1i64 << 39,
		kDismembermentMeatCuff = 1i64 << 40,
		kTint = 1i64 << 41,
		kVertexLighting = 1i64 << 42,
		kUniformScale = 1i64 << 43,
		kFitSlope = 1i64 << 44,
		kBillboard = 1i64 << 45,
		kLODLandBlend = 1i64 << 46,
		kDismemberment = 1i64 << 47,
		kWireframe = 1i64 << 48,
		kWeaponBlood = 1i64 << 49,
		kHideOnLocalMap = 1i64 << 50,
		kPremultAlpha = 1i64 << 51,
		kVATSTarget = 1i64 << 52,
		kAnisotropicLighting = 1i64 << 53,
		kSkewSpecularAlpha = 1i64 << 54,
		kMenuScreen = 1i64 << 55,
		kMultiLayerParallax = 1i64 << 56,
		kAlphaTest = 1i64 << 57,
		kInvertedFadePattern = 1i64 << 58,
		kVATSTargetDrawAll = 1i64 << 59,
		kPipboyScreen = 1i64 << 60,
		kTreeAnim = 1i64 << 61,
		kEffectLighting = 1i64 << 62,
		kRefractionWritesDepth = 1i64 << 63
	};

	class NiUpdateData
	{
	public:
		uint32_t unk00;
		uint32_t unk04;
		uint64_t unk08;
		uint64_t unk10;
		uint32_t unk18;
	};

	class ExtraBendableSplineParams : public BSExtraData
	{
	public:
		float unk18;      // 18
		float thickness;  // 1C
		float xOffset;    // 20
		float yOffset;    // 24
		float zOffset;    // 28
		float unk2C;      // 2C
	};

	class BSAnimationGraphManager
	{
	public:
		bool Activate()
		{
			using func_t = decltype(&RE::BSAnimationGraphManager::Activate);
			REL::Relocation<func_t> func{ REL::ID(950096) };
			return func(this);
		}
		bool Deactivate()
		{
			using func_t = decltype(&RE::BSAnimationGraphManager::Deactivate);
			REL::Relocation<func_t> func{ REL::ID(591084) };
			return func(this);
		}
	};

	class TESObjectREFREx : public TESObjectREFR
	{
	public:
		void RebuildBendableSpline(bool rebuildCollision, NiAVObject* target)
		{
			using func_t = decltype(&RE::TESObjectREFREx::RebuildBendableSpline);
			REL::Relocation<func_t> func{ REL::ID(340319) };
			return func(this, rebuildCollision, target);
		}

		void SetLocationOnReference(const NiPoint3& pos)
		{
			using func_t = decltype(&RE::TESObjectREFREx::SetLocationOnReference);
			REL::Relocation<func_t> func{ REL::ID(1101832) };
			return func(this, pos);
		}

		void SetAngleOnReference(const NiPoint3& ang)
		{
			using func_t = decltype(&RE::TESObjectREFREx::SetAngleOnReference);
			REL::Relocation<func_t> func{ REL::ID(1049748) };
			return func(this, ang);
		}

		bool GetItemCount(uint32_t& count, TESForm* item, bool countComponent)
		{
			using func_t = decltype(&RE::TESObjectREFREx::GetItemCount);
			REL::Relocation<func_t> func{ REL::ID(635042) };
			return func(this, count, item, countComponent);
		}
	};

	class ActorEx : public Actor
	{
	public:
		void GetAimVector(NiPoint3& out)
		{
			using func_t = decltype(&RE::ActorEx::GetAimVector);
			REL::Relocation<func_t> func{ REL::ID(554863) };
			return func(this, out);
		}

		uint32_t GetCurrentCollisionGroup()
		{
			using func_t = decltype(&RE::ActorEx::GetCurrentCollisionGroup);
			REL::Relocation<func_t> func{ REL::ID(410500) };
			return func(this);
		}

		bool GetCurrentFireLocation(BGSEquipIndex index, NiPoint3& out)
		{
			using func_t = decltype(&RE::ActorEx::GetCurrentFireLocation);
			REL::Relocation<func_t> func{ REL::ID(663107) };
			return func(this, index, out);
		}

		float GetDesiredSpeed()
		{
			using func_t = decltype(&RE::ActorEx::GetDesiredSpeed);
			REL::Relocation<func_t> func{ REL::ID(106892) };
			return func(this);
		}
	};

	class BGSProjectileEx : public BGSProjectile
	{
	public:
		bool CollidesWithSmallTransparentLayer()
		{
			using func_t = decltype(&RE::BGSProjectileEx::CollidesWithSmallTransparentLayer);
			REL::Relocation<func_t> func{ REL::ID(1115694) };
			return func(this);
		}
	};

	class NiPoint3Ex : public NiPoint3
	{
	public:
		float GetZAngleFromVector()
		{
			using func_t = decltype(&RE::NiPoint3Ex::GetZAngleFromVector);
			REL::Relocation<func_t> func{ REL::ID(1450064) };
			return func(this);
		}
	};

	class NiCloningProcess
	{
	public:
		BSTHashMap<void*, void*> unk00;  // 00
		void* unk30;                     // 30
		uint64_t unk38;                  // 38
		uint64_t unk40;                  // 40
		void* unk48;                     // 48 - DEADBEEF
		void* unk50;                     // 50 - bucket
		uint64_t unk58;                  // 58
		uint32_t unk60;                  // 60 - copytype? 0, 1, 2
		uint32_t unk64;                  // 64
	};

	struct ActorEquipManagerEvent::Event
	{
		uint32_t unk00;            //00
		uint8_t pad04[0x7 - 0x4];  //04
		bool isUnequip;            //07
		void* unk08;               //08
		Actor* a;                  //10	equip target
	};

	struct TESObjectLoadedEvent
	{
		uint32_t formId;  //00
		uint8_t loaded;   //08
	};

	struct TESEquipEvent
	{
		Actor* a;         //00
		uint32_t formId;  //0C
		uint32_t unk08;   //08
		uint64_t flag;    //10 0x00000000ff000000 for unequip
	};

	struct PlayerAmmoCountEvent
	{
		uint32_t current;
		uint32_t total;
	};

	struct BSTransformDeltaEvent
	{
		NiMatrix3 deltaRotation;
		NiPoint4 deltaTranslation;
		NiPoint4 previousTranslation;
		NiPoint4 previousRotation;
		NiPoint4 previousScale;
		NiPoint4 currentTranslation;
		NiPoint4 currentRotation;
		NiPoint4 currentScale;
	};

	struct TESLoadGameEvent
	{};

	class HitEventSource : public BSTEventSource<TESHitEvent>
	{
	public:
		[[nodiscard]] static HitEventSource* GetSingleton()
		{
			REL::Relocation<HitEventSource*> singleton{ REL::ID(989868) };
			return singleton.get();
		}
	};

	class ObjectLoadedEventSource : public BSTEventSource<TESObjectLoadedEvent>
	{
	public:
		[[nodiscard]] static ObjectLoadedEventSource* GetSingleton()
		{
			REL::Relocation<ObjectLoadedEventSource*> singleton{ REL::ID(416662) };
			return singleton.get();
		}
	};

	class EquipEventSource : public BSTEventSource<TESEquipEvent>
	{
	public:
		[[nodiscard]] static EquipEventSource* GetSingleton()
		{
			REL::Relocation<EquipEventSource*> singleton{ REL::ID(485633) };
			return singleton.get();
		}
	};

	class MGEFApplyEventSource : public BSTEventSource<TESMagicEffectApplyEvent>
	{
	public:
		[[nodiscard]] static MGEFApplyEventSource* GetSingleton()
		{
			REL::Relocation<MGEFApplyEventSource*> singleton{ REL::ID(1481228) };
			return singleton.get();
		}
	};

	namespace LoadGameEventSource
	{
		[[nodiscard]] BSTEventSource<TESLoadGameEvent>* GetSingleton()
		{
			using func_t = decltype(&RE::LoadGameEventSource::GetSingleton);
			REL::Relocation<func_t> func{ REL::ID(823570) };
			return func();
		}
	};

	BSLightingShaderProperty* FindPerPixelLighting(NiNode* node)
	{
		using func_t = decltype(&RE::FindPerPixelLighting);
		REL::Relocation<func_t> func{ REL::ID(1287682) };
		return func(node);
	}

	struct DamageImpactData
	{
		NiPoint4 hitPos;
		NiPoint4 hitDirection;
		NiPoint4 projectileDir;
		bhkNPCollisionObject* collisionObj;
	};
	static_assert(sizeof(DamageImpactData) == 0x38);

	struct VATSCommand;
	class HitData
	{
	public:
		DamageImpactData impactData;
		int8_t gap38[8];
		uint32_t attackerHandle;
		uint32_t victimHandle;
		uint32_t sourceHandle;
		int8_t gap4C[4];
		BGSAttackData* attackData;
		BGSObjectInstance source;
		MagicItem* effect;
		SpellItem* spellItem;
		VATSCommand* VATSCommand;
		TESAmmo* ammo;
		BSTArray<BSTTuple<TESForm*, BGSTypedFormValuePair::SharedVal>>* damageTypes;
		float calculatedBaseDamage;
		float baseDamage;
		float totalDamage;
		float blockedDamage;
		float blockMult;
		float reducedDamage;
		float field_A8;
		float blockStaggerMult;
		float sneakAttackMult;
		float field_B4;
		float field_B8;
		float field_BC;
		float criticalDamageMult;
		uint32_t flags;
		BGSEquipIndex equipIndex;
		uint32_t materialType;
		int32_t bodypartType;
		int8_t gapD4[4];
	};
	static_assert(sizeof(HitData) == 0xD8);

	class TESHitEvent
	{
	public:
		HitData hitdata;
		int8_t gapD8[8];
		TESObjectREFR* victim;
		TESObjectREFR* attacker;
		BSFixedString matName;
		uint32_t sourceFormID;
		uint32_t projFormID;
		bool hasHitData;
		int8_t gapD1[7];
	};
	static_assert(sizeof(TESHitEvent) == 0x108);
}

namespace F4
{

	//https://github.com/hexabits/nifskope/blob/develop/src/data/niftypes.h

	namespace BSGraphics
	{
		struct Vertex
		{
			enum Attribute : std::uint8_t
			{
				VA_POSITION = 0x0,
				VA_TEXCOORD0 = 0x1,
				VA_TEXCOORD1 = 0x2,
				VA_NORMAL = 0x3,
				VA_BINORMAL = 0x4,
				VA_COLOR = 0x5,
				VA_SKINNING = 0x6,
				VA_LANDDATA = 0x7,
				VA_EYEDATA = 0x8,

				VA_COUNT = 9
			};

			enum Flags : std::uint16_t
			{
				VF_VERTEX = 1 << VA_POSITION,
				VF_UV = 1 << VA_TEXCOORD0,
				VF_UV_2 = 1 << VA_TEXCOORD1,
				VF_NORMAL = 1 << VA_NORMAL,
				VF_TANGENT = 1 << VA_BINORMAL,
				VF_COLORS = 1 << VA_COLOR,
				VF_SKINNED = 1 << VA_SKINNING,
				VF_LANDDATA = 1 << VA_LANDDATA,
				VF_EYEDATA = 1 << VA_EYEDATA,
				VF_FULLPREC = 0x400
			};

			enum Masks : std::uint64_t
			{
				DESC_MASK_VERT = 0xFFFFFFFFFFFFFFF0,
				DESC_MASK_UVS = 0xFFFFFFFFFFFFFF0F,
				DESC_MASK_NBT = 0xFFFFFFFFFFFFF0FF,
				DESC_MASK_SKCOL = 0xFFFFFFFFFFFF0FFF,
				DESC_MASK_DATA = 0xFFFFFFFFFFF0FFFF,
				DESC_MASK_OFFSET = 0xFFFFFF0000000000,
				DESC_MASK_FLAGS = ~(DESC_MASK_OFFSET)
			};
		};

		class VertexDesc
		{
		public:
			[[nodiscard]] bool HasFlag(Vertex::Flags a_flag) const
			{
				return ((desc >> 44) & a_flag) != 0;
			}
			void SetFlag(Vertex::Flags a_flag)
			{
				desc |= (static_cast<uint64_t>(a_flag) << 44);
			}
			void ClearFlag(Vertex::Flags a_flag)
			{
				desc &= ~(static_cast<uint64_t>(a_flag) << 44);
			}

			[[nodiscard]] std::uint32_t GetAttributeOffset(Vertex::Attribute a_attribute) const
			{
				return (desc >> (4 * static_cast<uint8_t>(a_attribute) + 2)) & 0x3C;
			}
			void SetAttributeOffset(Vertex::Attribute a_attribute, std::uint32_t a_offset)
			{
				if (a_attribute != Vertex::Attribute::VA_POSITION) {
					const uint64_t lhs = static_cast<uint64_t>(a_offset) << (4 * static_cast<uint8_t>(a_attribute) + 2);
					const uint64_t rhs = desc & ~static_cast<uint64_t>(15 << (4 * static_cast<uint8_t>(a_attribute) + 4));

					desc = lhs | rhs;
				}
			}
			void ClearAttributeOffsets()
			{
				desc &= Vertex::Masks::DESC_MASK_OFFSET;
			}

			[[nodiscard]] Vertex::Flags GetFlags() const
			{
				return static_cast<Vertex::Flags>((desc & Vertex::Masks::DESC_MASK_OFFSET) >> 44);
			}
			void SetFlags(Vertex::Flags a_flags)
			{
				desc |= (static_cast<uint64_t>(a_flags) << 44) | (desc & Vertex::Masks::DESC_MASK_FLAGS);
			}

			[[nodiscard]] std::uint32_t GetSize()
			{
				/*std::uint32_t vertexSize = 0;
				auto          flags = GetFlags();

				if (flags & Vertex::VF_VERTEX) {
					vertexSize += sizeof(std::uint16_t) * 4; //Fallout 4 uses half precision for vertex coordinates
				}
				if (flags & Vertex::VF_UV) {
					vertexSize += sizeof(std::uint16_t) * 2;
				}
				if (flags & Vertex::VF_UV_2) {
					vertexSize += sizeof(std::uint16_t) * 2;
				}
				if (flags & Vertex::VF_NORMAL) {
					vertexSize += sizeof(std::uint16_t) * 2;
					if (flags & Vertex::VF_TANGENT) {
						vertexSize += sizeof(std::uint16_t) * 2;
					}
				}
				if (flags & Vertex::VF_COLORS) {
					vertexSize += sizeof(std::uint8_t) * 4;
				}
				if (flags & Vertex::VF_SKINNED) {
					vertexSize += sizeof(std::uint16_t) * 4 + sizeof(std::uint8_t) * 4;
				}
				if (flags & Vertex::VF_EYEDATA) {
					vertexSize += sizeof(std::uint16_t);
				}

				return vertexSize;*/
				return (desc & 0xF) * 4;
			}

		private:
			// members
			std::uint64_t desc;  // 00
		};
		static_assert(sizeof(VertexDesc) == 0x8);

		struct Buffer
		{
			uint64_t unk00;
			uint64_t rawVertexData;
		};

		struct TriShape
		{
			F4::BSGraphics::VertexDesc vertexDesc;
			F4::BSGraphics::Buffer* buffer08;
			F4::BSGraphics::Buffer* buffer10;
		};

		struct Renderer
		{
			void IncRef(F4::BSGraphics::Buffer* vertexBuffer)
			{
				using func_t = decltype(&F4::BSGraphics::Renderer::IncRef);
				REL::Relocation<func_t> func{ REL::ID(1337764) };
				return func(this, vertexBuffer);
			}
			void DecRef(F4::BSGraphics::Buffer* vertexBuffer)
			{
				using func_t = decltype(&F4::BSGraphics::Renderer::DecRef);
				REL::Relocation<func_t> func{ REL::ID(194808) };
				return func(this, vertexBuffer);
			}
		};
	}

	namespace BSResource
	{
		class __declspec(novtable) StreamBase
		{
		public:
			virtual ~StreamBase() = default;  // 01
			uint32_t unk08;
			uint32_t unk0C;
		};

		class __declspec(novtable) Stream : public StreamBase
		{
		public:
			BSFixedString exePath;  //0x10
			BSFixedString prefix;   //0x18
			BSFixedString name;     //0x20
		};

		class __declspec(novtable) AsyncStream : public StreamBase
		{
		public:
			uint64_t unk10;
			BSFixedString exePath;  //0x10
			BSFixedString prefix;   //0x18
			BSFixedString name;     //0x20
		};
	}

	struct Unk
	{
		uint32_t unk00 = 0xFFFFFFFF;
		uint32_t unk04 = 0x0;
		uint32_t unk08 = 1;
	};

	struct bhkPickData
	{
	public:
		bhkPickData()
		{
			typedef bhkPickData* func_t(bhkPickData*);
			REL::Relocation<func_t> func{ REL::ID(526783) };
			func(this);
		}

		~bhkPickData()
		{
			typedef bhkPickData* func_t(bhkPickData*);
			REL::Relocation<func_t> func{ REL::ID(109425) };
			func(this);
		}

		void SetStartEnd(const RE::NiPoint3& start, const RE::NiPoint3& end)
		{
			using func_t = decltype(&F4::bhkPickData::SetStartEnd);
			REL::Relocation<func_t> func{ REL::ID(747470) };
			return func(this, start, end);
		}

		void Reset()
		{
			using func_t = decltype(&F4::bhkPickData::Reset);
			REL::Relocation<func_t> func{ REL::ID(438299) };
			return func(this);
		}

		bool HasHit()
		{
			using func_t = decltype(&F4::bhkPickData::HasHit);
			REL::Relocation<func_t> func{ REL::ID(1181584) };
			return func(this);
		}

		float GetHitFraction()
		{
			using func_t = decltype(&F4::bhkPickData::GetHitFraction);
			REL::Relocation<func_t> func{ REL::ID(476687) };
			return func(this);
		}

		int32_t GetAllCollectorRayHitSize()
		{
			using func_t = decltype(&F4::bhkPickData::GetAllCollectorRayHitSize);
			REL::Relocation<func_t> func{ REL::ID(1288513) };
			return func(this);
		}

		bool GetAllCollectorRayHitAt(uint32_t i, hknpCollisionResult& result)
		{
			using func_t = decltype(&F4::bhkPickData::GetAllCollectorRayHitAt);
			REL::Relocation<func_t> func{ REL::ID(583997) };
			return func(this, i, result);
		}

		void SortAllCollectorHits()
		{
			using func_t = decltype(&F4::bhkPickData::SortAllCollectorHits);
			REL::Relocation<func_t> func{ REL::ID(1274842) };
			return func(this);
		}

		uint8_t pad[0xE0];
		F4_HEAP_REDEFINE_ALIGNED_NEW(bhkPickData);
	};
	static_assert(sizeof(bhkPickData) == 0xE0);

	class GameUIModel
	{
	public:
		void UpdateDataModels()
		{
			using func_t = decltype(&F4::GameUIModel::UpdateDataModels);
			REL::Relocation<func_t> func{ REL::ID(1269653) };
			return func(this);
		}
	};

	class TaskQueueInterface
	{
	public:
		void QueueRebuildBendableSpline(TESObjectREFR* ref, bool rebuildCollision, NiAVObject* target)
		{
			using func_t = decltype(&F4::TaskQueueInterface::QueueRebuildBendableSpline);
			REL::Relocation<func_t> func{ REL::ID(198419) };
			return func(this, ref, rebuildCollision, target);
		}

		void QueueShow1stPerson(bool shouldShow)
		{
			using func_t = decltype(&F4::TaskQueueInterface::QueueShow1stPerson);
			REL::Relocation<func_t> func{ REL::ID(994377) };
			return func(this, shouldShow);
		}

		void QueueUpdate3D(TESObjectREFR* ref, uint32_t unk = 0)
		{
			using func_t = decltype(&F4::TaskQueueInterface::QueueUpdate3D);
			REL::Relocation<func_t> func{ REL::ID(581890) };
			return func(this, ref, unk);
		}

		void QueueWeaponFire(TESObjectWEAP* wep, TESObjectREFR* shooter, BGSEquipIndex index, TESAmmo* ammo)
		{
			using func_t = decltype(&F4::TaskQueueInterface::QueueWeaponFire);
			REL::Relocation<func_t> func{ REL::ID(15449) };
			return func(this, wep, shooter, index, ammo);
		}
	};

	namespace BSJobs
	{
		class JobList
		{
		public:
			typedef void (*Job)(void*);

			void AddJobToList(Job job, void* arg, const char* name)
			{
				using func_t = decltype(&F4::BSJobs::JobList::AddJobToList);
				REL::Relocation<func_t> func{ REL::ID(546314) };
				return func(this, job, arg, name);
			}

			void InsertSyncPoint(uint32_t synctype)
			{
				using func_t = decltype(&F4::BSJobs::JobList::InsertSyncPoint);
				REL::Relocation<func_t> func{ REL::ID(625755) };
				return func(this, synctype);
			}

			void Submit(void* local, bool b)
			{
				using func_t = decltype(&F4::BSJobs::JobList::Submit);
				REL::Relocation<func_t> func{ REL::ID(970923) };
				return func(this, local, b);
			}
		};
	};

	namespace JobListManager
	{
		REL::Relocation<BSJobs::JobList**> ptr_pPostPhysicsUpdateJobList{ REL::ID(1183305) };
	}

	namespace BSUtilities
	{
		inline NiAVObject* GetObjectByName(NiAVObject* root, const BSFixedString& name, bool tryInternal, bool dontAttach)
		{
			using func_t = decltype(&GetObjectByName);
			REL::Relocation<func_t> func{ REL::ID(843650) };
			return func(root, name, tryInternal, dontAttach);
		}
	}

	namespace CombatUtilities
	{
		inline bool CalculateProjectileTrajectory(const NiPoint3& pos, const NiPoint3& vel, float gravity, const NiPoint3& targetPos, float X, NiPoint3& out)
		{
			using func_t = decltype(&CalculateProjectileTrajectory);
			REL::Relocation<func_t> func{ REL::ID(1575156) };
			return func(pos, vel, gravity, targetPos, X, out);
		}

		inline bool CalculateProjectileLOS(Actor* a, BGSProjectile* proj, float speed, const NiPoint3* launchPos, const NiPoint3* targetPos, NiPoint3* hitPos, TESObjectREFR** collidee, float* dist)
		{
			typedef bool func_t(Actor*, BGSProjectile*, float, const NiPoint3*, const NiPoint3*, NiPoint3*, TESObjectREFR**, float*);
			REL::Relocation<func_t> func{ REL::ID(798616) };
			return func(a, proj, speed, launchPos, targetPos, hitPos, collidee, dist);
		}

		inline bhkNPCollisionObject* CalculateProjectileLOS(Actor* a, BGSProjectile* proj, bhkPickData& pick)
		{
			typedef bhkNPCollisionObject* func_t(Actor*, BGSProjectile*, bhkPickData&);
			REL::Relocation<func_t> func{ REL::ID(55339) };
			return func(a, proj, pick);
		}
		REL::Relocation<float> fWorldGravity{ REL::ID(1378547) };
	};

	namespace AnimationSystemUtils
	{
		inline bool WillEventChangeState(const TESObjectREFR& ref, const BSFixedString& evn)
		{
			using func_t = decltype(&WillEventChangeState);
			REL::Relocation<func_t> func{ REL::ID(35074) };
			return func(ref, evn);
		}
	}

	namespace BGSAnimationSystemUtils
	{
		inline bool InitializeActorInstant(Actor& a, bool b)
		{
			using func_t = decltype(&InitializeActorInstant);
			REL::Relocation<func_t> func{ REL::ID(672857) };
			return func(a, b);
		}
	};

	class BSTGlobalEvent
	{
	public:
		virtual ~BSTGlobalEvent();

		template <typename T>
		class EventSource
		{
		public:
			virtual ~EventSource();

			// void ** _vtbl;                           // 00
			uint64_t unk08;         // 08
			BSTEventSource<T> src;  // 10
		};

		// void ** _vtbl;                               // 00
		uint64_t unk08;                              // 08
		uint64_t unk10;                              // 10
		BSTArray<EventSource<void*>*> eventSources;  // 18
	};

	bool PlaySound(BGSSoundDescriptorForm* sndr, NiPoint3 pos, NiAVObject* node)
	{
		typedef bool* func_t(Unk, BGSSoundDescriptorForm*, NiPoint3, NiAVObject*);
		REL::Relocation<func_t> func{ REL::ID(376497) };
		Unk u;
		return func(u, sndr, pos, node);
	}

	void ShakeCamera(float mul, NiPoint3 origin, float duration, float strength)
	{
		using func_t = decltype(&F4::ShakeCamera);
		REL::Relocation<func_t> func{ REL::ID(758209) };
		return func(mul, origin, duration, strength);
	}

	void ApplyImageSpaceModifier(TESImageSpaceModifier* imod, float strength, NiAVObject* target)
	{
		using func_t = decltype(&F4::ApplyImageSpaceModifier);
		REL::Relocation<func_t> func{ REL::ID(179769) };
		return func(imod, strength, target);
	}

	TESObjectREFR* PlaceAtMe_Native(BSScript::IVirtualMachine* vm, uint32_t stackId, TESObjectREFR** target, TESForm* form, int32_t count, bool bForcePersist, bool bInitiallyDisabled, bool bDeleteWhenAble)
	{
		using func_t = decltype(&F4::PlaceAtMe_Native);
		REL::Relocation<func_t> func{ REL::ID(984532) };
		return func(vm, stackId, target, form, count, bForcePersist, bInitiallyDisabled, bDeleteWhenAble);
	}

	void MoveRefrToPosition(TESObjectREFR* source, uint32_t* pTargetHandle, TESObjectCELL* parentCell, TESWorldSpace* worldSpace, NiPoint3* position, NiPoint3* rotation)
	{
		using func_t = decltype(&F4::MoveRefrToPosition);
		REL::Relocation<func_t> func{ REL::ID(1332434) };
		return func(source, pTargetHandle, parentCell, worldSpace, position, rotation);
	}

	REL::Relocation<BSTGlobalEvent**> g_globalEvents{ REL::ID(1424022) };

	REL::Relocation<TaskQueueInterface**> ptr_TaskQueueInterface{ REL::ID(7491) };

	REL::Relocation<DWORD*> ptr_hkMemoryRouterTlsIndex{ REL::ID(878080) };

	REL::Relocation<uint32_t*> ptr_invalidhandle{ REL::ID(888641) };

	class ProcessLists;
	REL::Relocation<ProcessLists*> ptr_processLists{ REL::ID(474742) };

	REL::Relocation<BSGraphics::Renderer**> ptr_gRenderer{ REL::ID(1378294) };

	REL::Relocation<float*> ptr_engineTime{ REL::ID(599343) };

	REL::Relocation<float*> ptr_deltaTime{ REL::ID(1013228) };

	REL::Relocation<NiPoint3*> ptr_k1stPersonCameraLocation{ REL::ID(1304276) };

	REL::Relocation<NiPoint3A*> ptr_PlayerAdjust{ REL::ID(988646) };

	REL::Relocation<GameUIModel**> ptr_GameUIModel{ REL::ID(17419) };
}

char tempbuf[8192] = { 0 };
char* _MESSAGE(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsnprintf(tempbuf, sizeof(tempbuf), fmt, args);
	va_end(args);
	spdlog::log(spdlog::level::warn, tempbuf);

	return tempbuf;
}

void Dump(const void* mem, unsigned int size)
{
	const char* p = static_cast<const char*>(mem);
	unsigned char* up = (unsigned char*)p;
	std::stringstream stream;
	int row = 0;
	for (unsigned int i = 0; i < size; i++) {
		stream << std::setfill('0') << std::setw(2) << std::hex << (int)up[i] << " ";
		if (i % 8 == 7) {
			stream << "\t0x"
				   << std::setw(2) << std::hex << (int)up[i]
				   << std::setw(2) << (int)up[i - 1]
				   << std::setw(2) << (int)up[i - 2]
				   << std::setw(2) << (int)up[i - 3]
				   << std::setw(2) << (int)up[i - 4]
				   << std::setw(2) << (int)up[i - 5]
				   << std::setw(2) << (int)up[i - 6]
				   << std::setw(2) << (int)up[i - 7] << std::setfill('0');
			stream << "\t0x" << std::setw(2) << std::hex << row * 8 << std::setfill('0');
			_MESSAGE("%s", stream.str().c_str());
			stream.str(std::string());
			row++;
		}
	}
}

void GetShaderFlags(uint64_t flag)
{
	std::string str;
	for (int i = 0; i < 64; ++i) {
		uint64_t f = 1i64 << i;
		if ((flag & f) == f) {
			if (str.length() != 0) {
				str += " ";
			}
			str += std::to_string(i);
		}
	}
	_MESSAGE("Shader Flag : %s", str.c_str());
}

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


ActorValueInfo* GetAVIFByEditorID(std::string editorID)
{
	TESDataHandler* dh = TESDataHandler::GetSingleton();
	BSTArray<ActorValueInfo*> avifs = dh->GetFormArray<ActorValueInfo>();
	for (auto it = avifs.begin(); it != avifs.end(); ++it) {
		if (strcmp((*it)->formEditorID.c_str(), editorID.c_str()) == 0) {
			return (*it);
		}
	}
	return nullptr;
}

BGSExplosion* GetExplosionByFullName(std::string explosionname)
{
	TESDataHandler* dh = TESDataHandler::GetSingleton();
	BSTArray<BGSExplosion*> explosions = dh->GetFormArray<BGSExplosion>();
	for (auto it = explosions.begin(); it != explosions.end(); ++it) {
		if (strcmp((*it)->GetFullName(), explosionname.c_str()) == 0) {
			return (*it);
		}
	}
	return nullptr;
}

SpellItem* GetSpellByFullName(std::string spellname)
{
	TESDataHandler* dh = TESDataHandler::GetSingleton();
	BSTArray<SpellItem*> spells = dh->GetFormArray<SpellItem>();
	for (auto it = spells.begin(); it != spells.end(); ++it) {
		if (strcmp((*it)->GetFullName(), spellname.c_str()) == 0) {
			return (*it);
		}
	}
	return nullptr;
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

const char* GetObjectClassNameImpl(const char* result, void* objBase)
{
	using namespace RTTI;
	void** obj = (void**)objBase;
	CompleteObjectLocator** vtbl = (CompleteObjectLocator**)obj[0];
	CompleteObjectLocator* rtti = vtbl[-1];
	RVA<TypeDescriptor> typeDesc = rtti->typeDescriptor;

	// starts with ,?
	const char* _name = typeDesc->mangled_name();
	if ((_name[0] == '.') && (_name[1] == '?')) {
		// is at most 100 chars long
		for (uint32_t i = 0; i < 100; i++) {
			if (_name[i] == 0) {
				// remove the .?AV
				return _name + 4;
				break;
			}
		}
	}
	return result;
}

const char* GetObjectClassName(void* objBase)
{
	const char* result = "<no rtti>";
	__try {
		result = GetObjectClassNameImpl(result, objBase);
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		// return the default
	}

	return result;
}

NiNode* CreateBone(const char* name)
{
	NiNode* newbone = new NiNode(0);
	newbone->name = name;
	return newbone;
}

bool CheckPA(Actor* a)
{
	if (!a->extraList) {
		return false;
	}
	return a->extraList->HasType(EXTRA_DATA_TYPE::kPowerArmor);
	;
}

float GetActorScale(Actor* a)
{
	typedef float (*_GetPlayerScale)(Actor*);
	REL::Relocation<_GetPlayerScale> func{ REL::ID(911188) };
	return func(a);
}

void ReloadWeaponGraph(BSExtraData* animGraphPreload, Actor& a_actor)
{
	typedef void (*_ReloadWeaponGraph)(BSExtraData*, Actor&);
	REL::Relocation<_ReloadWeaponGraph> func{ REL::ID(393711) };
	return func(animGraphPreload, a_actor);
}

bool Visit(NiAVObject* obj, std::vector<std::string> &namelist, const std::function<bool(NiAVObject*)>& functor)
{
	if (functor(obj))
		return true;

	NiPointer<NiNode> node(obj->IsNode());
	if (node) {
		for (auto it = node->children.begin(); it != node->children.end(); ++it) {
			NiPointer<NiAVObject> object(*it);
			if (object) {
				namelist.push_back(object->name.c_str());
				if (Visit(object.get(), namelist, functor))
					return true;
			}
		}
	}

	return false;
}

class RopeDelegate : public F4SE::ITaskDelegate
{
public:
	TESObjectREFREx* rope;
	NiPoint3 targetPos;
	TESObjectCELL* targetCell;
	TESWorldSpace* targetWorld;
	RopeDelegate(TESObjectREFR* ref, NiPoint3 pos, TESObjectCELL* cell, TESWorldSpace* world)
	{
		rope = (TESObjectREFREx*)ref;
		targetPos = pos;
		targetCell = cell;
		targetWorld = world;
	}
	virtual void Run() override
	{
		uint32_t nullHandle = *F4::ptr_invalidhandle;
		NiPoint3 rot;
		F4::MoveRefrToPosition(rope, &nullHandle, targetCell, targetWorld, &targetPos, &rot);
	}
};

//void MoveBendableSpline(TESObjectREFR* rope, NiPoint3 start, NiPoint3 end, TESObjectCELL* cell, TESWorldSpace* world, bool useQueue = true)
//{
//	F4::TaskQueueInterface* queue = *F4::ptr_TaskQueueInterface;
//
//	uint32_t nullHandle = *F4::ptr_invalidhandle;
//	NiPoint3 rot;
//	NiPoint3 ropePos = (start + end) / 2.f;
//	F4::MoveRefrToPosition(rope, &nullHandle, cell, world, &ropePos, &rot);
//
//	float thickness = 1.0f;
//	float slack = 0.f;
//	ExtraBendableSplineParams* splineParams = (ExtraBendableSplineParams*)rope->extraList->GetByType(EXTRA_DATA_TYPE::kBendableSplineParams);
//	if (splineParams) {
//		thickness = splineParams->thickness;
//	}
//	NiPoint3 offset = (start - end) / 2.f;
//	rope->extraList->SetBendableSplineInfo(&thickness, &slack, &offset);
//
//	if (useQueue) {
//		queue->QueueRebuildBendableSpline(rope, true, nullptr);
//	} else {
//		((TESObjectREFREx*)rope)->RebuildBendableSpline(false, nullptr);
//	}
//	//F4SE::GetTaskInterface()->AddTask(new RopeDelegate(rope, ropePos, cell, world));
//}

std::string SplitString(const std::string str, const std::string delimiter, std::string& remainder)
{
	std::string ret;
	size_t i = str.find(delimiter);
	if (i == std::string::npos) {
		ret = str;
		remainder = "";
		return ret;
	}

	ret = str.substr(0, i);
	remainder = str.substr(i + 1);
	return ret;
}

#pragma endregion
