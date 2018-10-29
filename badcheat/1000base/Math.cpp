#include "SDK.h"

void Math::VectorAngles(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / 3.14159265358979323846f;

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / 3.14159265358979323846f;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}

Vector Math::CalcAngle(const Vector& vecSource, const Vector& vecDestination)
{
	Vector qAngles;
	Vector delta = Vector((vecSource[0] - vecDestination[0]), (vecSource[1] - vecDestination[1]), (vecSource[2] - vecDestination[2]));
	double hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
	qAngles[0] = (float)(atan(delta[2] / hyp) * (180.0 / M_PI));
	qAngles[1] = (float)(atan(delta[1] / delta[0]) * (180.0 / M_PI));
	qAngles[2] = 0.f;
	if (delta[0] >= 0.f)
		qAngles[1] += 180.f;

	return qAngles;
}

inline Vector operator*(float lhs, const Vector &rhs)
{
	return rhs * lhs;
}

float Math::DistanceToRay(const Vector &pos, const Vector &ray_start, const Vector &ray_end)
{
	Vector to = pos - ray_start;
	Vector dir = ray_end - ray_start;
	float length = VectorNormalize(dir);

	float rangeAlong = dir.Dot(to);

	float range;

	if (rangeAlong < 0.0f)
		range = -(pos - ray_start).Length();
	else if (rangeAlong > length)
		range = -(pos - ray_end).Length();
	else
	{
		Vector onRay = ray_start + rangeAlong * dir;
		range = (pos - onRay).Length();
	}

	return range;
}

float DotProd(const float* a, const float* b) {
	return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void VectorTransOut(const float *in1, const matrix3x4_t& in2, float *out) {
	out[0] = DotProd(in1, in2[0]) + in2[0][3];
	out[1] = DotProd(in1, in2[1]) + in2[1][3];
	out[2] = DotProd(in1, in2[2]) + in2[2][3];
}

void Math::VectorTransform(const Vector& in1, const matrix3x4_t &in2, Vector &out) {
	VectorTransOut(&in1.x, in2, &out.x);
}


vec_t Math::VectorNormalize(Vector& v)
{
	vec_t l = v.Length();

	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; v.z = 1.0f;
	}

	return l;
}

void Math::AngleVectors(const Vector &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void Math::NormalizeAngles(Vector& angles)
{
	for (auto i = 0; i < 3; i++) {
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] >  180.0f) angles[i] -= 360.0f;
	}
}


float Math::NormalizeYaw(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}

void sin_cos(float radian, float* sin, float* cos)
{
	*sin = std::sin(radian);
	*cos = std::cos(radian);
}

static constexpr float deg_2_rad(float degree)
{
	return degree * (PI / 180.f);
}

void Math::AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sp, sy, sr, cp, cy, cr;

	sin_cos(deg_2_rad(angles.x), &sp, &cp);
	sin_cos(deg_2_rad(angles.y), &sy, &cy);
	sin_cos(deg_2_rad(angles.z), &sr, &cr);

	if (forward != nullptr)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right != nullptr)
	{
		right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		right->y = -1 * sr * sp * sy + -1 * cr * cy;
		right->z = -1 * sr * cp;
	}

	if (up != nullptr)
	{
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

void Math::RandomSeed(int seed)
{
	static auto random_seed = reinterpret_cast<void(*)(int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));

	random_seed(seed);
}

float Math::RandomFloat(float min, float max)
{
	static auto random_float = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));

	return random_float(min, max);
}

void Math::ClampAngles(Vector &angles) {
	if (angles.y > 180.0f)
		angles.y = 180.0f;
	else if (angles.y < -180.0f)
		angles.y = -180.0f;

	if (angles.x > 89.0f)
		angles.x = 89.0f;
	else if (angles.x < -89.0f)
		angles.x = -89.0f;

	angles.z = 0;
}

bool Math::Clamp(Vector &angles)
{
	if (g_Vars->Misc.ClampAngles)
	{
		Vector a = angles;
		NormalizeAngles(a);
		ClampAngles(a);

		if (isnan(a.x) || isinf(a.x) ||
			isnan(a.y) || isinf(a.y) ||
			isnan(a.z) || isinf(a.z)) {
			return false;
		}
		else {
			angles = a;
			return true;
		}
	}
	else
	{
		Vector a = angles;
		NormalizeYaw(a.y);
	}
}

float Math::GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

float Math::distanceCalculate(Vector num1, Vector num2)
{
	double x = num1.x - num2.x; //calculating number to square in next step
	double y = num1.y - num2.y;
	double dist;

	dist = pow(x, 2) + pow(y, 2);       //calculating Euclidean distance
	dist = sqrt(dist);

	return dist;
}