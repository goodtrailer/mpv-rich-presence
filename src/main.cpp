#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>
#include <mpv/client.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <string>

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

    std::optional<bool> is_enabled = std::nullopt;

    bool media_has_audio = false;
    bool media_has_video = false;
    std::string media_artist = "";
    std::string media_title = "";
};

static void handle_client_message(rich_presence_state& state, mpv_event_client_message* msg)
{
    if (msg->num_args == 0)
        return;

    auto is_enabled_prev = state.is_enabled;

    if (msg->args[0] == "toggle"s)
    {
        state.is_enabled = !state.is_enabled.has_value() || !*state.is_enabled;
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
                mpv_print(state.mpv_client, std::format("Received application_id: {}", application_id));
            } catch (const std::exception& e) {
                mpv_print(state.mpv_client, e.what() + ": "s + msg->args[1]);
            }
        }
    }

    if (is_enabled_prev.has_value() && state.is_enabled.has_value() && state.is_enabled != is_enabled_prev)
        mpv_show(state.mpv_client, std::format("Rich presence {}", *state.is_enabled ? "enabled" : "disabled"));
}

static void handle_file_loaded(rich_presence_state& state)
{
    state.media_has_audio = false;
    state.media_has_video = false;

    int64_t media_track_count = 0;
    mpv_get_property(state.mpv_client, "track-list/count", MPV_FORMAT_INT64, &media_track_count);
    for (int64_t i = 0; i < media_track_count; i++) {
        const char* media_track_type = "";
        mpv_get_property(state.mpv_client, std::format("track-list/{}/type", i).c_str(), MPV_FORMAT_STRING, &media_track_type);
        if (media_track_type == "audio"s)
            state.media_has_audio = true;

        if (media_track_type == "video"s) {
            int is_image = false;
            mpv_get_property(state.mpv_client, std::format("track-list/{}/image", i).c_str(), MPV_FORMAT_FLAG, &is_image);
            if (!is_image)
                state.media_has_video = true;
        }
    }

    const char* media_artist = "";
    const char* media_title = "";
    mpv_get_property(state.mpv_client, "metadata/by-key/Artist", MPV_FORMAT_OSD_STRING, &media_artist);
    mpv_get_property(state.mpv_client, "media-title", MPV_FORMAT_OSD_STRING, &media_title);
    state.media_artist = media_artist == nullptr ? "" : media_artist;
    state.media_title = media_title == nullptr ? "" : media_title;
}

extern "C" MPV_EXPORT
int mpv_open_cplugin(mpv_handle* ctx)
{
    auto state = rich_presence_state {};
    state.mpv_client = ctx;

    // Initialization

    mpv_print(state.mpv_client, "Pinging rich_presence_conf for config data...");
    auto ping_args = std::array<const char*, 4> { "script-message-to", "rich_presence_conf", "ping", nullptr };
    mpv_command(state.mpv_client, ping_args.data());

    mpv_print(state.mpv_client, "Initializing Discord Social SDK...");
    state.discord_client = std::make_unique<discordpp::Client>();
    state.discord_client->AddLogCallback([&state](auto msg, auto severity)
    {
        mpv_print(state.mpv_client, std::format("[{}] {}", EnumToString(severity), msg));
    }, discordpp::LoggingSeverity::Warning);

    // Main event loop

    while (true)
    {
        discordpp::RunCallbacks();

        // Event handling (+ sleep)

        auto* event = mpv_wait_event(state.mpv_client, chrono::duration_cast<chrono::milliseconds>(SLEEP_DURATION).count() * 1e-3);

        if (event->event_id == MPV_EVENT_SHUTDOWN)
            break;
        else if (event->event_id == MPV_EVENT_CLIENT_MESSAGE)
            handle_client_message(state, (mpv_event_client_message*)event->data);
        else if (event->event_id == MPV_EVENT_FILE_LOADED)
            handle_file_loaded(state);

        // Rich Presence updating

        if (state.discord_client->GetApplicationId() == 0)
            continue;

        if (!state.is_enabled.has_value()
            || !*state.is_enabled
            || (!state.media_has_audio && !state.media_has_video))
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

        auto activity = discordpp::Activity {};
        activity.SetType(state.media_has_video ? discordpp::ActivityTypes::Watching : discordpp::ActivityTypes::Listening);
        activity.SetName(state.media_artist != "" ? std::format("{} - {}", state.media_artist, state.media_title) : state.media_title);
        activity.SetState(is_media_paused ? std::make_optional("Paused") : std::nullopt);

        if (!is_media_paused) {
            auto timestamps = discordpp::ActivityTimestamps {};
            uint64_t now_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
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
