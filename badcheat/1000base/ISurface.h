#pragma once

struct Vertex_t
{
	Vector2D	m_Position;
	Vector2D	m_TexCoord;

	Vertex_t() {}
	Vertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
};

typedef  Vertex_t FontVertex_t;

class ISurface
{
public:
	void UnLockCursor()
	{
		typedef void(__thiscall* original_fn)(void*);
		g_Utils->get_vfunc< original_fn >(this, 66)(this);
	}

	void DrawSetColor(Color col)
	{
		typedef void(__thiscall* oDrawSetColor)(PVOID, Color);
		return g_Utils->get_vfunc< oDrawSetColor >(this, 14)(this, col);
	}

	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 15)(this, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 16)(this, x0, y0, x1, y1);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 18)(this, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 19)(this, x0, y0, x1, y1);
	}

	void DrawPolyLine(int *x, int *y, int count)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int *, int *, int);
		g_Utils->get_vfunc< OriginalFn >(this, 20)(this, x, y, count);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		g_Utils->get_vfunc< OriginalFn >(this, 23)(this, font);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 25)(this, r, g, b, a);
	}

	void DrawSetTextColor(Color col)
	{
		typedef void(__thiscall* oDrawSetTextColor)(PVOID, Color);
		return g_Utils->get_vfunc< oDrawSetTextColor >(this, 24)(this, col);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 26)(this, x, y);
	}

	void DrawPrintText(const wchar_t* text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t*, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 28)(this, text, textLen, 0);
	}

	void DrawSetTexture(int textureID)
	{
		typedef void(__thiscall* oDrawSetTextColor)(PVOID, int);
		return g_Utils->get_vfunc< oDrawSetTextColor >(this, 38)(this, textureID);
	}

	void DrawSetTextureRGBA(int textureID, unsigned char  const* colors, int w, int h)
	{
		typedef void(__thiscall* oDrawSetTextColor)(PVOID, int, unsigned char  const*, int, int);
		return g_Utils->get_vfunc< oDrawSetTextColor >(this, 37)(this, textureID, colors, w, h);
	}

	int CreateNewTextureID(bool procedural)
	{
		typedef int(__thiscall* oDrawSetTextColor)(PVOID, bool);
		return g_Utils->get_vfunc< oDrawSetTextColor >(this, 43)(this, procedural);
	}

	void DrawTexturedPolygon(int vtxCount, FontVertex_t *vtx, bool bClipVertices = true)
	{
		typedef void(__thiscall* oDrawSetTextColor)(PVOID, int, FontVertex_t*, bool);
		return g_Utils->get_vfunc< oDrawSetTextColor >(this, 106)(this, vtxCount, vtx, bClipVertices);
	}

	unsigned long FontCreate()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return g_Utils->get_vfunc< OriginalFn >(this, 71)(this);
	}

	void SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		g_Utils->get_vfunc< OriginalFn >(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}

	void ISurface::GetTextSize(DWORD font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* OriginalFn)(void*, DWORD, const wchar_t*, int&, int&);
		return g_Utils->get_vfunc<OriginalFn>(this, 79)(this, font, text, wide, tall);
	}

	void DrawOutlinedCircle(int x, int y, int r, int seg)
	{
		typedef void(__thiscall* oDrawOutlinedCircle)(PVOID, int, int, int, int);
		return g_Utils->get_vfunc< oDrawOutlinedCircle >(this, 103)(this, x, y, r, seg);
	}

	void SurfaceGetCursorPos(int &x, int &y)
	{
		typedef void(__thiscall* oSurfaceGetCursorPos)(PVOID, int&, int&);
		return g_Utils->get_vfunc< oSurfaceGetCursorPos >(this, 66)(this, x, y);
	}

	void FilledRect(int x, int y, int w, int h, Color color)
	{
		DrawSetColor(color);
		DrawFilledRect(x, y, x + w, y + h);
	}

	void OutlinedRect(int x, int y, int w, int h, Color color)
	{
		DrawSetColor(color);
		DrawOutlinedRect(x, y, x + w, y + h);
	}

	void OutlinedVecRect(int x, int y, int x2, int y2, Color color)
	{
		DrawSetColor(color);
		DrawLine(x, y, x2, y);
		DrawLine(x2, y, x2, y2);
		DrawLine(x2, y2, x, y2);
		DrawLine(x, y2, x, y);
	}

	void Line(int x, int y, int x2, int y2, Color color)
	{
		DrawSetColor(color);
		DrawLine(x, y, x2, y2);
	}

	void DrawT(int X, int Y, Color Color, int Font, bool Center, const char* _Input, ...)
	{
		int apple = 0;
		/* set up buffer */
		char Buffer[2048] = { '\0' };

		/* set up varargs*/
		va_list Args;

		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);

		size_t Size = strlen(Buffer) + 1;

		/* set up widebuffer*/
		wchar_t* WideBuffer = new wchar_t[Size];

		/* char -> wchar */
		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

		/* check center */
		int Width = 0, Height = 0;

		if (Center)
		{
			GetTextSize(Font, WideBuffer, Width, Height);
		}

		/* call and draw*/
		DrawSetTextColor(Color.r(), Color.g(), Color.b(), Color.a());
		DrawSetTextFont(Font);
		DrawSetTextPos(X - (Width / 2), Y);
		DrawPrintText(WideBuffer, wcslen(WideBuffer));

		return;
	}

	RECT GetTextSizeRect(DWORD font, const char* text)
	{
		size_t origsize = strlen(text) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

		RECT rect; int x, y;
		GetTextSize(font, wcstring, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x;
		return rect;
	}

	Vector2D GetMousePosition()
	{
		POINT mouse_position;
		GetCursorPos(&mouse_position);
		ScreenToClient(FindWindow(0, "Counter-Strike: Global Offensive"), &mouse_position);
		return { static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y) };
	}

	bool MouseInRegion(int x, int y, int x2, int y2){
		if (GetMousePosition().x > x && GetMousePosition().y > y && GetMousePosition().x < x2 + x && GetMousePosition().y < y2 + y)
			return true;	
		return false;
	}
};