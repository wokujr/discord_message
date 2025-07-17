#define DISCORDPP_IMPLEMENTATION
#include "discord_client.h"
#include "gui.h"
#include "pomodoro.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //create ImGui application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("DiscordSDKApp"), nullptr };
	RegisterClassEx(&wc);
	HWND hWnd = ::CreateWindowW(wc.lpszClassName, _T("Discord SDK App"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720, nullptr, nullptr, wc.hInstance, nullptr);

	if (!CreateDeviceD3D(hWnd))
    {
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
    }

	//Show the ImGui window
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

  
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    LoadFonts();

	ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    //DiscordCallback();

    //Class set
    discordpp::Client::Status clientStatus = discordpp::Client::Status::Connected;
    std::string presenceStatus = "Connected " ICON_FA_DOLLAR_SIGN;
    Pomodoro PomodoroTimer;

    // Keep application running to allow SDK to receive events and callbacks
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }

        discordpp::RunCallbacks();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //Render Windows
        if (APPLICATION_ID == 0 && !g_appIdSubmitted)
        {
            RenderAppIdPopup();
        }
        else if (!g_clientStarted && g_appIdSubmitted)
        {
            g_clientStarted = true;
            DiscordCallback();
        }

        //Main window
        if (g_clientStarted)
        {
	        RenderUI(clientStatus, presenceStatus);
			RenderShortcutUI();
            PomodoroTimer.RenderPomodoroTimer();
        }

        /*ImGui::Begin("Debug");
        ImGui::Text("App ID: %llu", APPLICATION_ID);
        ImGui::Text("Submitted: %s", g_appIdSubmitted ? "true" : "false");
        ImGui::Text("Client Started: %s", g_clientStarted ? "true" : "false");
        ImGui::End();*/

        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

	//Cleanup ImGui
    ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

    CleanupDeviceD3D();
	::DestroyWindow(hWnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}
