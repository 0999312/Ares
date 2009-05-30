#include "RMG.h"
#include "..\UI\registered.h"

bool RMG::UrbanAreas = 0;
bool RMG::UrbanAreasRead = 0;

//0x596FFE
DEFINE_HOOK(596FFE, RMG_EnableArchipelago, 0)
{
	R->set_EBP(0);						//start at index 0 instead of 1
	R->set_EBX(0x82B034);				//set the list offset to "TXT_MAP_ARCHIPELAGO"
	return 0x597008;
}

//0x5970EA
DEFINE_HOOK(5970EA, RMG_EnableDesert, 9)
{
	HWND hWnd=(HWND)R->get_EDI();

	//List the item
	LRESULT result=
		SendMessageA(
		hWnd,
		WW_CB_ADDITEM,		//CUSTOM BY WESTWOOD
		0,
		(LPARAM)StringTable::LoadString("TXT_BIOME_DESERT"));

	//Set the item data
	SendMessageA(
		hWnd,
		CB_SETITEMDATA,
		result,
		3);			//Make it actually be desert

	return 0;
}

DEFINE_HOOK(596C81, MapSeedClass_DialogFunc_GetData, 5)
{
	GET(HWND, hDlg, EBP);
	HWND hDlgItem = GetDlgItem(hDlg, ARES_CHK_RMG_URBAN_AREAS);
	if(hDlgItem) {
		RMG::UrbanAreas = (1 == SendMessageA(hDlgItem, BM_GETCHECK, 0, 0));
	}
	return 0;
}

DEFINE_HOOK(5982D5, MapSeedClass_LoadFromINI, 6)
{
	if(!RMG::UrbanAreasRead) {
		GET(CCINIClass *, pINI, EDI);
		RMG::UrbanAreas = pINI->ReadBool("General", "GenerateUrbanAreas", RMG::UrbanAreas);
		RMG::UrbanAreasRead = 1;
	}
	return 0;
}

DEFINE_HOOK(598FB8, RMG_GenerateUrban, 5)
{
	if(RMG::UrbanAreas) {
		void* pMapSeed = (void*)R->get_ESI();
		SET_REG32(ecx, pMapSeed);
		CALL(0x5A5020);
	}
	return 0;
}

DEFINE_HOOK(59000E, RMG_FixPavedRoadEnd_Bridges_North, 0)
{
	return 0x590087;
}

DEFINE_HOOK(5900F7, RMG_FixPavedRoadEnd_Bridges_South, 0)
{
	return 0x59015E;
}

DEFINE_HOOK(58FCC6, RMG_FixPavedRoadEnd_Bridges_West, 0)
{
	return 0x58FD2A;
}

DEFINE_HOOK(58FBDD, RMG_FixPavedRoadEnd_Bridges_East, 0)
{
	return 0x58FC55;
}

DEFINE_HOOK(58FA51, RMG_PlaceWEBridge, 6)
{
	RectangleStruct* pRect = (RectangleStruct*)((BYTE*)R->get_ESP()+0x14);

	//it's a WE bridge
	return (pRect->Width > pRect->Height)
	 ? 0x58FA73
	 : 0;
}

DEFINE_HOOK(58FE7B, RMG_PlaceNSBridge, 8)
{
	RectangleStruct* pRect = (RectangleStruct*)((BYTE*)R->get_ESP()+0x14);

	//it's a NS bridge
	return (pRect->Height > pRect->Width)
	 ? 0x58FE91
	 : 0;
}
