#include "Listener.h"

CGameEvents::WeaponFireListener weapon_fire_listener;
CGameEvents::PlayerHurtListener player_hurt_listener;
CGameEvents::BulletImpactListener bullet_impact_listener;
CGameEvents::PlayerDeathListener player_death_listener;
CGameEvents::RoundEndListener round_end_listener;
CGameEvents::RoundStartListener round_start_listener;

void CGameEvents::WeaponFireListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

	g_MissedShots->EventCallback(game_event);
}

void CGameEvents::PlayerHurtListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

	g_MissedShots->EventCallback(game_event);
}

void CGameEvents::BulletImpactListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

	g_MissedShots->EventCallback(game_event);
}

void CGameEvents::RoundStartListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

	g_MissedShots->EventCallback(game_event);
}

void CGameEvents::RoundEndListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

}

void CGameEvents::PlayerDeathListener::FireGameEvent(IGameEvent *game_event)
{
	if (!I::Engine->IsInGame() || I::Engine->IsConnected())
		return;

	if (!game_event)
		return;

}

void CGameEvents::InitializeEventListeners() 
{
	I::GameEvent->AddListener(&weapon_fire_listener, "weapon_fire", false);
	I::GameEvent->AddListener(&player_hurt_listener, "player_hurt", false);
	I::GameEvent->AddListener(&bullet_impact_listener, "bullet_impact", false);
	I::GameEvent->AddListener(&player_death_listener, "player_death", false);
	I::GameEvent->AddListener(&round_end_listener, "round_end", false);
	I::GameEvent->AddListener(&round_start_listener, "round_start", false);
}
