#pragma once

#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>
#include <chrono>
#include <cstdint>
#include <d3d11.h>
#include <algorithm>
#include <mutex>
#include <tchar.h>

#pragma  comment(lib, "winmm.lib")

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

//Fonts & Icons
#include "fa.h"
#include "IconsFontAwesome6.h"

inline extern uint64_t					APPLICATION_ID = 0;	
inline extern bool						g_showAppIdPopup = true;
inline extern char						g_appIdBuffer[64] = "";
inline extern bool						g_appIdSubmitted = false;

/** ImGui D3D11 setup*/
// Data
inline extern ID3D11Device*						g_pd3dDevice = nullptr;
inline extern ID3D11DeviceContext*				g_pd3dDeviceContext = nullptr;
inline extern IDXGISwapChain*					g_pSwapChain = nullptr;
inline extern bool								g_SwapChainOccluded = false;
inline extern UINT								g_ResizeWidth =0, g_ResizeHeight=0;
inline extern ID3D11RenderTargetView*			g_mainRenderTargetView = nullptr;

inline ImFont*									g_largeFont = nullptr;
inline extern bool								showPomodoro = false;