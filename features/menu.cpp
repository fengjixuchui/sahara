#define NOMINMAX

#include "menu.hpp"
#include <Windows.h>
#include <chrono>
#include "..//menuarrays.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

void Menu::Initialize()
{
	Visible = false;

	ImGui::CreateContext();
	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

	CreateStyle();
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::RenderMenu()
{
	if (!Visible) return;

	ImGui_ImplDX9_NewFrame();
	ImGui::GetIO().MouseDrawCursor = Visible;
	ImGui::SetNextWindowPosCenter(ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 250, 300 }, ImGuiSetCond_Once);

	if (ImGui::Begin("Sahara", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::CollapsingHeader("Aimbot"))
		{
			ImGui::Checkbox("Enable", &Feature.Enabled);

			ImGui::SliderInt("Hitchance", &Feature.Hitchance, 0, 100);
			ImGui::SliderInt("Min Dmg", &Feature.Damage, 0, 100);
		}
		if (ImGui::CollapsingHeader("Visuals"))
		{
			ImGui::Checkbox("Box", &Feature.Box);
			ImGui::Checkbox("Name", &Feature.Name);
			ImGui::Checkbox("Health", &Feature.Health);
			ImGui::Checkbox("Radar", &Feature.Radar);
			ImGui::Checkbox("Recoil", &Feature.Recoil);

			if (Feature.Recoil) ImGui::SliderInt("Size", &Feature.Size, 1, 15);
		}
		if (ImGui::CollapsingHeader("Misc"))
		{
			ImGui::Checkbox("Remove scope border", &Feature.RemoveScope);
			ImGui::Checkbox("Automatic bunnyhop", &Feature.AutoBunnyhop);
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::Show()
{
	Visible = true;
}

void Menu::Hide()
{
	Visible = false;
}

void Menu::Toggle()
{
	Visible = !Visible;
}

void Menu::CreateStyle()
{
	static int hue = 140;

	ImVec4 col_text = ImColor(150, 150, 150, 255);
	ImVec4 col_main = ImVec4(0.09f, .09f, .09f, 1.f);
	ImVec4 col_back = ImColor(31, 44, 54);
	ImVec4 col_area = ImColor(4, 32, 41);

	ImGui::GetIO().Fonts->AddFontDefault();

	Style.Alpha = 1.0f;                                // Global alpha applies to everything in ImGui
	Style.WindowPadding = ImVec2(7, 6);                      // Padding within a window
	Style.WindowMinSize = ImVec2(32, 32);                    // Minimum window size
	Style.WindowRounding = 4.0f;                                // Radius of window corners rounding. Set to 0.0f to have rectangular windows
	Style.WindowTitleAlign = ImVec2(0.006f, 0.5f);                  // Alignment for title bar text
	Style.FramePadding = ImVec2(4, 0);                        // Padding within a framed rectangle (used by most widgets)
	Style.FrameRounding = 10.0f;                                // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	Style.ItemSpacing = ImVec2(8, 4);                        // Horizontal and vertical spacing between widgets/lines
	Style.ItemInnerSpacing = ImVec2(4, 4);                        // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	Style.TouchExtraPadding = ImVec2(0, 0);                        // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	Style.IndentSpacing = 21.0f;                               // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	Style.ColumnsMinSpacing = 3.0f;                                // Minimum horizontal spacing between two columns
	Style.ScrollbarSize = 12.0f;                               // Width of the vertical scrollbar, Height of the horizontal scrollbar
	Style.ScrollbarRounding = 4.0f;                                // Radius of grab corners rounding for scrollbar
	Style.GrabMinSize = 10.0f;                               // Minimum width/height of a grab box for slider/scrollbar
	Style.GrabRounding = 10.0f;                                // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	Style.ButtonTextAlign = ImVec2(0.5f, 0.5f);                  // Alignment of button text when button is larger than text.
	Style.DisplayWindowPadding = ImVec2(22, 22);                      // Window positions are clamped to be IsVisible within the display area by at least this amount. Only covers regular windows.
	Style.DisplaySafeAreaPadding = ImVec2(4, 4);                        // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	Style.AntiAliasedLines = true;                                // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	Style.CurveTessellationTol = 1.25f;                               // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
	Style.WindowBorderSize = 1.2f;

	Style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_Text2] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_WindowBg] = ImColor(9, 9, 9, 255);
	Style.Colors[ImGuiCol_ChildWindowBg] = ImColor(10, 10, 10, 255);
	Style.Colors[ImGuiCol_PopupBg] = ImColor(4, 4, 4, 255);
	Style.Colors[ImGuiCol_Border] = ImColor(25, 25, 25, 255);
	Style.Colors[ImGuiCol_BorderShadow] = ImColor(25, 25, 25, 255);
	Style.Colors[ImGuiCol_FrameBg] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_FrameBgActive] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBg] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_TitleBgActive] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_MenuBarBg] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
	Style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_CheckMark] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_SliderGrab] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_SliderGrabActive] = ImColor(200, 200, 200, 255);
	Style.Colors[ImGuiCol_Button] = ImColor(37, 40, 47, 255);
	Style.Colors[ImGuiCol_ButtonHovered] = ImColor(57, 60, 67, 255);
	Style.Colors[ImGuiCol_ButtonActive] = ImColor(57, 60, 67, 255);
	Style.Colors[ImGuiCol_Header] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_HeaderHovered] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_HeaderActive] = ImColor(37, 40, 47, 170);
	Style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.f);
	Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	Style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	Style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	Style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	ImGui::GetStyle() = Style;
}
