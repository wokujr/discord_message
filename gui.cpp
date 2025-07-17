#include "gui.h"
#include "discord_client.h"
#include "pomodoro.h"
#include "roboto.h"

// Setup Dear ImGui context
bool CreateDeviceD3D(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
	default: ;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void RenderAppIdPopup()
{
	if (g_showAppIdPopup)
	{
        ImGui::OpenPopup("Enter Application ID");
        g_showAppIdPopup = false;
	}

    bool isOpen = true;
	if (ImGui::BeginPopupModal("Enter Application ID", &isOpen, ImGuiWindowFlags_AlwaysAutoResize))
	{
        ImGui::Text("Enter your Discord application id");
        ImGui::InputText("##appid", g_appIdBuffer, IM_ARRAYSIZE(g_appIdBuffer), ImGuiInputTextFlags_CharsDecimal);

        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120.f, 0.f)))
        {
	        try
	        {
                APPLICATION_ID = std::stoull(g_appIdBuffer);
                if (APPLICATION_ID != 0)
                {
                    g_appIdSubmitted = true;
                    ImGui::CloseCurrentPopup();
                }
	        }
            catch (...)
            {
                AddLog("Invalid Application ID input!");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.f, 0.f)))
        {
            exit(0);
        }
        ImGui::EndPopup();
	}
}

void RenderShortcutUI()
{
    ImGui::SetNextWindowPos(ImVec2(g_ResizeWidth / 1.5f + 10.f, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(g_ResizeWidth / 1.5f - 20.f, g_ResizeHeight/1.5f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.1f, 0.15f, 1.f));
    ImGui::Begin("Quick Links");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE " Youtube"))
    {
        OpenURL("https://www.youtube.com");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE" ChatGPT")) {
        OpenURL("https://chat.openai.com");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

	ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.4f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.2f, 0.5f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE " Instagram")) {
        OpenURL("https://www.instagram.com");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::Dummy(ImVec2(0.f, 4.f));

    // File Explorer (Default C: drive)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.7f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    if (ImGui::Button(ICON_FA_FOLDER" Explorer")) {
        OpenURL("shell:MyComputerFolder");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::Dummy(ImVec2(0.f, 4.f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.7f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20.f);
    if (ImGui::Button(ICON_FA_BUSINESS_TIME" Pomodoro"))
    {
        showPomodoro = true;
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::End();
    ImGui::PopStyleColor();
}

void OpenURL(const char* url) {
#if defined(_WIN32)
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    std::string command = "xdg-open \"" + std::string(url) + "\ "";
    system(command.c_str());
#else
#error Platform not supported!
#endif

}

void LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImFontConfig fontConfig;
    fontConfig.MergeMode = false; // base font
    fontConfig.PixelSnapH = true;

    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_compressed_data, roboto_compressed_size, 16.f, &fontConfig);

    // Load icon font merged into base font
    ImFontConfig iconConfig;
    iconConfig.MergeMode = true;
    iconConfig.PixelSnapH = true;
    static constexpr ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromMemoryCompressedTTF(MyFont_compressed_data, MyFont_compressed_size, 18.f, &iconConfig, iconsRanges);

    // 🔸 Add large font for timer (no merge)
    ImFontConfig bigFontConfig;
    bigFontConfig.MergeMode = false;
    bigFontConfig.PixelSnapH = true;
    g_largeFont = io.Fonts->AddFontFromMemoryCompressedTTF(roboto_compressed_data, roboto_compressed_size, 48.f, &bigFontConfig);

    // Don't forget to rebuild atlas
    io.Fonts->Build();
}
