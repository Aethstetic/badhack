#pragma once

class CUtlVectorSimple;
class C_BaseCombatWeapon;

class CSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* hud_name;				//0x0088 
	char* weapon_name;			//0x008C 
	char _0x0090[60];
	__int32 type;				//0x00CC 
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	__int32 damage;				//0x00F0 
	float armor_ratio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float penetration;			//0x00FC 
	char _0x0100[8];
	float range;				//0x0108 
	float range_modifier;		//0x010C 
	char _0x0110[16];
	BYTE silencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};// Size=0x01A8

class C_BaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
private:
	template<class T>
	T GetValue(const int offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	T& SetValue(uintptr_t offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	

public:

	C_BaseCombatWeapon* GetActiveWeapon();
	
	bool IsKnifeorNade();

	bool IsKnife();

	bool IsNade();

	bool& ClientSideAnimation()
	{
		static int m_bClientSideAnimation = g_pNetvars->GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		return SetValue<bool>(m_bClientSideAnimation);
	}

	void UpdateClientAnimation()
	{
		g_Utils->get_vfunc<void(__thiscall*)(void*)>(this, 218)(this);
	}

	C_BasePlayerAnimState* GetAnimState()
	{
		return *(C_BasePlayerAnimState**)(PVOID)((DWORD)(this) + 0x3884);
	}

	void SetAnimState(C_BasePlayerAnimState* State)
	{
		*(C_BasePlayerAnimState**)(PVOID)((DWORD)(this) + 0x3884) = State;
	}

	void SetAbsAngles(Vector angles);
	void SetAbsOrigin(Vector ArgOrigin);

	void InvalidateBoneCache();

	void InvalidatePhysicsRecursive(int nChangeFlags);

	CUtlVectorSimple* GetAnimOverlayStruct();

	AnimationLayer* GetAnimOverlay(int i);

	int GetNumAnimOverlays();

	int GetSequenceActivity(int sequence);

	float GetSimulationTime()
	{
		static int m_flSimulationTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
		return GetValue<float>(m_flSimulationTime);
	}

	float GetAnimTime()// dosnt work
	{
		static int m_flAnimTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flAnimTime");
		return GetValue<float>(m_flAnimTime);
	}

	float GetOldSimulationTime()
	{
		static int m_flOldSimulationTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
		return GetValue<float>(m_flOldSimulationTime + 4);
	}

	float GetLowerBodyYaw()
	{
		static int m_flLowerBodyYawTarget = g_pNetvars->GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		return GetValue<float>(m_flLowerBodyYawTarget);
	}

	int GetTickBase()
	{
		static int m_nTickBase = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
		return GetValue<int>(m_nTickBase);
	}

	Vector GetAimPunchAngle()
	{
		return *reinterpret_cast<Vector*>(uintptr_t(this) + uintptr_t(0x301C));
	}

	bool GetLifeState()
	{
		static int m_lifeState = g_pNetvars->GetOffset("DT_BasePlayer", "m_lifeState");
		return GetValue<bool>(m_lifeState);
	}

	bool GetHeavyArmor()
	{
		static int m_bHasHeavyArmor = g_pNetvars->GetOffset("DT_CSPlayer", "m_bHasHeavyArmor");
		return GetValue<bool>(m_bHasHeavyArmor);
	}

	int GetHealth()
	{
		static int m_iHealth = g_pNetvars->GetOffset("DT_BasePlayer", "m_iHealth");
		return GetValue<int>(m_iHealth);
	}

	bool IsScoped()
	{
		return *reinterpret_cast<bool*>(uintptr_t(this) + uintptr_t(0x388E));
		//static int m_bIsScoped = g_pNetvars->GetOffset("DT_CSPlayer", "m_bIsScoped");
		//return GetValue<bool>(m_bIsScoped);
	}

	int ArmorValue()
	{
		static int m_ArmorValue = g_pNetvars->GetOffset("DT_CSPlayer", "m_ArmorValue");
		return GetValue<int>(m_ArmorValue);
	}

	std::array<float, 24> &m_flPoseParameter()
	{
		return *(std::array<float, 24>*)((uintptr_t)this + (uintptr_t)0x2764);
	}

	bool HasHelmet()
	{
		static int m_bHasHelmet = g_pNetvars->GetOffset("DT_CSPlayer", "m_bHasHelmet");
		return GetValue<bool>(m_bHasHelmet);
	}

	char* GetArmorName()
	{
		if (ArmorValue() > 0)
		{
			if (HasHelmet())
				return "hk";
			else
				return "k";
		}
		else
			return " ";
	}

	bool IsAlive() 
	{ 
		return this->GetHealth() > 0 && this->GetLifeState() == 0; 
	}

	Vector GetOrigin()
	{
		static int m_vecOrigin = g_pNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
		return GetValue<Vector>(m_vecOrigin);
	}

	bool HasImmunity()
	{
		static int m_bGunGameImmunity = g_pNetvars->GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
		return GetValue<bool>(m_bGunGameImmunity);
	}

	int GetTeam()
	{
		static int m_iTeamNum = g_pNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
		return GetValue<int>(m_iTeamNum);
	}

	Vector GetEyeAngles()
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset("DT_CSPlayer", "m_angEyeAngles");
		return GetValue<Vector>(m_angEyeAngles);
	}

	void SetEyeAngles(Vector Angle)
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset("DT_CSPlayer", "m_angEyeAngles");
		*reinterpret_cast<Vector*>(uintptr_t(this) + m_angEyeAngles) = Angle;
	}

	Vector GetViewOffset()
	{
		static int m_vecViewOffset = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
		return GetValue<Vector>(m_vecViewOffset);
	}

	Vector GetVelocity()
	{
		static int m_vecVelocity = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
		return GetValue<Vector>(m_vecVelocity);
	}

	ICollideable* GetCollideable()
	{
		return (ICollideable*)((DWORD)this + 0x318);
	}

	Vector GetAbsOrigin() {
		__asm {
			MOV ECX, this
			MOV EAX, DWORD PTR DS : [ECX]
			CALL DWORD PTR DS : [EAX + 0x28]
		}
	}

	EntityFlags &GetFlags()
	{
		static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
		return *reinterpret_cast<EntityFlags*>(uintptr_t(this) + m_fFlags);
	}

	template<class T>
	void SetFlags(EntityFlags flags)
	{
		static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + m_fFlags) = flags;
	}

	Vector GetEyePosition(void)
	{
		return GetOrigin() + GetViewOffset();
	}

	int HitboxSet()
	{
		return *reinterpret_cast<int*>(uintptr_t(this) + uintptr_t(0x9FC));

		//static int m_nHitboxSet = g_pNetvars->GetOffset("DT_BasePlayer", "m_nHitboxSet");
		//return GetValue<int>(m_nHitboxSet);
	}

	Vector GetHitboxPosition(int Hitbox, matrix3x4_t matrix[]);
};

typedef CSWeaponInfo& (__thiscall* GetCSWpnDataFn)(void*);

class C_BaseCombatWeapon : public C_BaseEntity
{
private:
	template<class T>
	T GetValue(const int offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
	}

public:

	float GetInaccuracy()
	{
		typedef float(__thiscall* oInaccuracy)(PVOID);
		return g_Utils->get_vfunc< oInaccuracy >(this, 467)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall* oWeaponSpread)(PVOID);
		return g_Utils->get_vfunc< oWeaponSpread >(this, 436)(this);
	}

	void AccuracyPenalty()
	{
		typedef void(__thiscall *OrigFn)(void *);
		return g_Utils->get_vfunc<OrigFn>(this, 468)(this);
	}

	CSWeaponInfo* GetCSWpnData()
	{
		if (!this)
			return nullptr;
		typedef CSWeaponInfo*(__thiscall* tGetCSWpnData)(void*);
		return g_Utils->get_vfunc<tGetCSWpnData>(this, 444)(this);
	}

	std::string GetName()
	{
		return std::string(this->GetCSWpnData()->weapon_name);
	}

	int GetAmmo()
	{
		return *reinterpret_cast<int*>(uintptr_t(this) + uintptr_t(0x3234));
		//static int m_iClip1 = g_pNetvars->GetOffset("DT_WeaponCSBase", "m_iClip1");
		//return GetValue<int>(m_iClip1);
	}

	float NextPrimaryAttack()
	{
		static int m_flNextPrimaryAttack = g_pNetvars->GetOffset("CBaseCombatWeapon", "m_flNextPrimaryAttack");
		return GetValue<float>(m_flNextPrimaryAttack);
	}

	float GetNextAttack()
	{
		return *reinterpret_cast<float*>(uintptr_t(this) + uintptr_t(0x2D60));
	}

	int GetItemDefenitionIndex()
	{
		return *reinterpret_cast<short*>(uintptr_t(this) + uintptr_t(0x2F9A));
	}

	float GetPostponeFireReadyTime()
	{
		return *reinterpret_cast<float*>(uintptr_t(this) + uintptr_t(0x32C4));
	}

	float GetNextPrimaryAttack()
	{
		return *reinterpret_cast<float*>(uintptr_t(this) + uintptr_t(0x3208));
	}

	bool C_BaseCombatWeapon::IsReloading();
	bool C_BaseCombatWeapon::CanFire();
};

struct SpatializationInfo_t;
class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void             Release(void) = 0;
	virtual const Vector     GetAbsOrigin(void) const = 0;
	virtual const QAngle     GetAbsAngles(void) const = 0;
	virtual void*            GetMouth(void) = 0;
	virtual bool             GetSoundSpatialization(SpatializationInfo_t info) = 0;
	virtual bool             IsBlurred(void) = 0;
};

class IClientEntityList
{
public:
	virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
	virtual void*                 vtablepad0x1(void) = 0;
	virtual void*                 vtablepad0x2(void) = 0;
	virtual C_BaseEntity*        GetClientEntity(int entNum) = 0;
	virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int                   GetHighestEntityIndex(void) = 0;
	virtual void                  SetMaxEntities(int maxEnts) = 0;
	virtual int                   GetMaxEntities() = 0;
};