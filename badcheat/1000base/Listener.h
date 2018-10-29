#pragma once

#include "Main.h"

#define CREATE_EVENT_LISTENER(class_name)\
class class_name : public IGameEventListener\
{\
public:\
	~class_name() { I::GameEvent->RemoveListener(this); }\
\
	virtual void FireGameEvent(IGameEvent* game_event);\
};

namespace CGameEvents
{
	void InitializeEventListeners();

	CREATE_EVENT_LISTENER(WeaponFireListener);
	CREATE_EVENT_LISTENER(PlayerHurtListener);
	CREATE_EVENT_LISTENER(BulletImpactListener);
	CREATE_EVENT_LISTENER(PlayerDeathListener);
	CREATE_EVENT_LISTENER(RoundStartListener);
	CREATE_EVENT_LISTENER(RoundEndListener);
};
