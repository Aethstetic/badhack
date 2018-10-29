#pragma once

class COBB
{
public:
	Vector vecbbMin;
	Vector vecbbMax;
	matrix3x4_t *boneMatrix;
	int hitgroup;

	COBB(void) {};
	COBB(const Vector& bbMin, const Vector& bbMax, matrix3x4_t *matrix, int ihitgroup) { vecbbMin = bbMin; vecbbMax = bbMax; boneMatrix = matrix; hitgroup = ihitgroup; };
};

class CSphere
{
public:
	Vector m_vecCenter;
	float   m_flRadius = 0.f;
	//float   m_flRadius2 = 0.f; // r^2

	CSphere(void) {};
	CSphere(const Vector& vecCenter, float flRadius, int hitgroup) { m_vecCenter = vecCenter; m_flRadius = flRadius; Hitgroup = hitgroup; };

	int Hitgroup;
	bool intersectsRay(const Ray_t& ray);
	bool intersectsRay(const Ray_t& ray, Vector& vecIntersection);
};

struct ShotInfo
{
	C_BaseEntity* Player; 
	float TimeFired; 
	float TimeProcessed; 
	bool WasProcessed;
	int Hitgroup;
	int Damage;
	std::string ResolverStage;
	Vector ShootPos;
	Vector AimPos;	
	matrix3x4_t Matrix[128];

	bool processed_trace;
};

struct HitMarker
{
	Vector AimPos;
	Vector ImpactPos;
	float Time;
	float Alpha;
};

class MissedShots {
public:
	void FSN(ClientFrameStage_t curStage);
	void AddSnapshot(C_BaseEntity* player);
	void EventCallback(IGameEvent* game_event);
	void ProcessSnapshots();

	bool IntersectInfiniteRayWithSphere(const Vector &vecRayOrigin, const Vector &vecRayDelta, const Vector &vecSphereCenter, float flRadius, float *pT1, float *pT2);
	void VectorITransform(const Vector *in1, const matrix3x4_t& in2, Vector *out);
	void VectorIRotate(const Vector *in1, const matrix3x4_t& in2, Vector *out);
	bool IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max);
	bool intersected_ray_obb(const Ray_t& ray, Vector& vecbbMin, Vector& vecbbMax, const matrix3x4_t &boneMatrix);
	void trace_hitbox(C_BaseEntity* player, Ray_t& ray, trace_t &tr, std::vector<CSphere>&m_cSpheres, std::vector<COBB>&m_cOBBs);
	void SetupCapsule(const Vector& vecMin, const Vector& vecMax, float flRadius, int hitgroup, std::vector<CSphere>&m_cSpheres);
	void DrawHitmarkers();
	void HandleTracers(void);

	Vector InterceptPos;
	std::vector<ShotInfo> ShotRecords;
	std::vector<HitMarker> HitmarkerInfo;
	std::vector<Vector> BulletImpacts;
}; extern MissedShots* g_MissedShots;
