#pragma once
#include "globals.h"

// Declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void RenderAppIdPopup();
void RenderShortcutUI();
void OpenURL(const char* url);

void LoadFonts();