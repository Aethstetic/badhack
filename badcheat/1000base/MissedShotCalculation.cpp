#include "Main.h"

void MissedShots::AddSnapshot(C_BaseEntity* player)
{
	ShotInfo Shot;

	Shot.Player = player;
	Shot.ShootPos = g_Globals->LocalPlayer->GetEyePosition();
	Shot.TimeFired = I::Globals->curtime;
	Shot.Hitgroup = -1;
	Shot.AimPos = g_Globals->LastAimVec;
	Shot.ResolverStage = g_Globals->ResolverStage[player->EntIndex()];
	std::memcpy(Shot.Matrix, g_Globals->LastAimMatrix, sizeof(g_Globals->LastAimMatrix));

	ShotRecords.push_back(Shot);
}

void MissedShots::FSN(ClientFrameStage_t curStage)
{
	if (curStage == FRAME_RENDER_START)
	{
		g_Globals->LocalPlayer = I::EntityList->GetClientEntity(I::Engine->GetLocalPlayer());
		g_MissedShots->ProcessSnapshots();
		g_MissedShots->HandleTracers();
	}
}

void MissedShots::EventCallback(IGameEvent* game_event)
{
	if (!I::Engine->IsInGame() || !I::Engine->IsConnected() || !g_Globals->LocalPlayer || ShotRecords.empty() || !g_Globals->LocalPlayer->IsAlive())
		return;

	// we do this up here so we can render traces without aimbot shooting
	if (!strcmp(game_event->GetName(), ("bullet_impact")) && I::Engine->GetPlayerForUserID(game_event->GetInt("userid")) == I::Engine->GetLocalPlayer()) 
		BulletImpacts.push_back(Vector(game_event->GetFloat("x"), game_event->GetFloat("y"), game_event->GetFloat("z")));

	// clear records when round restarts
	if (!strcmp(game_event->GetName(), ("round_start")))
		ShotRecords.clear();

	auto& Shot = ShotRecords.front();
	if (!strcmp(game_event->GetName(), ("bullet_impact"))) 
	{
		int Uid = game_event->GetInt("userid");
		if (I::Engine->GetPlayerForUserID(Uid) == I::Engine->GetLocalPlayer() && !Shot.WasProcessed)
		{
			Shot.WasProcessed = true;
			Shot.TimeProcessed = I::Globals->curtime;
		}
	}

	if (!strcmp(game_event->GetName(), ("player_hurt"))) 
	{
		int Uid = game_event->GetInt("userid");
		if (I::Engine->GetPlayerForUserID(Uid) == I::Engine->GetLocalPlayer() && !Shot.WasProcessed)
		{
			Shot.WasProcessed = true;
			Shot.TimeProcessed = I::Globals->curtime;
		}
	
		Shot.Damage = game_event->GetInt("dmg_health");
		Shot.Hitgroup = game_event->GetInt(("hitgroup"));
		I::Engine->ExecuteClientCmd("play physics\\metal\\paintcan_impact_hard3.wav");
	}
}

void MissedShots::ProcessSnapshots()
{
	if (ShotRecords.empty() || BulletImpacts.empty())
		return;

	auto Shot = ShotRecords.front();
	if (abs(I::Globals->curtime - Shot.TimeFired) > 1.f) {
		ShotRecords.erase(ShotRecords.begin());
		return;
	}

	studiohdr_t *studioHdr = I::ModelInfo->GetStudiomodel(Shot.Player->GetModel());
	if (!studioHdr)
		return;

	mstudiohitboxset_t *set = studioHdr->GetHitboxSet(0);
	if (!set)
		return;

	std::vector<CSphere> m_cSpheres;
	std::vector<COBB> m_cOBBs;
	Ray_t Ray;
	trace_t Trace;
	Vector vMin, vMax;

	for (int i = 0; i < set->numhitboxes; i++) {
		mstudiobbox_t* pbox = set->GetHitbox(i);
		if (pbox->radius != -1.0f) {
			Vector min, max;

			g_Math->VectorTransform(pbox->min, Shot.Matrix[pbox->bone], min);
			g_Math->VectorTransform(pbox->max, Shot.Matrix[pbox->bone], max);

			//I::DebugOverlay->AddCapsuleOverlay(min, max, pbox->radius, 150, 50, 50, 100, 2);
			SetupCapsule(min, max, pbox->radius, pbox->group, m_cSpheres);
		}
	}

	CTraceFilterPlayersOnlySkipOne filter(Shot.Player);

	Vector BulletHitPos;
	float LowestDistance = 100000;

	for (size_t i = 0; i < BulletImpacts.size(); i++) {
		auto current_impact = BulletImpacts.at(i);

		if (current_impact.DistTo(Shot.Player->GetOrigin()) < LowestDistance)
		{
			LowestDistance = current_impact.DistTo(Shot.Player->GetOrigin());
			BulletHitPos = current_impact;
		}
	}

	// we need to use bullet_land_pos but because of shitty things we have to make changes..
	Vector Pos = BulletHitPos.DistTo(Shot.AimPos) <= 6.f ? BulletHitPos : BulletImpacts.back();
	Ray.Init(Shot.ShootPos, BulletImpacts.back());
	trace_hitbox(Shot.Player, Ray, Trace, m_cSpheres, m_cOBBs);

	// we hit them
	if (Shot.Hitgroup >= 0)
	{
		I::CVar->ConsoleColorPrintf(Color(65, 135, 245), "[jefferyhook] ");
		Color color = Color(255, 255, 255);

		I::CVar->ConsoleColorPrintf(color, "-%i dealt (%i health left) at %s\n", Shot.Damage, Shot.Player->GetHealth(), Shot.ResolverStage.c_str());
		HitmarkerInfo.push_back({ Shot.AimPos, BulletHitPos, I::Globals->curtime + 4.f, 255 });
	}
	else if (Shot.TimeProcessed != 0.f) /// missed
	{
		I::CVar->ConsoleColorPrintf(Color(65, 135, 245), "[jefferyhook] ");
		Color color = Color(255, 255, 255);

		if (Trace.hit_entity == Shot.Player) {
			I::CVar->ConsoleColorPrintf(color, "missed shot due to bad resolve at %s\n", Shot.ResolverStage.c_str());
			g_Globals->MissedShotsResolve[Shot.Player->EntIndex()]++;
		}
		else
			I::CVar->ConsoleColorPrintf(color, "missed shot due to spread at %s\n", Shot.ResolverStage.c_str());
	}
	else
		return;

	ShotRecords.erase(ShotRecords.begin());
}

void MissedShots::DrawHitmarkers()
{
	if (!I::Engine->IsInGame() || !g_Globals->LocalPlayer || !g_Globals->LocalPlayer->IsAlive()) {
		HitmarkerInfo.clear();
		return;
	}

	if (HitmarkerInfo.empty())
		return;

	float Time = 4.f, Size = 8.f, Gap = 2.5f;
	for (int i = 0; i < HitmarkerInfo.size(); i++)
	{
		float Difference = HitmarkerInfo.at(i).Time - I::Globals->curtime;
		float Difference2 = I::Globals->curtime - (HitmarkerInfo.at(i).Time - Time);

		if (Difference <= 0.f)
		{
			HitmarkerInfo.erase(HitmarkerInfo.begin() + i);
			continue;
		}

		Vector Pos;
		if (HitmarkerInfo.at(i).AimPos.DistTo(HitmarkerInfo.at(i).ImpactPos) <= 6.f)
			Pos = HitmarkerInfo.at(i).ImpactPos;
		else
			Pos = HitmarkerInfo.at(i).AimPos;

		Vector pos2D;
		if (!g_Utils->WorldToScreen(Pos, pos2D))
			continue;

		/* this will give it a delay for the fade */
		float Alpha = Difference2 < 1 ? 1 : (Difference / (Time - 1));
		I::Surface->DrawSetColor(Color(255, 255, 255, 255 * Alpha));

		/* center white line */
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y - Size, pos2D.x - Gap, pos2D.y - Gap);
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y + Size, pos2D.x - Gap, pos2D.y + Gap);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y - Size, pos2D.x + Gap, pos2D.y - Gap);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y + Size, pos2D.x + Gap, pos2D.y + Gap);

		/* first outline */
		I::Surface->DrawSetColor(Color(0, 0, 0, 160 * Alpha));
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y - Size + 1, pos2D.x - Gap, pos2D.y - Gap + 1);
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y + Size + 1, pos2D.x - Gap, pos2D.y + Gap + 1);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y - Size + 1, pos2D.x + Gap, pos2D.y - Gap + 1);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y + Size + 1, pos2D.x + Gap, pos2D.y + Gap + 1);
		
		/* second outline */
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y - Size - 1, pos2D.x - Gap, pos2D.y - Gap - 1);
		I::Surface->DrawLine(pos2D.x - Size, pos2D.y + Size - 1, pos2D.x - Gap, pos2D.y + Gap - 1);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y - Size - 1, pos2D.x + Gap, pos2D.y - Gap - 1);
		I::Surface->DrawLine(pos2D.x + Size, pos2D.y + Size - 1, pos2D.x + Gap, pos2D.y + Gap - 1);
	}
}

void MissedShots::HandleTracers(void)
{
	if (!I::Engine->IsInGame() || !g_Globals->LocalPlayer || !g_Globals->LocalPlayer->IsAlive()) {
		BulletImpacts.clear();
		return;
	}

	Color CurrentColor = Color(244, 244, 66);
	for (size_t i = 0; i < BulletImpacts.size(); i++) {
		auto Impact = BulletImpacts.at(i);

		BeamInfo_t BeamInfo;

		BeamInfo.m_nType = TE_BEAMPOINTS;
		BeamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		BeamInfo.m_nModelIndex = -1;
		BeamInfo.m_flHaloScale = 0.0f;
		BeamInfo.m_flLife = 4;
		BeamInfo.m_flWidth = 2.75;
		BeamInfo.m_flEndWidth = 2.75;
		BeamInfo.m_flFadeLength = 0.0f;
		BeamInfo.m_flAmplitude = 2.0f;
		BeamInfo.m_flBrightness = 255.f;
		BeamInfo.m_flSpeed = 0.2f;
		BeamInfo.m_nStartFrame = 0;
		BeamInfo.m_flFrameRate = 0.f;
		BeamInfo.m_flRed = CurrentColor.r();
		BeamInfo.m_flGreen = CurrentColor.g();
		BeamInfo.m_flBlue = CurrentColor.b();
		BeamInfo.m_nSegments = 2;
		BeamInfo.m_bRenderable = true;
		BeamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		BeamInfo.m_vecStart = g_Globals->LocalPlayer->GetEyePosition();
		BeamInfo.m_vecStart.z -= 3; // so we can actually see the tracer in firstperson

		BeamInfo.m_vecEnd = Impact;

		auto beam = I::RenderBeams->CreateBeamPoints(BeamInfo);
		if (beam)
			I::RenderBeams->DrawBeam(beam);

		BulletImpacts.erase(BulletImpacts.begin() + i);
	}
}

__forceinline float DotProduct(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

__forceinline float DotProduct(const Vector& a, const Vector& b)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	return(a.x*b.x + a.y*b.y + a.z*b.z);
}

bool MissedShots::IntersectInfiniteRayWithSphere(const Vector &vecRayOrigin, const Vector &vecRayDelta, const Vector &vecSphereCenter, float flRadius, float *pT1, float *pT2)
{
	Vector vecSphereToRay;
	VectorSubtract(vecRayOrigin, vecSphereCenter, vecSphereToRay);

	float a = DotProduct(vecRayDelta, vecRayDelta);

	// This would occur in the case of a zero-length ray
	if (a == 0.0f) {
		*pT1 = *pT2 = 0.0f;
		return vecSphereToRay.LengthSqr() <= flRadius * flRadius;
	}

	float b = 2 * DotProduct(vecSphereToRay, vecRayDelta);
	float c = DotProduct(vecSphereToRay, vecSphereToRay) - flRadius * flRadius;
	float flDiscrim = b * b - 4 * a * c;
	if (flDiscrim < 0.0f)
		return false;

	flDiscrim = sqrt(flDiscrim);
	float oo2a = 0.5f / a;
	*pT1 = (-b - flDiscrim) * oo2a;
	*pT2 = (-b + flDiscrim) * oo2a;
	return true;
}

void MissedShots::VectorITransform(const Vector *in1, const matrix3x4_t& in2, Vector *out)
{
	Assert(s_bMathlibInitialized);
	float in1t[3];

	in1t[0] = in1->x - in2[0][3];
	in1t[1] = in1->y - in2[1][3];
	in1t[2] = in1->z - in2[2][3];

	out->x = in1t[0] * in2[0][0] + in1t[1] * in2[1][0] + in1t[2] * in2[2][0];
	out->y = in1t[0] * in2[0][1] + in1t[1] * in2[1][1] + in1t[2] * in2[2][1];
	out->z = in1t[0] * in2[0][2] + in1t[1] * in2[1][2] + in1t[2] * in2[2][2];
}

// assume in2 is a rotation and rotate the input vector
void  MissedShots::VectorIRotate(const Vector *in1, const matrix3x4_t& in2, Vector *out)
{
	Assert(s_bMathlibInitialized);
	Assert(in1 != out);
	out->x = DotProduct((const float*)in1, in2[0]);
	out->y = DotProduct((const float*)in1, in2[1]);
	out->z = DotProduct((const float*)in1, in2[2]);
}

bool  MissedShots::IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (dir.x >= 0)
	{
		tmin = (min.x - origin.x) / dir.x;
		tmax = (max.x - origin.x) / dir.x;
	}
	else
	{
		tmin = (max.x - origin.x) / dir.x;
		tmax = (min.x - origin.x) / dir.x;
	}

	if (dir.y >= 0)
	{
		tymin = (min.y - origin.y) / dir.y;
		tymax = (max.y - origin.y) / dir.y;
	}
	else
	{
		tymin = (max.y - origin.y) / dir.y;
		tymax = (min.y - origin.y) / dir.y;
	}

	if (tmin > tymax || tymin > tmax)
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	if (dir.z >= 0)
	{
		tzmin = (min.z - origin.z) / dir.z;
		tzmax = (max.z - origin.z) / dir.z;
	}
	else
	{
		tzmin = (max.z - origin.z) / dir.z;
		tzmax = (min.z - origin.z) / dir.z;
	}

	if (tmin > tzmax || tzmin > tmax)
		return false;

	//behind us
	if (tmin < 0 || tmax < 0)
		return false;

	return true;
}

bool MissedShots::intersected_ray_obb(const Ray_t& ray, Vector& vecbbMin, Vector& vecbbMax, const matrix3x4_t &boneMatrix)
{
	//Transform ray into model space of hitbox so we only have to deal with an AABB instead of OBB
	Vector ray_trans, dir_trans;
	VectorITransform(&ray.m_Start, boneMatrix, &ray_trans);
	VectorIRotate(&ray.m_Delta, boneMatrix, &dir_trans); //only rotate direction vector! no translation!

	return IntersectRayWithAABB(ray_trans, dir_trans, vecbbMin, vecbbMax);
}

void MissedShots::trace_hitbox(C_BaseEntity* player, Ray_t& ray, trace_t &tr, std::vector<CSphere>&m_cSpheres, std::vector<COBB>&m_cOBBs)
{
	tr.hit_entity = nullptr;
	Vector intersectpos;

	for (auto& i : m_cSpheres)
	{
		if (i.intersectsRay(ray, intersectpos))
		{
			InterceptPos = intersectpos;
			//I::DebugOverlay->AddBoxOverlay(intersectpos, Vector(-1.75, -1.75, -1.75), Vector(1.75, 1.75, 1.75), QAngle(0, 0, 0), 255, 255, 255, 255, 2.f);
			tr.hitgroup = i.Hitgroup;
			tr.hit_entity = player;
			return;
		}
	}

	if (!tr.hit_entity)
	{
		for (auto& i : m_cOBBs)
		{
			if (intersected_ray_obb(ray, i.vecbbMin, i.vecbbMax, *i.boneMatrix))
			{
				tr.hitgroup = i.hitgroup;
				tr.hit_entity = player;
				return;
			}
		}
	}
}

void MissedShots::SetupCapsule(const Vector& vecMin, const Vector& vecMax, float flRadius, int hitgroup, std::vector<CSphere>&m_cSpheres)
{
	auto vecDelta = (vecMax - vecMin);
	g_Math->VectorNormalize(vecDelta);
	auto vecCenter = vecMin;

	CSphere Sphere = CSphere{ vecMin, flRadius, hitgroup };
	m_cSpheres.push_back(Sphere);

	for (size_t i = 1; i < std::floor(vecMin.DistTo(vecMax)); ++i)
	{
		CSphere Sphere = CSphere{ vecMin + vecDelta * static_cast<float>(i), flRadius, hitgroup };
		m_cSpheres.push_back(CSphere{ Sphere });
	}

	CSphere UsedSphere = CSphere{ vecMin, flRadius, hitgroup };
	m_cSpheres.push_back(UsedSphere);
}

bool CSphere::intersectsRay(const Ray_t& ray, Vector& vecIntersection)
{
	float T1, T2;
	if (!g_MissedShots->IntersectInfiniteRayWithSphere(ray.m_Start, ray.m_Delta, m_vecCenter, m_flRadius, &T1, &T2))
		return false;

	if (T1 > 1.0f || T2 < 0.0f)
		return false;

	// Clamp it!
	if (T1 < 0.0f)
		T1 = 0.0f;
	if (T2 > 1.0f)
		T2 = 1.0f;

	vecIntersection = ray.m_Start + ray.m_Delta * T1;

	return true;
}

bool CSphere::intersectsRay(const Ray_t& ray)
{
	float T1, T2;
	if (!g_MissedShots->IntersectInfiniteRayWithSphere(ray.m_Start, ray.m_Delta, m_vecCenter, m_flRadius, &T1, &T2))
		return false;

	return T2 >= 0.0f && T1 <= 1.0f;
}