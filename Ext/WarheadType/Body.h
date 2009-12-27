#ifndef WARHEADTYPE_EXT_H
#define WARHEADTYPE_EXT_H

#include <xcompile.h>
#include <AnimClass.h>
#include <IonBlastClass.h>
#include <AnimTypeClass.h>
#include <BulletClass.h>
#include <CaptureManagerClass.h>
#include <CCINIClass.h>
#include <CellSpread.h>
#include <EMPulseClass.h>
#include <MapClass.h>
#include <TechnoClass.h>
#include <WarheadTypeClass.h>

#include <Conversions.h>

#include "../_Container.hpp"

#include "../../Utilities/Template.h"

#ifdef DEBUGBUILD
#include "../../Misc/Debug.h"
#endif

//class WarheadTypeExt;
class WarheadTypeExt //: public Container<WarheadTypeExt>
{
public:
	typedef WarheadTypeClass TT;

	struct VersesData {
		double Verses;
		bool ForceFire;
		bool Retaliate;
		bool PassiveAcquire;

		bool operator ==(const VersesData &RHS) const {
			return (CLOSE_ENOUGH(this->Verses, RHS.Verses));
		};
	};

	class ExtData : public Extension<TT>
	{
	public:
		bool MindControl_Permanent;

		int Ripple_Radius;

		int EMP_Duration;

		int IC_Duration;

		DynamicVectorClass<VersesData> Verses;
		double DeployedDamage;

		Customizable<AnimTypeClass *> Temporal_WarpAway;

		ExtData(const DWORD Canary, TT* const OwnerObject) : Extension<TT>(Canary, OwnerObject),
			MindControl_Permanent (false),
			Ripple_Radius (0),
			EMP_Duration (0),
			IC_Duration (0),
			DeployedDamage (1.00),
			Temporal_WarpAway (&RulesClass::Global()->WarpAway)
			{
				for(int i = 0; i < 11; ++i) {
					VersesData vs = {1.00, 1, 1, 1};
					Verses.AddItem(vs);
				}
			};

		virtual ~ExtData() { };

		virtual size_t Size() const { return sizeof(*this); };

		virtual void LoadFromINIFile(TT *pThis, CCINIClass *pINI);
	};

	static Container<WarheadTypeExt> ExtMap;

	// evil hack
	static WarheadTypeClass *Temporal_WH;

	static hash_map<IonBlastClass *, WarheadTypeExt::ExtData *> IonExt;

	static void PointerGotInvalid(void *ptr);
/*
	EXT_P_DECLARE(WarheadTypeClass);
	EXT_FUNCS(WarheadTypeClass);
	EXT_INI_FUNCS(WarheadTypeClass);
*/
};

typedef hash_map<IonBlastClass *, WarheadTypeExt::ExtData *> hash_ionExt;
#endif
