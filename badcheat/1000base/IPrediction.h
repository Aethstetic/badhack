#pragma once

class CMoveData
{
public:
	bool    m_bFirstRunOfFunctions : 1;
	bool    m_bGameCodeMovedPlayer : 1;
	int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
	int     m_nImpulseCommand;      // Impulse command issued.
	Vector  m_vecViewAngles;        // Command view angles (local space)
	Vector  m_vecAbsViewAngles;     // Command view angles (world space)
	int     m_nButtons;             // Attack buttons.
	int     m_nOldButtons;          // From host_client->oldbuttons;
	float   m_flForwardMove;
	float   m_flSideMove;
	float   m_flUpMove;
	float   m_flMaxSpeed;
	float   m_flClientMaxSpeed;
	Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
	Vector  m_vecAngles;            // edict::angles
	Vector  m_vecOldAngles;
	float   m_outStepHeight;        // how much you climbed this move
	Vector  m_outWishVel;           // This is where you tried 
	Vector  m_outJumpVel;           // This is your jump velocity
	Vector  m_vecConstraintCenter;
	float   m_flConstraintRadius;
	float   m_flConstraintWidth;
	float   m_flConstraintSpeedFactor;
	float   m_flUnknown[5];
	Vector  m_vecAbsOrigin;
};

class IPrediction {
public:
	virtual void UnknownVirtual0() = 0;
	virtual void UnknownVirtual1() = 0;
	virtual void UnknownVirtual2() = 0;
	virtual void UnknownVirtual3() = 0;
	virtual void UnknownVirtual4() = 0;
	virtual void UnknownVirtual5() = 0;
	virtual void UnknownVirtual6() = 0;
	virtual void UnknownVirtual7() = 0;
	virtual void UnknownVirtual8() = 0;
	virtual void UnknownVirtual9() = 0;
	virtual void UnknownVirtual10() = 0;
	virtual void UnknownVirtual11() = 0;
	virtual void UnknownVirtual12() = 0;
	virtual void UnknownVirtual13() = 0;
	virtual void UnknownVirtual14() = 0;
	virtual void UnknownVirtual15() = 0;
	virtual void UnknownVirtual16() = 0;
	virtual void UnknownVirtual17() = 0;
	virtual void UnknownVirtual18() = 0;
	virtual void UnknownVirtual19() = 0;
	virtual void SetupMove(C_BaseEntity* player, CUserCmd* ucmd, void* pHelper, void* move) = 0; //20
	virtual void FinishMove(C_BaseEntity* player, CUserCmd* ucmd, void* move) = 0;
};