#include "Body.h"
#include "../BuildingType/Body.h"
#include <BulletClass.h>
#include <LaserDrawClass.h>

DEFINE_HOOK(44B2FE, BuildingClass_Mi_Attack_IsPrism, 6)
{
	GET(BuildingClass *, B, ESI);
	GET(int, idxWeapon, EBP); //which weapon was chosen to attack the target with
	R->EAX<BuildingTypeClass *>(B->Type);

	enum { IsPrism = 0x44B310, IsNotPrism = 0x44B630, IsCustomPrism = 0x44B6D6};

	BuildingTypeClass *pMasterType = B->Type;
	BuildingTypeExt::ExtData *pMasterTypeData = BuildingTypeExt::ExtMap.Find(pMasterType);

	if (pMasterTypeData->PrismForwarding.Enabled == BuildingTypeExt::cPrismForwarding::YES
		|| pMasterTypeData->PrismForwarding.Enabled == BuildingTypeExt::cPrismForwarding::ATTACK) {

		Debug::Log("PrismForwarding: Setting up a new Master tower\n");
		if (B->PrismStage == pcs_Idle) {
			B->PrismStage = pcs_Master;
			B->DelayBeforeFiring = B->Type->DelayedFireDelay;
			B->PrismTargetCoords.X = 0;
			B->PrismTargetCoords.Y = B->PrismTargetCoords.Z = 0;
			B->DestroyNthAnim(BuildingAnimSlot::Active);
			B->PlayNthAnim(BuildingAnimSlot::Special);

			int LongestChain = 0;

			//set up slaves
			int NetworkSize = 0;
			int stage = 0;

			//when it reaches zero we can't acquire any more slaves
			Debug::Log("PrismForwarding: initial multistage calling with stage %d\n", stage);
			while (BuildingTypeExt::cPrismForwarding::AcquireSlaves_MultiStage(B, B, stage++, 0, &NetworkSize, &LongestChain) != 0) {}

			//now we have all the towers we know the longest chain, and can set all the towers' charge delays
			BuildingTypeExt::cPrismForwarding::SetPrismChargeDelay(B);

		} else if (B->PrismStage == pcs_Slave) {
			Debug::Log("PrismForwarding: Converting Slave to Master\n");
			//a slave tower is changing into a master tower at the last second
			B->PrismStage = pcs_Master;
			BuildingExt::ExtData *pMasterData = BuildingExt::ExtMap.Find(B);
			pMasterData->PrismForwarding.SupportTarget = NULL;

		}

		return IsCustomPrism; //always custom, the new code is a complete rewrite of the old code

	}

	return IsNotPrism;
}

DEFINE_HOOK(447FAE, BuildingClass_GetObjectActivityState, 6)
{
	GET(BuildingClass *, B, ESI);
	enum { BusyCharging = 0x447FB8, NotBusyCharging = 0x447FC3};

	Debug::Log("PrismForwarding: GOAS\n");
	if(B->DelayBeforeFiring > 0) {
		Debug::Log("PrismForwarding: GOAS->DBF\n");
		//if this is a slave prism tower, then it might still be able to become a master tower at this time
		BuildingTypeClass *pType = B->Type;
		BuildingTypeExt::ExtData *pTypeData = BuildingTypeExt::ExtMap.Find(pType);
		if (pTypeData->PrismForwarding.Enabled == BuildingTypeExt::cPrismForwarding::YES
				|| pTypeData->PrismForwarding.Enabled == BuildingTypeExt::cPrismForwarding::ATTACK) {
			//is a prism tower
			Debug::Log("PrismForwarding: GOAS->DBF->IsPrism\n");
			if (B->PrismStage == pcs_Slave && pTypeData->PrismForwarding.BreakSupport) {
				Debug::Log("PrismForwarding: GOAS->DBF->IsPrism->NotBusy\n");
				return NotBusyCharging;
			}
		}
		Debug::Log("PrismForwarding: GOAS->DBF->Busy\n");
		return BusyCharging;
	}
	Debug::Log("PrismForwarding: GOAS->NotBusy\n");
	return NotBusyCharging;
}

//NB: PrismTargetCoords is not a coord struct, it's some kind of garbage whose first dword is the used weapon index and two others are undefined...
DEFINE_HOOK(4503F0, BuildingClass_Update_Prism, 9)
{
	GET(BuildingClass *, pThis, ECX);
	if(int PrismStage = pThis->PrismStage) {
		BuildingExt::ExtData *pData = BuildingExt::ExtMap.Find(pThis);
		if (pData->PrismForwarding.PrismChargeDelay <= 0) {
			--pThis->DelayBeforeFiring;
			if(pThis->DelayBeforeFiring <= 0) {
				if(PrismStage == pcs_Slave) {
					Debug::Log("[Prismforwarding] Slave ready to fire.");
					if (BuildingClass *pTarget = pData->PrismForwarding.SupportTarget) {
						Debug::Log("[Prismforwarding] Slave has a SupportTarget.");
						BuildingExt::ExtData *pTargetData = BuildingExt::ExtMap.Find(pTarget);
						BuildingTypeClass *pType = pThis->Type;
						BuildingTypeExt::ExtData *pTypeData = BuildingTypeExt::ExtMap.Find(pType);
						pTargetData->PrismForwarding.ModifierReserve += (pTypeData->PrismForwarding.SupportModifier + pData->PrismForwarding.ModifierReserve);
						pTargetData->PrismForwarding.DamageReserve += (pTypeData->PrismForwarding.DamageAdd  + pData->PrismForwarding.DamageReserve);
						Debug::Log("[Prismforwarding] Slave laser firing.");
						pThis->FireLaser(pThis->PrismTargetCoords);

					}
				}
				if(PrismStage == pcs_Master) {
					if(ObjectClass *Target = pThis->Target) {
						if(pThis->GetFireError(Target, pThis->PrismTargetCoords.X, true) == FireError::OK) {
							if(BulletClass *LaserBeam = pThis->Fire(Target, pThis->PrismTargetCoords.X)) {
								BuildingTypeClass *pType = pThis->Type;
								BuildingTypeExt::ExtData *pTypeData = BuildingTypeExt::ExtMap.Find(pType);
								double DamageMult = pTypeData->PrismForwarding.SupportModifier + pData->PrismForwarding.ModifierReserve;
								LaserBeam->DamageMultiplier = ((DamageMult + 100) * 256) / 100; //apparently this is divided by 256 elsewhere
								LaserBeam->Health += pTypeData->PrismForwarding.DamageAdd  + pData->PrismForwarding.DamageReserve;
							}
						}
					}
				}
				//This tower's job is done. Go idle.
				pData->PrismForwarding.ModifierReserve = 0.0;
				pData->PrismForwarding.DamageReserve = 0;
				pData->PrismForwarding.Senders.Clear();
				pThis->SupportingPrisms = 0; //Ares doesn't actually use this, but maintaining it anyway (as direct feeds only)
				pData->PrismForwarding.SupportTarget = NULL;
				pThis->PrismStage = pcs_Idle;
			}
		} else {
			//still in delayed charge so not actually charging yet
			--pData->PrismForwarding.PrismChargeDelay;
			pThis->DestroyNthAnim(BuildingAnimSlot::Active);
			pThis->PlayNthAnim(BuildingAnimSlot::Special);
		}
	}
	return 0x4504E2;
}

DEFINE_HOOK(44ABD0, BuildingClass_FireLaser, 5)
{
	GET(BuildingClass *, B, ECX);
	LEA_STACK(CoordStruct *, pTargetXYZ, 0x4);

	BuildingTypeClass *pType = B->Type;
	BuildingTypeExt::ExtData *pTypeData = BuildingTypeExt::ExtMap.Find(pType);

	CoordStruct SourceXYZ, Base = {0, 0, 0};
	B->GetFLH(&SourceXYZ, 0, Base);

	ColorStruct blank(0, 0, 0);

	LaserDrawClass * LaserBeam;
	Debug::Log("[Prismforwarding] allocing laser draw class.");
	GAME_ALLOC(LaserDrawClass, LaserBeam, SourceXYZ, *pTargetXYZ, B->Owner->LaserColor, blank, blank, pTypeData->PrismForwarding.SupportDuration);

	if(LaserBeam) {
		LaserBeam->IsHouseColor = true;
		LaserBeam->field_1C = 3;
	}

	B->SupportingPrisms = 0;
	B->ReloadTimer.Start(pTypeData->PrismForwarding.SupportDelay);

	return 0x44ACE2;
}

