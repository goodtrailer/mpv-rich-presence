// This file is part of mpv-rich-presence.
// Copyright (c) 2026 Alden Wu.
//
// mpv-rich-presence is free software: you can redistribute it and/or modify it
// under the terms of the GNU Affero General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// mpv-rich-presence is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
// for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with mpv-rich-presence. If not, see <https://www.gnu.org/licenses/>.

#include <cdiscord.h>
#include <mpv/client.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <functional>
#include <string>

#include "mpv_rich_presence/mpv_utils.hpp"
#include "mpv_rich_presence/rich_presence_state.hpp"

namespace chrono = std::chrono;
using namespace std::literals;
using namespace mpvrp;

static constexpr chrono::seconds SLEEP_DURATION = chrono::seconds(1);

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
            mpv_print(state.mpv, "Not enough args for application_id");
        }
        else
        {
            try
            {
                int64_t application_id = std::stoll(msg->args[1]);
                Discord_Client_SetApplicationId(&state.discord->get(), application_id);
                mpv_print(state.mpv, std::format("Received application_id: {}", application_id));
            }
            catch (const std::exception& e)
            {
                mpv_print(state.mpv, e.what() + ": "s + msg->args[1]);
            }
        }
    }

    if (is_enabled_prev.has_value() && state.is_enabled.has_value() && state.is_enabled != is_enabled_prev)
        mpv_show(state.mpv, std::format("Rich presence {}", *state.is_enabled ? "enabled" : "disabled"));
}

static void handle_file_loaded(rich_presence_state& state)
{
    state.media_has_audio = false;
    state.media_has_video = false;

    int64_t media_track_count = 0;
    mpv_get_property(state.mpv, "track-list/count", MPV_FORMAT_INT64, &media_track_count);
    for (int64_t i = 0; i < media_track_count; i++)
    {
        const char* media_track_type = "";
        mpv_get_property(state.mpv, std::format("track-list/{}/type", i).c_str(), MPV_FORMAT_STRING, &media_track_type);
        if (media_track_type == "audio"s)
            state.media_has_audio = true;

        if (media_track_type == "video"s)
        {
            int is_image = 0;
            mpv_get_property(state.mpv, std::format("track-list/{}/image", i).c_str(), MPV_FORMAT_FLAG, &is_image);
            if (is_image == 0)
                state.media_has_video = true;
        }
    }

    const char* media_artist = "";
    const char* media_title = "";
    mpv_get_property(state.mpv, "metadata/by-key/Artist", MPV_FORMAT_OSD_STRING, &media_artist);
    mpv_get_property(state.mpv, "media-title", MPV_FORMAT_OSD_STRING, &media_title);
    state.media_artist = media_artist == nullptr ? "" : media_artist;
    state.media_title = media_title == nullptr ? "" : media_title;
}

void on_discord_log(Discord_String msg, Discord_LoggingSeverity severity, void* payload)
{
    auto& state = *(rich_presence_state*)payload;

    auto severity_str = ""s;
    switch (severity)
    {
    case Discord_LoggingSeverity_Verbose:
        severity_str = "Verbose";
        break;
    case Discord_LoggingSeverity_Info:
        severity_str = "Info";
        break;
    case Discord_LoggingSeverity_Warning:
        severity_str = "Warning";
        break;
    case Discord_LoggingSeverity_Error:
        severity_str = "Error";
        break;
    case Discord_LoggingSeverity_None:
        severity_str = "None";
        break;
    default:
        severity_str = "unknown";
        break;
    }

    auto msg_view = std::string_view { (char*)msg.ptr, msg.size };
    mpv_print(state.mpv, std::format("[{}] {}", severity_str, msg_view));
}

void on_discord_update_rich_presence(Discord_ClientResult* result, void* payload)
{
    auto& state = *(rich_presence_state*)payload;

    if (Discord_ClientResult_Successful(result))
        return;

    int32_t code = Discord_ClientResult_ErrorCode(result);
    mpv_print(state.mpv, std::format("Rich presence error: {}", code));
}

extern "C" MPV_EXPORT auto mpv_open_cplugin(mpv_handle* ctx) -> int
{
    auto state = rich_presence_state {};
    state.mpv = ctx;

    // Initialization

    mpv_print(state.mpv, "Pinging rich_presence_conf for config data...");
    auto ping_args = std::array<const char*, 4> { "script-message-to", "rich_presence_conf", "ping", nullptr };
    mpv_command(state.mpv, ping_args.data());

    mpv_print(state.mpv, "Initializing Discord Social SDK...");
    state.discord = std::make_unique<discord_client>();

    Discord_Client_AddLogCallback(&state.discord->get(), on_discord_log, nullptr, &state, Discord_LoggingSeverity_Warning);

    // Main event loop

    while (true)
    {
        Discord_RunCallbacks();

        // Event handling (+ sleep)

        auto* event = mpv_wait_event(state.mpv, chrono::duration_cast<chrono::milliseconds>(SLEEP_DURATION).count() * 1e-3);

        if (event->event_id == MPV_EVENT_SHUTDOWN)
            break;

        if (event->event_id == MPV_EVENT_CLIENT_MESSAGE)
            handle_client_message(state, (mpv_event_client_message*)event->data);
        else if (event->event_id == MPV_EVENT_FILE_LOADED)
            handle_file_loaded(state);

        // Rich Presence updating

        if (Discord_Client_GetApplicationId(&state.discord->get()) == 0)
            continue;

        if (!state.is_enabled.has_value()
            || !*state.is_enabled
            || (!state.media_has_audio && !state.media_has_video))
        {
            Discord_Client_ClearRichPresence(&state.discord->get());
            continue;
        }

        double media_time_pos_s = 0.0;
        double media_time_left_s = 0.0;
        int is_media_paused = 0;
        mpv_get_property(state.mpv, "time-pos/full", MPV_FORMAT_DOUBLE, &media_time_pos_s);
        mpv_get_property(state.mpv, "time-remaining/full", MPV_FORMAT_DOUBLE, &media_time_left_s);
        mpv_get_property(state.mpv, "pause", MPV_FORMAT_FLAG, &is_media_paused);

        auto activity = discord_activity {};
        auto activity_type = state.media_has_video ? Discord_ActivityTypes_Watching : Discord_ActivityTypes_Listening;
        auto activity_name = !state.media_artist.empty() ? std::format("{} - {}", state.media_artist, state.media_title) : state.media_title;
        static auto paused_state = Discord_String { (uint8_t*)"Paused", sizeof("Paused") - 1 };
        Discord_Activity_SetType(&activity.get(), activity_type);
        Discord_Activity_SetName(&activity.get(), { (uint8_t*)activity_name.data(), activity_name.size() });
        Discord_Activity_SetState(&activity.get(), is_media_paused == 1 ? &paused_state : nullptr);

        if (is_media_paused == 0)
        {
            auto timestamps = discord_activity_timestamps {};
            uint64_t now_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            Discord_ActivityTimestamps_SetStart(&timestamps.get(), now_ms - (media_time_pos_s * 1'000));
            Discord_ActivityTimestamps_SetEnd(&timestamps.get(), now_ms + (media_time_left_s * 1'000));
            Discord_Activity_SetTimestamps(&activity.get(), &timestamps.get());
        }

        Discord_Client_UpdateRichPresence(&state.discord->get(), &activity.get(), on_discord_update_rich_presence, nullptr, &state);
    }

    // Shut down

    mpv_print(state.mpv, "Shutting down...");
    Discord_Client_ClearRichPresence(&state.discord->get());

    return EXIT_SUCCESS;
}
