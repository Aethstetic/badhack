#include "Main.h"

float render_alpha[64];
float bomb_timer;
bool bomb_planted;

void Visuals::SetupDraw() {
	//	local drawing
	ApplyNightmode();

	//	player related rendering
	for (int i = 1; i <= I::Globals->maxClients; i++)
	{
		auto entity = I::EntityList->GetClientEntity(i);

		if (!entity)
			continue;

		if (!g_Globals->LocalPlayer)
			return;

		if (entity == g_Globals->LocalPlayer)
			continue;

		if (entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
			continue;

		if ((entity->IsDormant() || entity->GetHealth() <= 0) && render_alpha[entity->EntIndex()] > 0)
			render_alpha[entity->EntIndex()] -= 5;
		else if (render_alpha[entity->EntIndex()] < 210 && !(entity->IsDormant() || entity->GetHealth() <= 0))
			render_alpha[entity->EntIndex()] += 5;

		if (!entity->IsAlive() || entity->IsDormant() && render_alpha[entity->EntIndex()] <= 0) //	 hackerman
			continue;

		RenderPlayers(entity, render_alpha[entity->EntIndex()]);
	}

	//	other entity related rendering
	/*for (int i = 64; i <= I::EntityList->GetHighestEntityIndex(); ++i)
	{

	}*/
}

std::string FixNetworkedName(std::string name) //	kinda ghetto but w/e tbh
{
	std::string cname = name;

	if (cname[0] == 'C')
		cname.erase(cname.begin());

	auto start_of_weap = cname.find("Weapon");
	if (start_of_weap != std::string::npos)
		cname.erase(cname.begin() + start_of_weap, cname.begin() + start_of_weap + 6);

	return cname;
}

std::string StringUpper(std::string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

	return strToConvert;
}

void Visuals::RenderPlayers(C_BaseEntity* entity, float alpha, matrix3x4_t* bone_matrix)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 15);

	int screen_x, screen_y;
	I::Engine->GetScreenSize(screen_x, screen_y);

	if (g_Utils->WorldToScreen(pos3D, pos) && g_Utils->WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;

		//--- Player Name ---//
		if (g_Vars->Visuals.Name)
		{
			PlayerInfo_t ent_info;
			if (!I::Engine->GetPlayerInfo(entity->EntIndex(), &ent_info))
				return;

			std::string name = ent_info.szName;
			I::Surface->DrawT(pos.x, top.y - 14, Color(255, 255, 255, alpha), g_Globals->Namefont, true, name.c_str());
		}

		//--- Player Box ---//
		if (g_Vars->Visuals.Box)
		{
			I::Surface->DrawSetColor(Color(255, 255, 255, alpha));
			I::Surface->DrawOutlinedRect(pos.x - width / 2, top.y, (pos.x - width / 2) + width, top.y + height);
			I::Surface->DrawSetColor(Color(20, 20, 20, alpha));
			I::Surface->DrawOutlinedRect((pos.x - width / 2) + 1, top.y + 1, (pos.x - width / 2) + width - 1, top.y + height - 1);
			I::Surface->DrawSetColor(Color(20, 20, 20, alpha));
			I::Surface->DrawOutlinedRect((pos.x - width / 2) - 1, top.y - 1, (pos.x - width / 2) + width + 1, top.y + height + 1);
		}

		//--- Player Health ---//
		if (g_Vars->Visuals.HealthBar)
		{
			int enemy_hp = entity->GetHealth();
			int hp_red = 255 - (enemy_hp * 2.55);
			int hp_green = enemy_hp * 2.55;
			Color health_color = Color(hp_red, hp_green, 1, alpha);

			float offset = (height / 4.f) + 5;
			int hp = height - ((height * enemy_hp) / 100);

			I::Surface->DrawSetColor(Color(20, 20, 20, alpha));
			I::Surface->DrawFilledRect((pos.x - width / 2) - 7, top.y - 1, (pos.x - width / 2) - 3, top.y + height + 1); //	intense maths

			I::Surface->DrawSetColor(health_color);
			I::Surface->DrawLine((pos.x - width / 2) - 6, top.y + hp, (pos.x - width / 2) - 6, top.y + height - 1); //	could have done a rect here,
			I::Surface->DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height - 1); //	but fuck it

			if (enemy_hp < 100)
				I::Surface->DrawT((pos.x - width / 2) - 5, top.y + hp - 2, Color(255, 255, 255, alpha), g_Globals->Espfont, true, StringUpper(std::to_string(enemy_hp)).c_str());
		}

		//--- Player Ammunition ---//
		if (g_Vars->Visuals.Ammo && !entity->IsKnifeorNade())
		{
			int height = (pos.y - top.y);

			float offset = (height / 4.f) + 5;
			UINT hp = height - (UINT)((height * 3) / 100);

			auto animLayer = entity->GetAnimOverlay(1);

			if (!animLayer->m_pOwner)
				return;

			auto weapon = entity->GetActiveWeapon();

			if (!weapon)
				return;

			auto activity = entity->GetSequenceActivity(animLayer->m_nSequence);

			int iClip = weapon->GetAmmo();
			int iClipMax = weapon->GetCSWpnData()->max_clip;

			float box_w = (float)fabs(height / 2);
			float width;
			if (activity == 967 && animLayer->m_flWeight != 0.f)
			{
				float cycle = animLayer->m_flCycle; // 1 = finished 0 = just started
				width = (((box_w * cycle) / 1.f));
			}
			else
				width = (((box_w * iClip) / iClipMax));

			I::Surface->DrawSetColor(Color(20, 20, 20, alpha));
			I::Surface->DrawFilledRect((pos.x - box_w / 2), top.y + height + 2, (pos.x - box_w / 2) + box_w + 1, top.y + height + 6); //outline
			I::Surface->DrawSetColor(Color(61, 135, 255, alpha));
			I::Surface->DrawFilledRect((pos.x - box_w / 2) + 1, top.y + height + 3, (pos.x - box_w / 2) + width, top.y + height + 5); //ammo
		}

		//--- Player Bones ---//
		if (g_Vars->Visuals.Skeleton)
		{
			static Vector HitboxW2S[19];

			auto& records = g_Backtrack->PlayerRecords[entity->EntIndex()];
			if (records.empty())
				return;

			//matrix3x4_t matrix[128];
			//entity->SetupBones(matrix, 128, 256, I::Globals->curtime);
			//std::memcpy(matrix, records.front().matrix, sizeof(records.front().matrix));

			for (int i = 0; i < 19; i++)
			{
			//	g_Utils->WorldToScreen(entity->GetHitboxPosition(i, matrix), HitboxW2S[i]);
			}

			Color color = Color(255, 255, 255, alpha);
			
			matrix3x4_t matrix[128];
			entity->SetupBones(matrix, 128, 256, I::Globals->curtime);

			studiohdr_t *studioHdr = I::ModelInfo->GetStudiomodel(entity->GetModel());
			if (studioHdr) {
				for (int i = 0; i < studioHdr->numbones; i++)
				{
					mstudiobone_t *bone = studioHdr->GetBone(i);
					if (!bone || !(bone->flags & 0x00000100) || bone->parent == -1)
						continue;

					Vector bonePos1;
					if (!g_Utils->WorldToScreen(Vector(matrix[i][0][3], matrix[i][1][3], matrix[i][2][3]), bonePos1))
						continue;

					Vector bonePos2;
					if (!g_Utils->WorldToScreen(Vector(matrix[bone->parent][0][3], matrix[bone->parent][1][3], matrix[bone->parent][2][3]), bonePos2))
						continue;

					I::Surface->DrawSetColor(Color(255, 255, 255, alpha));
					I::Surface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
				}
			}
		}

		//--- Player Weapon ---//
		if (g_Vars->Visuals.Weapon)
		{
			auto weapon = entity->GetActiveWeapon();

			if (!weapon)
				return;

			std::string weap_name = FixNetworkedName(weapon->GetClientClass()->NetworkName);
			std::transform(weap_name.begin(), weap_name.end(), weap_name.begin(), ::tolower);
			if (entity->IsKnifeorNade() || !g_Vars->Visuals.Ammo)
				I::Surface->DrawT(pos.x, pos.y + 1, Color(255, 255, 255, alpha), g_Globals->Espfont, true, StringUpper(weap_name).c_str());
			else
				I::Surface->DrawT(pos.x, pos.y + 6, Color(255, 255, 255, alpha), g_Globals->Espfont, true, StringUpper(weap_name).c_str());
		}

		//--- Side Info ---//
		if (g_Vars->Visuals.Info)
		{
			std::vector<std::pair<std::string, Color>> side_info; //pair so we can add a color

			if (entity->ArmorValue() > 0)
				side_info.push_back(std::pair<std::string, Color>(StringUpper(entity->GetArmorName()), Color(255, 255, 255, alpha)));

			if (entity->IsScoped())
				side_info.push_back(std::pair<std::string, Color>("ZOOM", Color(52, 165, 207, alpha)));

			if (g_Globals->IsChokingTicks[entity->EntIndex()])
				side_info.push_back(std::pair<std::string, Color>("FAKE", Color(255, 255, 255, alpha)));

			int i = 0;
			for (auto Text : side_info)
			{
				I::Surface->DrawT((pos.x + width / 2) + 3, top.y + i, Text.second, g_Globals->Espfont, false, Text.first.c_str());
				i += 9;
			}
		}
	}
}

void Visuals::ApplyNightmode()
{
	auto local_player = g_Globals->LocalPlayer;

	if (!local_player)
		return;

	static auto r_drawspecificstaticprop = I::CVar->FindVar("r_DrawSpecificStaticProp");
	r_drawspecificstaticprop->SetValue(1);

	if (!I::Engine->IsInGame())
	{
		NightmodeComplete = false;
		return;
	}

	if (!NightmodeComplete)
	{
		for (MaterialHandle_t i = I::MaterialSystem->FirstMaterial(); i != I::MaterialSystem->InvalidMaterial(); i = I::MaterialSystem->NextMaterial(i))
		{
			IMaterial *pMaterial = I::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World textures"))
			{
				pMaterial->ColorModulate(0.18, 0.18, 0.18); //sure why not
				pMaterial->ColorModulate(0.18, 0.18, 0.18); //sure why not
			}
			else if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				pMaterial->ColorModulate(.40, .40, .40); //.30 all
				pMaterial->AlphaModulate(0.75); //0.70 is the minimum value before some props begin to not draw ~stacker
			}
			else if (strstr(pMaterial->GetTextureGroupName(), "Particle textures"))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true); //yes god please
			}
			
			g_MiscFunc->LoadNamedSky("sky_csgo_night02b");
		}
		NightmodeComplete = true;
	}
}

/*Visuals::BBox Visuals::BoundingBox(C_BaseEntity* Entity, ICollideable* collision) // uc pasting > all
{
	static Visuals::BBox Final = {0,0,0,0};

	DWORD m_rgflCoordinateFrame = (DWORD)0x470 - 0x30;

	const matrix3x4_t& trans = *(matrix3x4_t*)((DWORD)Entity + (DWORD)m_rgflCoordinateFrame);

	Vector min = collision->OBBMins();
	Vector max = collision->OBBMaxs();

	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++)
	{
		g_Math->VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = Entity->GetOrigin();
	Vector flb;
	Vector brt;
	Vector blb;
	Vector frt;
	Vector frb;
	Vector brb;
	Vector blt;
	Vector flt;

	g_Utils->WorldToScreen(pointsTransformed[3], flb);
	g_Utils->WorldToScreen(pointsTransformed[5], brt);
	g_Utils->WorldToScreen(pointsTransformed[0], blb);
	g_Utils->WorldToScreen(pointsTransformed[4], frt);
	g_Utils->WorldToScreen(pointsTransformed[2], frb);
	g_Utils->WorldToScreen(pointsTransformed[1], brb);
	g_Utils->WorldToScreen(pointsTransformed[6], blt);
	g_Utils->WorldToScreen(pointsTransformed[7], flt);

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;		// left
	float top = flb.y;		// top
	float right = flb.x;	// right
	float bottom = flb.y;	// bottom

	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	Final.x = left;
	Final.y = bottom;
	Final.w = right;
	Final.h = top;

	return Final;
}

void Visuals::ESP()
{
	if (I::Engine->IsInGame() && I::Engine->IsConnected()) {
		if (g_Vars->Visuals.Enable)
		{
			if (g_Globals->LocalPlayer && g_Globals->LocalPlayer->IsAlive())
			{
				for (int i = 1; i <= I::Engine->GetMaxClients(); ++i)
				{
					C_BaseEntity* Entity = I::EntityList->GetClientEntity(i);
					if (!Entity
						|| !Entity->IsAlive()
						|| Entity->IsDormant()
						|| Entity == g_Globals->LocalPlayer
						|| Entity->GetTeam() == g_Globals->LocalPlayer->GetTeam())
						continue;

					ICollideable* collision = (ICollideable*)Entity->GetCollideable();

					Vector wtsOrigin;

					if (!g_Utils->WorldToScreen(Entity->GetOrigin(), wtsOrigin))
						continue;

					Visuals::BBox box = BoundingBox(Entity, collision);

					if (g_Vars->Visuals.AimPoint)
					{
						static Vector AimpointsAfter[25];

						for (int hitbox = 0; hitbox < 25; hitbox++)
						{
							g_Utils->WorldToScreen(g_Globals->AimPoints[Entity->EntIndex()][hitbox], AimpointsAfter[hitbox]);

							if (g_Globals->AimPoints[Entity->EntIndex()][hitbox] != Vector(0, 0, 0))
							{
								I::Surface->FilledRect(AimpointsAfter[hitbox].x - 1.5, AimpointsAfter[hitbox].y - 1.5, 3, 3, Color(255, 255, 255, 255));
							}
						}
					}

					if (g_Vars->Visuals.Skeleton)
					{
						static Vector HitboxW2S[19];

						matrix3x4_t matrix[128];
						Entity->SetupBones(matrix, 128, 256, I::Globals->curtime);

						for (int i = 0; i < 19; i++)
						{
							g_Utils->WorldToScreen(Entity->GetHitboxPosition(i, matrix), HitboxW2S[i]);
						}

						Color color = Color(255, 255, 255, 255);

						//spine
						I::Surface->Line(HitboxW2S[HITBOX_HEAD].x, HitboxW2S[HITBOX_HEAD].y, HitboxW2S[HITBOX_NECK].x, HitboxW2S[HITBOX_NECK].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_NECK].x, HitboxW2S[HITBOX_NECK].y, HitboxW2S[HITBOX_UPPER_CHEST].x, HitboxW2S[HITBOX_UPPER_CHEST].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_UPPER_CHEST].x, HitboxW2S[HITBOX_UPPER_CHEST].y, HitboxW2S[HITBOX_LOWER_CHEST].x, HitboxW2S[HITBOX_LOWER_CHEST].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_LOWER_CHEST].x, HitboxW2S[HITBOX_LOWER_CHEST].y, HitboxW2S[HITBOX_THORAX].x, HitboxW2S[HITBOX_THORAX].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_THORAX].x, HitboxW2S[HITBOX_THORAX].y, HitboxW2S[HITBOX_BELLY].x, HitboxW2S[HITBOX_BELLY].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_BELLY].x, HitboxW2S[HITBOX_BELLY].y, HitboxW2S[HITBOX_PELVIS].x, HitboxW2S[HITBOX_PELVIS].y, color);

						//right leg
						I::Surface->Line(HitboxW2S[HITBOX_PELVIS].x, HitboxW2S[HITBOX_PELVIS].y, HitboxW2S[HITBOX_RIGHT_THIGH].x, HitboxW2S[HITBOX_RIGHT_THIGH].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_RIGHT_THIGH].x, HitboxW2S[HITBOX_RIGHT_THIGH].y, HitboxW2S[HITBOX_RIGHT_CALF].x, HitboxW2S[HITBOX_RIGHT_CALF].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_RIGHT_CALF].x, HitboxW2S[HITBOX_RIGHT_CALF].y, HitboxW2S[HITBOX_RIGHT_FOOT].x, HitboxW2S[HITBOX_RIGHT_FOOT].y, color);

						//right arm
						I::Surface->Line(HitboxW2S[HITBOX_NECK].x, HitboxW2S[HITBOX_NECK].y, HitboxW2S[HITBOX_RIGHT_UPPER_ARM].x, HitboxW2S[HITBOX_RIGHT_UPPER_ARM].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_RIGHT_UPPER_ARM].x, HitboxW2S[HITBOX_RIGHT_UPPER_ARM].y, HitboxW2S[HITBOX_RIGHT_FOREARM].x, HitboxW2S[HITBOX_RIGHT_FOREARM].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_RIGHT_FOREARM].x, HitboxW2S[HITBOX_RIGHT_FOREARM].y, HitboxW2S[HITBOX_RIGHT_HAND].x, HitboxW2S[HITBOX_RIGHT_HAND].y, color);

						//left leg
						I::Surface->Line(HitboxW2S[HITBOX_PELVIS].x, HitboxW2S[HITBOX_PELVIS].y, HitboxW2S[HITBOX_LEFT_THIGH].x, HitboxW2S[HITBOX_LEFT_THIGH].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_LEFT_THIGH].x, HitboxW2S[HITBOX_LEFT_THIGH].y, HitboxW2S[HITBOX_LEFT_CALF].x, HitboxW2S[HITBOX_LEFT_CALF].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_LEFT_CALF].x, HitboxW2S[HITBOX_LEFT_CALF].y, HitboxW2S[HITBOX_LEFT_FOOT].x, HitboxW2S[HITBOX_LEFT_FOOT].y, color);

						//left arm
						I::Surface->Line(HitboxW2S[HITBOX_NECK].x, HitboxW2S[HITBOX_NECK].y, HitboxW2S[HITBOX_LEFT_UPPER_ARM].x, HitboxW2S[HITBOX_LEFT_UPPER_ARM].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_LEFT_UPPER_ARM].x, HitboxW2S[HITBOX_LEFT_UPPER_ARM].y, HitboxW2S[HITBOX_LEFT_FOREARM].x, HitboxW2S[HITBOX_LEFT_FOREARM].y, color);
						I::Surface->Line(HitboxW2S[HITBOX_LEFT_FOREARM].x, HitboxW2S[HITBOX_LEFT_FOREARM].y, HitboxW2S[HITBOX_LEFT_HAND].x, HitboxW2S[HITBOX_LEFT_HAND].y, color);
					}

					if (g_Vars->Visuals.Box)
					{
						I::Surface->OutlinedVecRect(box.x, box.y, box.w, box.h, Color(200, 225, 0, 255));
						I::Surface->OutlinedVecRect(box.x - 1, box.y - 1, box.w + 1, box.h + 1, Color(0, 0, 0, 255));
						I::Surface->OutlinedVecRect(box.x + 1, box.y + 1, box.w - 1, box.h - 1, Color(0, 0, 0, 255));
					}

					if (g_Vars->Visuals.HealthBar)
					{
						int Health = Entity->GetHealth();
						int Height = (box.h - box.y);
						int HealthHeight = Health * Height / 100;

						I::Surface->FilledRect(box.w + 3, box.y - 1, 4, Height + 3, Color(0, 0, 0, 255));
						I::Surface->FilledRect(box.w + 4, box.y + Height - HealthHeight, 2, HealthHeight + 1, Color(0, 255, 0, 255));
					}

					if (g_Vars->Visuals.Name)
					{
						PlayerInfo_t pInfo;
						I::Engine->GetPlayerInfo(i, &pInfo);
						I::Surface->DrawT((box.x + ((box.w - box.x) / 2)), box.y - 14, Color(255, 255, 255, 255), g_Globals->Espfont, true, pInfo.szName);
					}

					if (g_Vars->Visuals.Weapon)
					{
						auto weapon = Entity->GetActiveWeapon();

						if (weapon)
						{
							I::Surface->DrawT((box.x + ((box.w - box.x) / 2)), box.h, Color(255, 255, 255, 255), g_Globals->Espfont, true, weapon->GetName().c_str());
						}

						I::Surface->DrawT((box.x + ((box.w - box.x) / 2)), box.h + 15, Color(255, 255, 255, 255), g_Globals->Espfont, true, g_Globals->ResolverStage[Entity->EntIndex()].c_str());
					}

	/*				auto RemapVal = [](float val, float A, float B, float C, float D) -> float
					{
						if (A == B)
							return val >= B ? D : C;
						return C + (D - C) * (val - A) / (B - A);
					};

					{
						for (int paramloop = 0; paramloop < 24; paramloop++)
						{
							float posparam = RemapVal(Entity->m_flPoseParameter()[paramloop], 0, 1, -180, 180);
							I::Surface->DrawT((box.x + ((box.w - box.x) / 2)), box.h + 15 + (paramloop * 15), Color(255, 255, 255, 255), g_Globals->Espfont, false, std::to_string(posparam).c_str());

							I::Surface->DrawT((box.x - 20 + ((box.w - box.x) / 2)), box.h + 15 + (paramloop * 15), Color(255, 255, 255, 255), g_Globals->Espfont, false , std::to_string(paramloop).c_str());
						}
					}
					*/
				/*}
			}	
		}
	}
}*/