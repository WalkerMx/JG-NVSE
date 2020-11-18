#include "..\..\nvse\nvse\ScriptUtils.h"
#include "internal/decoding.h"
#include <Windows.h>
#include <unordered_map>
#pragma once

NVSEArrayVarInterface* ArrIfc = NULL;
NVSEStringVarInterface* StrIfc = NULL;
NVSEMessagingInterface* g_msg = NULL;
NVSEScriptInterface* g_script = NULL;
NVSECommandTableInterface* CmdIfc = NULL;
InventoryRef* (*InventoryRefGetForID)(UInt32 refID);
void DisableMuzzleFlashLightsHook();
float(*GetWeaponDPS)(ActorValueOwner* avOwner, TESObjectWEAP* weapon, float condition, UInt8 arg4, ContChangesEntry* entry, UInt8 arg6, UInt8 arg7, int arg8, float arg9, float arg10, UInt8 arg11, UInt8 arg12, TESForm* ammo) =
(float(*)(ActorValueOwner*, TESObjectWEAP*, float, UInt8, ContChangesEntry*, UInt8, UInt8, int, float, float, UInt8, UInt8, TESForm*))0x645380;
bool isShowLevelUp = true;
bool bArrowKeysDisabled = false;
char* StrArgBuf;
#define ExtractArgsEx(...) g_script->ExtractArgsEx(__VA_ARGS__)
#define ExtractFormatStringArgs(...) g_script->ExtractFormatStringArgs(__VA_ARGS__)
#define IS_TYPE(form, type) (*(UInt32*)form == kVtbl_##type)
#define NOT_ID(form, type) (form->typeID != kFormType_##type)
#define IS_ID(form, type) (form->typeID == kFormType_##type)
#define REG_CMD(name) nvse->RegisterCommand(&kCommandInfo_##name);
#define REG_TYPED_CMD(name, type) nvse->RegisterTypedCommand(&kCommandInfo_##name,kRetnType_##type);
#define VarNameSize 64

IDebugLog ParamLog;
bool loadEditorIDs = 0;
bool fixHighNoon = 0;
TESSound* questFailSound = 0;
TESSound* questNewSound = 0;
TESSound* questCompeteSound = 0;
TESSound* locationDiscoverSound = 0;
std::unordered_map<UInt32, char*> CustomMapMarkerMap;

double GetVectorAngle2D(NiPoint3* pt)
{
	double angle;
	if (pt->y == 0)
	{
		if (pt->x <= 0)
		{
			angle = kDblPIx3d2;
		}
		else
		{
			angle = kDblPId2;
		}
	}
	else
	{
		double ratio = pt->x / pt->y;
		angle = dAtan(ratio);
		if (pt->y < 0.0)
		{
			angle += kDblPI;
		}
	}

	return angle;
}


double GetAngleBetweenPoints(NiPoint3* actorPos, NiPoint3* playerPos, float offset)
{
	NiPoint3 diff;
	diff.Init(actorPos);
	diff.Subtract(playerPos);

	double angle = GetVectorAngle2D(&diff) - offset;
	if (angle > -kDblPI)
	{
		if (angle > kDblPI)
		{
			angle = kDblPIx2 - angle;
		}
	}
	else
	{
		angle += kDblPIx2;
	}
	return angle * 57.295779513;
}
namespace SpecialCaseEDIDs {
	void Handle();
}
TESActorBase* Actor::GetActorBase()
{
	ExtraLeveledCreature* xLvlCre = GetExtraType(extraDataList, LeveledCreature);
	return (xLvlCre && xLvlCre->form) ? (TESActorBase*)xLvlCre->form : (TESActorBase*)baseForm;
}


char** DefaultMarkers = (char**) 0x11A0404;

char* __fastcall hk_GetMapMarker(TESObjectREFR* thisObj, UInt16 MapMarkerType)
{
	
	auto it = CustomMapMarkerMap.find(thisObj->refID);
	if (it != CustomMapMarkerMap.end()) return it->second;
	return DefaultMarkers[MapMarkerType];
}
CommandTable::CommandTable() { }
CommandTable::~CommandTable() { }
void CommandTable::Dump(void)
{
	for (CommandList::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter)
	{
		_MESSAGE("%08X %04X %s %s", iter->opcode, iter->needsParent, iter->longName, iter->shortName);
		gLog.Indent();

#if 0
		for (UInt32 i = 0; i < iter->numParams; i++)
		{
			ParamInfo* param = &iter->params[i];
			_DMESSAGE("%08X %08X %s", param->typeID, param->isOptional, param->typeStr);
		}
#endif

		gLog.Outdent();
	}
}
__declspec (naked) void AsmGetMapMarkerRoute()
{
	//UInt32 static const retAddr = 0x079D337;
	__asm
	{
		mov edx, eax
		mov ecx, [ebp-0x24]
		jmp hk_GetMapMarker
	}
}


__declspec(naked) void Tile::SetFloat(UInt32 id, float fltVal, bool bPropagate)
{
	static const UInt32 procAddr = 0xA012D0;
	__asm	jmp		procAddr
}

__declspec(naked) float ExtraContainerChanges::EntryData::GetItemHealthPerc(bool arg1)
{
	static const UInt32 procAddr = 0x4BCDB0;
	__asm	jmp		procAddr
}
__declspec(naked) ContChangesEntry* ExtraContainerChanges::EntryDataList::FindForItem(TESForm* item)
{
	__asm
	{
		mov		edx, [esp + 4]
		listIter:
		mov		eax, [ecx]
			test	eax, eax
			jz		listNext
			cmp[eax + 8], edx
			jz		done
			listNext :
		mov		ecx, [ecx + 4]
			test	ecx, ecx
			jnz		listIter
			xor eax, eax
			done :
		retn	4
	}
}

float __declspec(naked) __fastcall NiNodeComputeDistance(NiVector3* Vector1, NiVector3* Vector2)
{
	__asm
	{
		movd xmm0, [ecx]
		subss xmm0, [edx]
		mulss xmm0, xmm0
		movd xmm1, [ecx + 4]
		subss xmm1, [edx + 4]
		mulss xmm1, xmm1
		movd xmm2, [ecx + 8]
		subss xmm2, [edx + 8]
		mulss xmm2, xmm2
		addss xmm0, xmm1
		addss xmm0, xmm2
		sqrtss xmm0, xmm0
		movd eax, xmm0
		push eax
		fld dword ptr[esp]
		add esp, 4
		ret
	}
}
NiNode* NiNode::GetNode(const char* nodeName)
{
	NiAVObject* found = GetBlock(nodeName);
	return found ? found->GetNiNode() : NULL;
}
NiNode* TESObjectREFR::GetNode(const char* nodeName)
{
	NiNode* rootNode = GetNiNode();
	return rootNode ? (*nodeName ? rootNode->GetNode(nodeName) : rootNode) : NULL;
}
hkpRigidBody* TESObjectREFR::GetRigidBody(const char* nodeName)
{
	NiNode* rootNode = GetNiNode();
	if (rootNode)
	{
		NiNode* targetNode = rootNode->GetNode(nodeName);
		if (targetNode && targetNode->m_collisionObject)
		{
			bhkWorldObject* hWorldObj = targetNode->m_collisionObject->worldObj;
			if (hWorldObj)
			{
				hkpRigidBody* rigidBody = (hkpRigidBody*)hWorldObj->refObject;
				UInt8 motionType = rigidBody->motion.type;
				if ((motionType == 2) || (motionType == 3) || (motionType == 6))
					return rigidBody;
			}
		}
	}
	return NULL;
}
__forceinline void NiPointAssign(float& xIn, float& yIn, float& zIn)
{
	NiPointBuffer->x = xIn;
	NiPointBuffer->y = yIn;
	NiPointBuffer->z = zIn;
}
UInt32 DoSkipMuzzleLights = -1;

__declspec (naked) void DisableMuzzleFlashLightsHook()
{
	static const UInt32 retAddrDisable = 0x9BB843;
	static const UInt32 retAddrKeep = 0x9BB81A;
	__asm
	{
		cmp DoSkipMuzzleLights, 1
		jz Skip
		push 1
		mov ecx, [ebp - 4]
		jmp retAddrKeep
		Skip :
		jmp retAddrDisable
	}
}
void DoCustomMapMarker(TESObjectREFR* Marker, char* PathToPass)
{

	auto Position = CustomMapMarkerMap.find(Marker->refID);
	char* MapMarkerAllocString = new char[strlen(PathToPass) + 1];
	strcpy(MapMarkerAllocString, PathToPass);

	if (Position != CustomMapMarkerMap.end())
	{
		delete[] Position->second;
		Position->second = MapMarkerAllocString;
	}
	else
	{
		CustomMapMarkerMap.insert({ Marker->refID, MapMarkerAllocString });
	}
}
__declspec(naked) ExtraContainerChanges::EntryDataList* TESObjectREFR::GetContainerChangesList()
{
	__asm
	{
		push	kExtraData_ContainerChanges
		add		ecx, 0x44
		call	BaseExtraList::GetByType
		test	eax, eax
		jz		done
		mov		eax, [eax + 0xC]
		test	eax, eax
		jz		done
		mov		eax, [eax]
		done:
		retn
	}
}
UInt8 TESForm::GetOverridingModIdx()
{
	ModInfo* info = mods.GetLastItem();
	return info ? info->modIndex : 0xFF;
}
_declspec(naked) void LevelUpHook() {
	static const UInt32 noShowAddr = 0x77D903;
	static const UInt32 showAddr = 0x77D618;
	_asm {
		jne noLevelUp
		cmp dword ptr ds : [isShowLevelUp] , 0
		je noLevelUp
		jmp showAddr
		noLevelUp :
		jmp noShowAddr
	}
}

__declspec(naked) void GetNameHook() {
	__asm jmp TESForm::hk_GetName
}
__declspec(naked) void SetEditorIdHook() {
	__asm jmp TESForm::hk_SetEditorId
}
__declspec(naked) void SetEditorIdHook_REFR()
{
	__asm jmp TESForm::hk_SetEditorID_REFR
}
const char* __fastcall ConsoleNameHook(TESObjectREFR* ref) {
	try {
		const char* name = ref->baseForm->GetTheName();
		if (!strlen(name)) name = ref->baseForm->GetName();
		return name;
	}
	catch (...) {
		_MESSAGE("Couldn't retrieve EDID for %u", ref->refID);
	}
	return "";
}
void LoadEditorIDs() {
	WriteRelCall(0x486903, (UInt32(GetNameHook))); // replaces empty string with editor id in TESForm::GetDebugName
	WriteRelCall(0x71B748, UInt32(ConsoleNameHook)); // replaces empty string with editor id in selected ref name in console
	WriteRelCall(0x710BFC, UInt32(ConsoleNameHook));
	WriteRelCall(0x55D498, (UInt32(GetNameHook))); // replaces empty string with editor id in TESObjectREFR::GetDebugName
	SafeWrite16(0x467A12, 0x3AEB); // loads more types in game's editor:form map
	for (uint32_t i = 0; i < ARRAYSIZE(TESForm_Vtables); i++)
	{
		if (*(uintptr_t*)(TESForm_Vtables[i] + 0x130) == 0x00401280)
			SafeWrite32(TESForm_Vtables[i] + 0x130, (UInt32)GetNameHook);

		if (*(uintptr_t*)(TESForm_Vtables[i] + 0x134) == 0x00401290)
			SafeWrite32(TESForm_Vtables[i] + 0x134, (UInt32)SetEditorIdHook);
	}
	for (uint32_t i = 0; i < ARRAYSIZE(TESForm_REFR_Vtables); i++)
	{
		if (*(uintptr_t*)(TESForm_REFR_Vtables[i] + 0x130) == 0x00401280)
			SafeWrite32(TESForm_REFR_Vtables[i] + 0x130, (UInt32)GetNameHook);

		if (*(uintptr_t*)(TESForm_REFR_Vtables[i] + 0x134) == 0x00401290)
			SafeWrite32(TESForm_REFR_Vtables[i] + 0x134, (UInt32)SetEditorIdHook_REFR);
	}



	SpecialCaseEDIDs::Handle();

}
bool __fastcall CheckForHighNoon(Sky* sky)
{
	return (float)(sky->firstClimate->sunriseEnd) / 6.0 < sky->gameHour && sky->gameHour < (float)(sky->firstClimate->sunsetBegin) / 6.0;
}

__declspec (naked) void HookforIMOD1()
{
	static const UInt32 retaddress = 0x063F575;
	__asm {
		mov ecx, dword ptr ds : [0x11DEA20]
		call CheckForHighNoon
		test al, al
		mov ecx, [ebp - 0x7C]
		jz HandleNormal
		mov ecx, [ecx + 0x120]
		jmp RETURN
		HandleNormal :
		mov ecx, [ecx + 0x11C]
			RETURN :
			jmp retaddress
	}
}

__declspec (naked) void HookforIMOD2()
{
	static const UInt32 retaddress = 0x063F5F6;

	__asm {
		mov ecx, dword ptr ds : [0x11DEA20]
		call CheckForHighNoon
		test al, al
		mov ecx, [ebp - 0x7C]
		jz HandleNormal
		mov ecx, [ecx + 0x11C]
		jmp RETURN
		HandleNormal :
		mov ecx, [ecx + 0x120]
			RETURN :
			jmp retaddress
	}
}
TESForm* __fastcall GetAmmoInInventory(TESObjectWEAP* weap) {
	if (weap->ammo.ammo) {
		if (IS_TYPE(weap->ammo.ammo, BGSListForm)) {
			BGSListForm* ammoList = (BGSListForm*)weap->ammo.ammo;
			PlayerCharacter* g_thePlayer = PlayerCharacter::GetSingleton();
			ExtraContainerChanges* xChanges = GetExtraType(g_thePlayer->extraDataList, ContainerChanges);
			TESForm* ammo = 0;
			for (int i = 0; i < ammoList->Count(); i++) {
				ammo = ammoList->GetNthForm(i);
				UInt32 count = ThisStdCall<UInt32>(0x4C8F30, xChanges->data, ammo);
				if (count > 0) return ammo;
			}
		}
	}
	return 0;
}
__declspec(naked) void InventoryAmmoHook() {
	static const UInt32 retnAddr = 0x7080A8;
	__asm {
		mov ecx, dword ptr[ebp - 0x2D4]
		call GetAmmoInInventory
		mov dword ptr[ebp - 0x2C8], eax
		jmp retnAddr
	}
}
NiAVObject* NiNode::GetBlock(const char* blockName)
{
	if (StrEqualCI(m_blockName, blockName))
		return this;
	NiAVObject* found = NULL;
	for (NiTArray<NiAVObject*>::Iterator iter(m_children); !iter.End(); ++iter)
	{
		if (!*iter) continue;
		if (iter->GetNiNode())
			found = ((NiNode*)* iter)->GetBlock(blockName);
		else if (StrEqualCI(iter->m_blockName, blockName))
			found = *iter;
		else continue;
		if (found) break;
	}
	return found;
}
NiAVObject* TESObjectREFR::GetNiBlock(const char* blockName)
{
	NiNode* rootNode = GetNiNode();
	return rootNode ? rootNode->GetBlock(blockName) : NULL;
}



__declspec(naked) void OnCloseContainerHook()
{
	static const UInt32 retnAddr = 0x75B240;
	__asm
	{
		push    ecx
		mov     ecx, [ecx + 0x74]
		test    ecx, ecx
		jz      done
		mov     eax, [ecx]
		call    dword ptr[eax + 0x100]
		test    al, al
		jz      done
		mov     word ptr[ecx + 0x7C], 1
		done:
		pop     ecx
			jmp     retnAddr
	}
}
void patchFixDisintegrationsStat()
{
	// check if user has Unnecessary Tweaks or lStewieAl's Tweaks installed
	if (*(UInt8*)0x8A1B4D == 0xE9 || *(UInt8*)0x8A1B51 == 0x26) return;
	// critical stage 1, jump to checks for IncPCMiscStat
	SafeWrite8(0x8A1B51, 0x26); // change JE offset

	// critical stage 3, jump to checks for IncPCMiscStat
	SafeWriteBuf(0x8A1B1B, "\xF9\x01\x74\x12\x83\xF9\x02\x74\x30\x83\xF9\x04\x74\x2B\x83\xF9\x03\x74\x4A\xEB\x22\x90", 22);

	// critical stages 2 and 4, skip IncPCMiscStat
	SafeWriteBuf(0x8A1B6E, "\x82\xC0\x01\x00\x00\xFF\xD0\xEB\x53\x90", 10);
}
static float value = 15.0;
float* __fastcall VATSSpreadMultHook(void* ecx) {
	return &value;
}
__declspec(naked) void DialogueAnimHook() {
	static const UInt32 jumpAddr = 0x8A56DF;
	static const UInt32 retnAddr = 0x8A566B;
	__asm {
		movzx eax, byte ptr ss : [ebp + 0x10]
		test eax, eax
		jnz ANIM
		jmp retnAddr
		ANIM:

		jmp jumpAddr
	}
}
__declspec(naked) void DisableArrowKeysHook() {
	static const UInt32 retnAddr = 0x70F711;
	__asm {
		cmp byte ptr [bArrowKeysDisabled], 1
		jnz DONE
		cmp dword ptr [ebp+8], 4
		jnz MATCHED
		cmp dword ptr[ebp + 8], 3
		jnz MATCHED
		cmp dword ptr[ebp + 8], 1
		jnz MATCHED
		cmp dword ptr[ebp + 8], 2
		jnz MATCHED
		jmp DONE
		MATCHED:
			mov dword ptr[ebp+8], 0
		DONE:
			mov byte ptr[ebp-0xD], 0
			mov eax, 1
			jmp retnAddr
	}
}
__declspec(naked) void FixNPCIncrementingChallenges() {
	static const UInt32 retnAddr = 0x88D0D8;
	static const UInt32 noIncrementAddr = 0x88D100;
	__asm {
		mov eax, [ebp-0x80]
		cmp eax, dword ptr ds:[0x11DEA3C]
		jz INCREMENT
		jmp noIncrementAddr
		INCREMENT:
		push 0
		push 0
		push 0
		push 0
		jmp retnAddr
	}
}
void __fastcall PlayQuestFailSound(Sound* sound, int dummy) {
	tList<QuestUpdateManager>* g_questUpdateManager = (tList <QuestUpdateManager>*)0x11D970C;
	if (g_questUpdateManager) {
		ListNode<QuestUpdateManager>* iter = g_questUpdateManager->Head();
		do {
			switch (iter->data->updateType) {
			case QuestAdded:
				if (questNewSound) sound = &Sound(questNewSound->refID, 0x121);
				break;
			case QuestCompleted:
				if (questCompeteSound) sound = &Sound(questCompeteSound->refID, 0x121);
				break;
			case QuestFailed:
				if (questFailSound) sound = &Sound(questFailSound->refID, 0x121);
				break;
			case LocationDiscovered:
				if (locationDiscoverSound) sound = &Sound(locationDiscoverSound->refID, 0x121);
				break;
			}
			sound->Play();
		} while (iter = iter->next);
	}
}
static void PatchMemoryNop(ULONG_PTR Address, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE*)(Address + i) = 0x90; //0x90 == opcode for NOP

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

void HandleGameHooks()
{
	WriteRelJump(0x70809E, (UInt32)InventoryAmmoHook); // use available ammo in inventory instead of NULL when default ammo isn't present
	WriteRelJump(0xC5244A, (UInt32)NiCameraGetAltHook);
	WriteRelJump(0x77D612, UInt32(LevelUpHook)); // for ToggleLevelUpMenu
	SafeWrite32(0x10721AC, (UInt32)OnCloseContainerHook);
	WriteRelJump(0x9BB815, (UInt32)DisableMuzzleFlashLightsHook); // for DisableMuzzleFlashLights
	SafeWrite16(0x79D330, 0x9090);
	WriteRelCall(0x79D332, (UInt32)AsmGetMapMarkerRoute);
	WriteRelCall(0x8B102B, (UInt32)VATSSpreadMultHook); // replace fNPCMaxWobbleAngle with 15.0 for VATS
	PatchMemoryNop(0x8A56C4, 4); // Fix for animations not working in dialog topics with sound
	PatchMemoryNop(0x8A56C8, 4);
	WriteRelJump(0x70F708, (UInt32)DisableArrowKeysHook);
	patchFixDisintegrationsStat();
	WriteRelJump(0x88D0D0, (UInt32)FixNPCIncrementingChallenges);
	WriteRelCall(0x77A8E9, (UInt32)PlayQuestFailSound);
	if (loadEditorIDs) LoadEditorIDs();
}


bool removeFiles(char* folder1)
{
	char folder[MAX_PATH];
	char filename[MAX_PATH];
	strcpy(folder, folder1);
	strcat(folder, "/*.*");
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(folder, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				sprintf(filename, "%s\\%s", folder1, fd.cFileName);
				remove(filename);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
		RemoveDirectory(folder1);
	}
	return 1;
}

TESRegionDataWeather* GetWeatherData(TESRegion* region) {
	ListNode<TESRegionData>* iter = region->dataEntries->Head();
	TESRegionData* regData;
	do
	{
		regData = iter->data;
		if ((*(UInt32*)regData == 0x1023E18))
			return (TESRegionDataWeather*)regData;
	} while (iter = iter->next);
	return NULL;
}