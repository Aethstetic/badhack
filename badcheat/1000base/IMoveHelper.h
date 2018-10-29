#pragma once

class IMoveHelper
{
private:
	virtual void UnknownVirtual() = 0;
public:
	virtual void SetHost(C_BaseEntity* host) = 0;
};