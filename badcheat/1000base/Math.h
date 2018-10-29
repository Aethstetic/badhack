#pragma once

#define PI 3.14159265358979323846f

class Math {
public:
	float DistanceToRay(const Vector &pos, const Vector &ray_start, const Vector &ray_end);

	void VectorAngles(const Vector &vecForward, Vector &vecAngles);

	void VectorTransform(const Vector& in1, const matrix3x4_t &in2, Vector &out);

	Vector CalcAngle(const Vector& vecSource, const Vector& vecDestination);

	vec_t VectorNormalize(Vector& v);

	void AngleVectors(const Vector &angles, Vector *forward);

	void NormalizeAngles(Vector& angles);

	float NormalizeYaw(float yaw);

	void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up);

	void RandomSeed(int seed);

	float RandomFloat(float min, float max);

	bool Clamp(Vector &angles);

	void ClampAngles(Vector &angles);

	float GRD_TO_BOG(float GRD);

	float distanceCalculate(Vector num1, Vector num2);

}; extern Math* g_Math;