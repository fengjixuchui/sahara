#include "Visuals.hpp"
#include "../hooks.hpp"
#include "Aimbot.hpp"

void Render::CreateFonts()
{
	Visuals = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(Visuals, "Tahoma", 12, 400, 0, 0, FONTFLAG_OUTLINE);
}

void Render::Text(int X, int Y, const char* Text, vgui::HFont Font, Color DrawColor, bool Center)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());

	g_VGuiSurface->DrawSetTextFont(Font);
	g_VGuiSurface->DrawSetTextColor(DrawColor);

	if (Center)
	{
		int TextWidth, TextHeight;
		Render::Get().TextSize(TextWidth, TextHeight, Text, Font);
		g_VGuiSurface->DrawSetTextPos(X - TextWidth / 2, Y);
	}
	else
		g_VGuiSurface->DrawSetTextPos(X, Y);
	g_VGuiSurface->DrawPrintText(WText.c_str(), wcslen(WText.c_str()));
}

void Render::TextSize(int& Width, int& Height, const char* Text, vgui::HFont Font)
{
	std::wstring WText = std::wstring(std::string_view(Text).begin(), std::string_view(Text).end());
	g_VGuiSurface->GetTextSize(Font, WText.c_str(), Width, Height);
}

void Render::FilledRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawFilledRect(X1, Y1, X2, Y2);
}

void Render::OutlinedRectange(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawOutlinedRect(X1, Y1, X2, Y2);
}

void Render::Line(int X1, int Y1, int X2, int Y2, Color DrawColor)
{
	g_VGuiSurface->DrawSetColor(DrawColor);
	g_VGuiSurface->DrawLine(X1, Y1, X2, Y2);
}

RECT Visuals::GetBBox(C_BasePlayer* Player, Vector TransformedPoints[])
{
	RECT rect{};
	ICollideable* collideable = Player->GetCollideable();

	if (!collideable) return rect;

	Vector min = collideable->OBBMins();
	Vector max = collideable->OBBMaxs();

	const matrix3x4_t& trans = Player->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];

	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = Player->GetAbsOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i])) return rect;
		else TransformedPoints[i] = screen_points[i];

	float left = screen_points[0].x;
	float top = screen_points[0].y;
	float right = screen_points[0].x;
	float bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x) left = screen_points[i].x;
		if (top < screen_points[i].y) top = screen_points[i].y;
		if (right < screen_points[i].x) right = screen_points[i].x;
		if (bottom > screen_points[i].y) bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

bool Visuals::Begin(C_BasePlayer* Player)
{
	Context.Player = Player;

	if (!Context.Player->IsEnemy()) return false;
	if (Context.Player->IsDormant()) return false;

	Vector head = Context.Player->GetHitboxPos(HITBOX_HEAD);
	Vector origin = Context.Player->GetAbsOrigin();

	head.z += 15;

	if (!Math::WorldToScreen(head, Context.HeadPos) || !Math::WorldToScreen(origin, Context.Origin)) return false;

	Vector points_transformed[8];
	RECT Box = GetBBox(Context.Player, points_transformed);

	Context.Box = Box;
	Context.Box.top = Box.bottom;
	Context.Box.bottom = Box.top;

	return true;
}

void Visuals::Box()
{
	Render::Get().OutlinedRectange(Context.Box.left - 1, Context.Box.top - 1, Context.Box.right + 1, Context.Box.bottom + 1, Color::Black);
	Render::Get().OutlinedRectange(Context.Box.left + 1, Context.Box.top + 1, Context.Box.right - 1, Context.Box.bottom - 1, Color::Black);
	Render::Get().OutlinedRectange(Context.Box.left, Context.Box.top, Context.Box.right, Context.Box.bottom, Color::White);
}

void Visuals::Name()
{
	player_info_t PlayerInfo;
	g_EngineClient->GetPlayerInfo(Context.Player->EntIndex(), &PlayerInfo);

	int TextWidth, TextHeight;
	Render::Get().TextSize(TextWidth, TextHeight, PlayerInfo.szName, Render::Get().Visuals);
	Render::Get().Text(Context.Box.left + (Context.Box.right - Context.Box.left) / 2, Context.Box.top - TextHeight, PlayerInfo.szName, Render::Get().Visuals, Color(255, 255, 255, 255), true);
}

void Visuals::Recoil()
{
	if (!g_LocalPlayer->IsAlive()) return;

	int w, h;

	g_EngineClient->GetScreenSize(w, h);
	g_VGuiSurface->DrawSetColor(Color::White);

	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;

	QAngle punchAngle = g_LocalPlayer->m_aimPunchAngle();

	x -= dx * punchAngle.yaw;
	y += dy * punchAngle.pitch;

	g_VGuiSurface->DrawLine(x - Feature.Size, y, x + Feature.Size + 1, y);
	g_VGuiSurface->DrawLine(x, y - Feature.Size, x, y + Feature.Size + 1);
}

void Visuals::Health()
{
	int HealthValue = Context.Player->m_iHealth();
	std::clamp(HealthValue, 0, 100);

	float Height = (Context.Box.bottom - Context.Box.top) * float(HealthValue / 100.f);

	Render::Get().FilledRectange(Context.Box.left - 7, Context.Box.top - 1, Context.Box.left - 2, Context.Box.bottom + 1, Color(0, 0, 0, 150));
	Render::Get().FilledRectange(Context.Box.left - 6, Context.Box.top, Context.Box.left - 3, Context.Box.top + Height, Color(135, 235, 50, 255));
}

void Visuals::Radar()
{
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		if (i == g_EngineClient->GetLocalPlayer()) continue;

		C_BasePlayer* pPlayer = reinterpret_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!pPlayer) continue; pPlayer->m_bSpotted() = true;
	}
}