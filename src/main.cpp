#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>
#include <mpv/client.h>

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace chrono = std::chrono;
using namespace std::literals;

static constexpr chrono::seconds SLEEP_DURATION = chrono::seconds(1);

static int mpv_print(mpv_handle* ctx, std::string_view str)
{
    auto full_str = std::format("[rich-presence] {}", str);
    auto args = std::array<const char*, 3> { "print-text", full_str.c_str(), nullptr };
    return mpv_command(ctx, args.data());
}

static int mpv_show(mpv_handle* ctx, const std::string& str)
{
    auto args = std::array<const char*, 3> { "show-text", str.c_str(), nullptr };
    return mpv_command(ctx, args.data());
}

struct rich_presence_state {
    mpv_handle* mpv_client;
    std::unique_ptr<discordpp::Client> discord_client;
    bool is_enabled = false;
};

static void handle_client_message(rich_presence_state& state, mpv_event_client_message* msg)
{
    if (msg->num_args == 0)
        return;

    bool is_enabled_prev = state.is_enabled;

    if (msg->args[0] == "toggle"s)
    {
        state.is_enabled = !state.is_enabled;
    }
    else if (msg->args[0] == "on"s)
    {
        state.is_enabled = true;
    }
    else if (msg->args[0] == "off"s)
    {
        state.is_enabled = false;
    }
    else if (msg->args[0] == "application_id"s)
    {
        if (msg->num_args < 2)
        {
            mpv_print(state.mpv_client, "Not enough args for application_id");
        }
        else
        {
            try {
                int64_t application_id = std::stoll(msg->args[1]);
                state.discord_client->SetApplicationId(application_id);
                mpv_print(state.mpv_client, std::format("application_id: {}", application_id));
            } catch (const std::exception& e) {
                mpv_print(state.mpv_client, e.what() + ": "s + msg->args[1]);
            }
        }
    }

    if (state.is_enabled != is_enabled_prev)
        mpv_show(state.mpv_client, std::format("Rich presence {}", state.is_enabled ? "enabled" : "disabled"));
}

extern "C" MPV_EXPORT
int mpv_open_cplugin(mpv_handle* ctx)
{
    // Initialization

    auto state = rich_presence_state {};
    state.mpv_client = ctx;

    mpv_print(state.mpv_client, "Initializing Discord Social SDK...");

    state.discord_client = std::make_unique<discordpp::Client>();

    state.discord_client->AddLogCallback([&state](auto msg, auto severity) {
        mpv_print(state.mpv_client, std::format("[{}] {}", EnumToString(severity), msg));
    }, discordpp::LoggingSeverity::Warning);

    auto ping_args = std::array<const char*, 4> { "script-message-to", "rich_presence_conf", "ping", nullptr };
    mpv_command(state.mpv_client, ping_args.data());

    // Main event loop

    while (true)
    {
        auto* event = mpv_wait_event(state.mpv_client, chrono::duration_cast<chrono::milliseconds>(SLEEP_DURATION).count() * 1e-3);
        
        discordpp::RunCallbacks();

        if (event->event_id == MPV_EVENT_SHUTDOWN)
            break;
        
        if (event->event_id == MPV_EVENT_CLIENT_MESSAGE)
            handle_client_message(state, (mpv_event_client_message*)event->data);

        if (state.discord_client->GetApplicationId() == 0)
            continue;

        if (!state.is_enabled)
        {
            state.discord_client->ClearRichPresence();
            continue;
        }

        auto activity = discordpp::Activity {};

        bool has_audio = false;
        bool has_video = false;
        int64_t media_track_count = 0;
        mpv_get_property(state.mpv_client, "track-list/count", MPV_FORMAT_INT64, &media_track_count);
        for (int64_t i = 0; i < media_track_count; i++) {
            const char* media_track_type = "";
            mpv_get_property(state.mpv_client, std::format("track-list/{}/type", i).c_str(), MPV_FORMAT_STRING, &media_track_type);
            if (media_track_type == "audio"s)
                has_audio = true;

            if (media_track_type == "video"s) {
                int is_image = false;
                mpv_get_property(state.mpv_client, std::format("track-list/{}/image", i).c_str(), MPV_FORMAT_FLAG, &is_image);
                if (!is_image)
                    has_video = true;
            }
        }

        if (has_video)
        {
            activity.SetType(discordpp::ActivityTypes::Watching);
        }
        else if (has_audio)
        {
            activity.SetType(discordpp::ActivityTypes::Listening);
        }
        else
        {
            state.discord_client->ClearRichPresence();
            continue;
        }

        double media_time_pos_s = 0.0;
        double media_time_left_s = 0.0;
        int is_media_paused = false;
        mpv_get_property(state.mpv_client, "time-pos/full", MPV_FORMAT_DOUBLE, &media_time_pos_s);
        mpv_get_property(state.mpv_client, "time-remaining/full", MPV_FORMAT_DOUBLE, &media_time_left_s);
        mpv_get_property(state.mpv_client, "pause", MPV_FORMAT_FLAG, &is_media_paused);

        const char* media_artist = "";
        const char* media_title = "";
        mpv_get_property(state.mpv_client, "metadata/by-key/Artist", MPV_FORMAT_OSD_STRING, &media_artist);
        mpv_get_property(state.mpv_client, "media-title", MPV_FORMAT_OSD_STRING, &media_title);

        activity.SetName(media_artist != "" ? std::format("{} - {}", media_artist, media_title) : media_title);
        activity.SetState(is_media_paused ? std::make_optional("Paused") : std::nullopt);

        auto assets = discordpp::ActivityAssets {};
        assets.SetLargeImage(std::nullopt);
        activity.SetAssets(assets);

        if (!is_media_paused) {
            uint64_t now_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            auto timestamps = discordpp::ActivityTimestamps {};
            timestamps.SetStart(now_ms - media_time_pos_s * 1000);
            timestamps.SetEnd(now_ms + media_time_left_s * 1000);
            activity.SetTimestamps(timestamps);
        }

        state.discord_client->UpdateRichPresence(activity, [&state](discordpp::ClientResult result)
        {
            if (!result.Successful())
                mpv_print(state.mpv_client, std::format("Rich presence error: {}", result.ErrorCode()));
        });
    }

    // Shut down

    mpv_print(state.mpv_client, "Shutting down...");
    state.discord_client->ClearRichPresence();

    return EXIT_SUCCESS;
}
