#pragma once
#include "globals.h"
#include "discordpp.h"


static bool g_clientStarted = false;

// Create a flag to stop the application
inline std::atomic<bool> running = true;
inline std::vector<std::string> logMessages;
inline std::mutex logMutex;

void signalHandler(int signum);
void DiscordCallback();
void AddLog(const std::string& msg);
void RenderUI(discordpp::Client::Status clientStatus, const std::string& presenceStatus);