#ifndef TECHNO_EXT_H
#define TECHNO_EXT_H

#include <xcompile.h>
#include <algorithm>

#include <AircraftClass.h>
#include <CellSpread.h>
#include <HouseClass.h>
#include <InfantryClass.h>
#include <MapClass.h>
#include <Randomizer.h>
#include <TemporalClass.h>
#include <WeaponTypeClass.h>
#include <WarheadTypeClass.h>
#include <AlphaShapeClass.h>
#include <SpotlightClass.h>

#include "../WarheadType/Body.h"
#include "../WeaponType/Body.h"
#include "../TechnoType/Body.h"

#include "../_Container.hpp"

//#include "../../Misc/JammerClass.h"
class JammerClass;
class PoweredUnitClass;

class TechnoExt
{
public:
	typedef TechnoClass TT;

	class ExtData : public Extension<TT>
	{
		mutable TechnoTypeExt::TT * TypeData;
		mutable TechnoTypeExt::ExtData * TypeExt;
	public:
		// weapon slots fsblargh
		BYTE idxSlot_Wave;
		BYTE idxSlot_Beam;
		BYTE idxSlot_Warp;
		BYTE idxSlot_Parasite;

		bool Survivors_Done;

		TimerStruct CloakSkipTimer;
		SHPStruct * Insignia_Image;

		BuildingClass *GarrisonedIn; // when infantry garrisons a building, we need a fast way to find said building when damage forwarding kills it

		AnimClass *EMPSparkleAnim;
		eMission EMPLastMission;

		bool ShadowDrawnManually;

		// 305 Radar Jammers
		JammerClass* RadarJam;
		
		// issue #617 powered units
		PoweredUnitClass* PoweredUnit;

		ExtData(const DWORD Canary, TT* const OwnerObject) : Extension<TT>(Canary, OwnerObject),
			TypeData (NULL),
			TypeExt(NULL),
			idxSlot_Wave (0),
			idxSlot_Beam (0),
			idxSlot_Warp (0),
			idxSlot_Parasite(0),
			Survivors_Done (0),
			Insignia_Image (NULL),
			GarrisonedIn (NULL),
			EMPSparkleAnim (NULL),
			EMPLastMission (mission_None),
			ShadowDrawnManually (false),
			RadarJam(NULL),
			PoweredUnit(NULL)
			{
				this->CloakSkipTimer.Stop();
				// hope this works with the timing - I assume it does, since Types should be created before derivates thereof
				//TechnoTypeExt::ExtData* TypeExt = TechnoTypeExt::ExtMap.Find(this->AttachedToObject->GetTechnoType());
				/*if(TypeExt->RadarJamRadius) {
					RadarJam = new JammerClass(this->AttachedToObject, this); // now in hooks -> TechnoClass_Update_CheckOperators
				}*/
			};

		virtual ~ExtData() {
			//delete RadarJam; // now in hooks -> TechnoClass_Remove
		};

		virtual size_t Size() const { return sizeof(*this); };

		virtual void InvalidatePointer(void *ptr) {
			AnnounceInvalidPointer(this->GarrisonedIn, ptr);
		}

		bool IsOperated();
		bool IsPowered();

		unsigned int AlphaFrame(SHPStruct * Image);

		bool DrawVisualFX();

		UnitTypeClass * GetUnitType();

		TechnoTypeExt::TT * GetTypeData() const {
			if(!this->TypeData) {
				this->TypeData = this->AttachedToObject->GetTechnoType();
			}
			return this->TypeData;
		}

		TechnoTypeExt::ExtData * GetTypeExt() const {
			if(!this->TypeExt) {
				this->TypeExt = TechnoTypeExt::ExtMap.Find(this->GetTypeData());
			}
			return this->TypeExt;
		}

	protected:
		template<typename T>
		T GetBountyValue(Nullable<T> BountyClass::*pMember) const;

	public:
		bool Get_Bounty_Message() const;
		bool Get_Bounty_FriendlyMessage() const;
		double Get_Bounty_Modifier() const;
		double Get_Bounty_FriendlyModifier() const;
		// #1523 also Money Conversion -> Pillage
		bool Get_Bounty_Pillager() const;
		double Get_Bounty_CostMultiplier() const;
		double Get_Bounty_PillageMultiplier() const;

	};

	static Container<TechnoExt> ExtMap;
	static hash_map<ObjectClass *, AlphaShapeClass *> AlphaExt;
	static hash_map<TechnoClass *, BuildingLightClass *> SpotlightExt;

	static BuildingLightClass * ActiveBuildingLight;

	static FireError::Value FiringStateCache;

	static bool NeedsRegap;

	static void SpawnSurvivors(FootClass *pThis, TechnoClass *pKiller, bool Select, bool IgnoreDefenses);
	static bool EjectSurvivor(FootClass *Survivor, CoordStruct *loc, bool Select);
	static void EjectPassengers(FootClass *, signed short);
	static void GetPutLocation(CoordStruct const &, CoordStruct &);

	static void StopDraining(TechnoClass *Drainer, TechnoClass *Drainee);

	static bool CreateWithDroppod(FootClass *Object, CoordStruct *XYZ);
/*
	static int SelectWeaponAgainst(TechnoClass *pThis, TechnoClass *pTarget);
	static bool EvalWeaponAgainst(TechnoClass *pThis, TechnoClass *pTarget, WeaponTypeClass* W);
	static float EvalVersesAgainst(TechnoClass *pThis, TechnoClass *pTarget, WeaponTypeClass* W);
*/
};

typedef hash_map<ObjectClass *, AlphaShapeClass *> hash_AlphaExt;
typedef hash_map<TechnoClass *, BuildingLightClass *> hash_SpotlightExt;
#endif
