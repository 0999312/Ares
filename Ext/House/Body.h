#ifndef HOUSE_EXT_H
#define HOUSE_EXT_H

#include "..\_Container.hpp"
#include <Helpers\Macro.h>
#include <Helpers\Template.h>

#include <FactoryClass.h>
#include <HouseClass.h>
#include <ScenarioClass.h>
#include <SuperClass.h>

class HouseExt
{
	public:
	typedef HouseClass TT;
	
	class ExtData : public Extension<TT> 
	{
		public:
			bool IonSensitive;
			bool FirewallActive;
			
			BuildingClass *Factory_BuildingType;
			BuildingClass *Factory_InfantryType;
			BuildingClass *Factory_VehicleType;
			BuildingClass *Factory_NavyType;
			BuildingClass *Factory_AircraftType;
			
		ExtData(const DWORD Canary = 0) : 
			IonSensitive(0),
			FirewallActive(0),
			Factory_BuildingType(NULL),
			Factory_InfantryType(NULL),
			Factory_VehicleType(NULL),
			Factory_NavyType(NULL),
			Factory_AircraftType(NULL)
		{
		};

		virtual ~ExtData() {

		}

		virtual size_t Size() const { return sizeof(*this); };
		
	};

	static Container<HouseExt> ExtMap;

	static HouseClass *SavingNow;
	static IStream *SavingStream;

	static signed int BuildLimitRemaining(HouseClass *pHouse, TechnoTypeClass *pItem);
	static signed int CheckBuildLimit(HouseClass *pHouse, TechnoTypeClass *pItem, bool IncludeQueued);

	static signed int RequirementsMet(HouseClass *pHouse, TechnoTypeClass *pItem);
	static bool PrerequisitesMet(HouseClass *pHouse, TechnoTypeClass *pItem);

	static signed int PrereqValidate
		(HouseClass *pHouse, TechnoTypeClass *pItem, bool BuildLimitOnly, bool IncludeQueued);

};

#endif
