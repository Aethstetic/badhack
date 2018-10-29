#pragma once

struct BBox
{
	int x, y, w, h;
};

class Visuals {
public:

	void SetupDraw();
	bool NightmodeComplete = false;

private:

	/*struct BBox
	{
		int x, y, w, h;
	};*/

	//BBox BoundingBox(C_BaseEntity* Entity, ICollideable* collision);
	void RenderPlayers(C_BaseEntity * entity, float alpha, matrix3x4_t * bone_matrix = nullptr);
	void RenderDropped(C_BaseEntity * entity);
	void ApplyNightmode();

}; extern Visuals* g_Visuals;