#include "pomodoro.h"


Pomodoro::Pomodoro() :
	m_isPomodoroRunning(false),
	m_pomodoroMinutes(25.f),
	m_pomodoroTimeLeft(25.f *  60.f),
	m_pomodoroMaxTime(25.f * 60.f),
    m_lastUpdateTime(std::chrono::steady_clock::now()),
    m_elapsed(),
	minutes(0),
    seconds(0)
{

}

void Pomodoro::RenderPomodoroTimer()
{
	if (!showPomodoro) return;

    ImVec2 windowSize = ImVec2(300, 200);
    ImVec2 windowPos = ImVec2(1280 - windowSize.x - 20, 720 - windowSize.y - 70);

	ImGui::SetNextWindowPos(windowSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(windowPos, ImGuiCond_FirstUseEver);

    //ImGui::SetNextWindowPos(ImVec2(g_ResizeWidth / 1.52f + 20.f, g_ResizeHeight / 1.5f + 10.f), ImGuiCond_Always);
    //ImGui::SetNextWindowSize(ImVec2(g_ResizeWidth / 1.5f - 10.f, g_ResizeHeight), ImGuiCond_Always);

    ImGui::Begin("Pomodoro Timer", &showPomodoro, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    // Timer Update
    auto now = std::chrono::steady_clock::now();
    m_elapsed = now - m_lastUpdateTime;
    m_lastUpdateTime = now;

    if (m_isPomodoroRunning && m_pomodoroTimeLeft > 0.f)
    {
        m_pomodoroTimeLeft -= m_elapsed.count();
        if (m_pomodoroTimeLeft <= 0.f)
        {
            m_pomodoroTimeLeft = 0.f;
            m_isPomodoroRunning = false;
            PlaySound(TEXT("assets/jikankeika.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
    }

    // Time Display
    minutes = static_cast<int>(m_pomodoroTimeLeft) / 60;
    seconds = static_cast<int>(m_pomodoroTimeLeft) % 60;
    char timeStr[16];
    sprintf_s(timeStr, "%02d:%02d", minutes, seconds);

    ImVec2 textSize = ImGui::CalcTextSize(timeStr);
    float centerX = (ImGui::GetWindowSize().x, textSize.x) * 0.5f;

    if (g_largeFont) ImGui::PushFont(g_largeFont);
    ImGui::SetCursorPosX(centerX);
    ImGui::TextColored(ImVec4(1, 0.4f, 0.2f, 1), "%s", timeStr);
    if (g_largeFont) ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Duration setting
    if (!m_isPomodoroRunning)
    {
        ImGui::Text("Set Duration:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("##minutes", &m_pomodoroMinutes, 1, 5);
        m_pomodoroMinutes = std::clamp(m_pomodoroMinutes, 1, 60);

        if (ImGui::Button("Apply Duration"))
        {
            m_pomodoroMaxTime = static_cast<float>(m_pomodoroMinutes) * 60.f;
            m_pomodoroTimeLeft = m_pomodoroMaxTime;
        }
    }

    // Control Buttons
    ImGui::Spacing();
    float btnWidth = 60.f;
    //float spacing = (ImGui::GetWindowSize().x - btn_width * 2) / 3;

    ImGui::SetCursorPosX(centerX);
    if (ImGui::Button(m_isPomodoroRunning ? "Pause" : "Start", ImVec2(btnWidth, 40)))
    {
        if (!m_isPomodoroRunning)
        {
            PlaySound(TEXT("assets/jikansettei.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
        m_isPomodoroRunning = !m_isPomodoroRunning;
        m_lastUpdateTime = std::chrono::steady_clock::now();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(centerX * 2 + btnWidth);
    if (ImGui::Button("Reset", ImVec2(btnWidth, 40)))
    {
        m_pomodoroTimeLeft = m_pomodoroMaxTime;
        m_isPomodoroRunning = false;
    }

    ImGui::End();
}