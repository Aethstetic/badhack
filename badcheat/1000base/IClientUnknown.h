#pragma once
#include "IClientNetworkable.h"
#include "IClientRenderable.h"
#include "IClientThinkable.h"

class IClientAlphaProperty;
class C_BaseEntity;
class IClientEntity;

class ICollideable
{
public:
	virtual void pad0();
	virtual Vector&				OBBMins() const = 0;
	virtual Vector&				OBBMaxs() const = 0;
};

class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable*              GetCollideable() = 0;
	virtual IClientNetworkable*        GetClientNetworkable() = 0;
	virtual IClientRenderable*         GetClientRenderable() = 0;
	virtual IClientEntity*             GetIClientEntity() = 0;
	virtual C_BaseEntity*              GetBaseEntity() = 0;
	virtual IClientThinkable*          GetClientThinkable() = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
};