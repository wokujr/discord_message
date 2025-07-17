#include "discord_client.h"
#include "gui.h"

#include <csignal>


void signalHandler(int signum) {
    running.store(false);
}

void AddLog(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logMessages.push_back(msg);
}

void RenderUI(discordpp::Client::Status clientStatus, const std::string& presenceStatus)
{
    bool autoScroll = true;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(g_ResizeWidth / 1.5f, g_ResizeHeight), ImGuiCond_Always);

    ImGui::Begin("Discord SDK app");

    ImGui::Text("Client Status: %s", discordpp::Client::StatusToString(clientStatus).c_str());

    ImGui::Text("Presence: %s", presenceStatus.c_str());

    ImGui::Separator();
    ImGui::Text("Log Output");
    ImGui::BeginChild("LogRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    {
        std::lock_guard<std::mutex> lock(logMutex);
        for (auto& log : logMessages)
        {
            ImGui::TextWrapped("%s", log.c_str());
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

    }
    ImGui::EndChild();
    //ImGui::Checkbox("Auto-Scroll", &autoScroll);

    ImGui::End();
}


void DiscordCallback()
{
    std::signal(SIGINT, signalHandler);
    AddLog("Initializing Discord SDK...");

    // Create our Discord Client
    auto client = std::make_shared<discordpp::Client>();

    // Set up logging callback
    client->AddLogCallback([](auto message, auto severity) {
        AddLog("[" + std::string(EnumToString(severity)) + "] " + std::string(message));
        }, discordpp::LoggingSeverity::Info);

    // Set up status callback to monitor client connection
    client->SetStatusChangedCallback([client](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) {
        AddLog("Status changed: " + discordpp::Client::StatusToString(status));

        if (status == discordpp::Client::Status::Ready) {
            AddLog("Client is ready! You can now call SDK functions.");

            discordpp::Activity activity{};
            activity.SetName("Playing Mumet");
            activity.SetState("In Mumet State");
            activity.SetDetails("Shin no Shitake Hunter");

            client->UpdateRichPresence(activity, [](discordpp::ClientResult result)
                {
                    if (result.Successful())
                    {
                        //std::cout << "Rich presence updated successfully!\n";
                        AddLog("Rich presence updated successfully!");
                    }
                    else
                    {
                        //std::cerr << "Failed to update rich presence: " << result.Error() << std::endl;
                        AddLog("Failed to update rich presence: " + result.Error());
                    }
                });
        }
        else if (error != discordpp::Client::Error::None) {
            //std::cerr << "Connection Error: " << discordpp::Client::ErrorToString(error) << " - Details: " << errorDetail << std::endl;
            AddLog("Connection Error: " + discordpp::Client::ErrorToString(error) + " - Details: " + std::to_string(errorDetail));
        }
        });

    // Generate OAuth2 code verifier for authentication
    auto codeVerifier = client->CreateAuthorizationCodeVerifier();

    // Set up authentication arguments
    discordpp::AuthorizationArgs args{};
    args.SetClientId(APPLICATION_ID);
    args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
    args.SetCodeChallenge(codeVerifier.Challenge());

    // Begin authentication process
    client->Authorize(args, [client, codeVerifier](auto result, auto code, auto redirectUri) {
        if (!result.Successful()) {
            //std::cerr << "Authentication Error: " << result.Error() << std::endl;
            AddLog("Authentication Error: " + result.Error());
            return;
        }
        else {
            //std::cout << "Authorization successful! Getting access token...\n";
            AddLog("Authorization successful! Getting access token...");

            // Exchange auth code for access token
            client->GetToken(APPLICATION_ID, code, codeVerifier.Verifier(), redirectUri,
                [client](discordpp::ClientResult result,
                    std::string accessToken,
                    std::string refreshToken,
                    discordpp::AuthorizationTokenType tokenType,
                    int32_t expiresIn,
                    std::string scope) {
                        //std::cout << "Access token received! Establishing connection...\n";
                        AddLog("Access token received! Establish connection... \n");

                        // Next Step: Update the token and connect
                        client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [client](discordpp::ClientResult result) {
                            if (result.Successful()) {
                                //std::cout << "Token updated, connecting to Discord...\n";
                                AddLog("Token updated, connecting to Discord...\n");
                                client->Connect();
                            }
                            });
                });
        }
        });
}
