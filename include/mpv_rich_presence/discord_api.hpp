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

#pragma once

#include <boost/dll.hpp>

#include <cstdint>
#include <functional>

namespace mpvrp
{
    struct Discord_Activity
    {
        void* opaque;
    };

    struct Discord_ActivityTimestamps
    {
        void* opaque;
    };

    struct Discord_Client
    {
        void* opaque;
    };

    struct Discord_ClientResult
    {
        void* opaque;
    };

    struct Discord_String
    {
        const char* data;
        size_t size;
    };

    enum class Discord_LoggingSeverity : int32_t
    {
        Verbose = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        None = 5,
    };

    enum class Discord_ActivityTypes : int32_t
    {
        Playing = 0,
        Streaming = 1,
        Listening = 2,
        Watching = 3,
        CustomStatus = 4,
        Competing = 5,
        HangStatus = 6,
    };

    using Discord_Client_LogCallback = void(Discord_String message, Discord_LoggingSeverity severity, void* userData);
    using Discord_Client_UpdateRichPresenceCallback = void(Discord_ClientResult* result, void* userData);
    using Discord_FreeFn = void(void* ptr);

    class discord_api
    {
    public:
        discord_api(const boost::dll::shared_library& discord_social_sdk);

        void Discord_Activity_Drop(Discord_Activity* self) const;
        void Discord_Activity_Init(Discord_Activity* self) const;
        void Discord_Activity_SetName(Discord_Activity* self, Discord_String value) const;
        void Discord_Activity_SetState(Discord_Activity* self, Discord_String* value) const;
        void Discord_Activity_SetTimestamps(Discord_Activity* self, Discord_ActivityTimestamps* value) const;
        void Discord_Activity_SetType(Discord_Activity* self, Discord_ActivityTypes value) const;
        void Discord_ActivityTimestamps_Drop(Discord_ActivityTimestamps* self) const;
        void Discord_ActivityTimestamps_Init(Discord_ActivityTimestamps* self) const;
        void Discord_ActivityTimestamps_SetEnd(Discord_ActivityTimestamps* self, uint64_t value) const;
        void Discord_ActivityTimestamps_SetStart(Discord_ActivityTimestamps* self, uint64_t value) const;
        void Discord_Client_AddLogCallback(Discord_Client* self, Discord_Client_LogCallback callback, Discord_FreeFn callback__userDataFree, void* callback__userData, Discord_LoggingSeverity minSeverity) const;
        void Discord_Client_ClearRichPresence(Discord_Client* self) const;
        auto Discord_Client_GetApplicationId(Discord_Client* self) const -> uint64_t;
        void Discord_Client_Drop(Discord_Client* self) const;
        void Discord_Client_Init(Discord_Client* self) const;
        void Discord_Client_SetApplicationId(Discord_Client* self, uint64_t applicationId) const;
        void Discord_Client_UpdateRichPresence(Discord_Client* self, Discord_Activity* activity, Discord_Client_UpdateRichPresenceCallback cb, Discord_FreeFn cb__userDataFree, void* cb__userData) const;
        auto Discord_ClientResult_ErrorCode(Discord_ClientResult* self) const -> int32_t;
        auto Discord_ClientResult_Successful(Discord_ClientResult* self) const -> bool;
        void Discord_RunCallbacks() const;

    public:
        std::function<void(Discord_Activity*)> Discord_Activity_Drop_impl;
        std::function<void(Discord_Activity*)> Discord_Activity_Init_impl;
        std::function<void(Discord_Activity* self, Discord_String value)> Discord_Activity_SetName_impl;
        std::function<void(Discord_Activity* self, Discord_String* value)> Discord_Activity_SetState_impl;
        std::function<void(Discord_Activity* self, Discord_ActivityTimestamps* value)> Discord_Activity_SetTimestamps_impl;
        std::function<void(Discord_Activity* self, Discord_ActivityTypes value)> Discord_Activity_SetType_impl;
        std::function<void(Discord_ActivityTimestamps* self)> Discord_ActivityTimestamps_Drop_impl;
        std::function<void(Discord_ActivityTimestamps* self)> Discord_ActivityTimestamps_Init_impl;
        std::function<void(Discord_ActivityTimestamps* self, uint64_t value)> Discord_ActivityTimestamps_SetEnd_impl;
        std::function<void(Discord_ActivityTimestamps* self, uint64_t value)> Discord_ActivityTimestamps_SetStart_impl;
        std::function<void(Discord_Client* self, Discord_Client_LogCallback callback, Discord_FreeFn callback__userDataFree, void* callback__userData, Discord_LoggingSeverity minSeverity)> Discord_Client_AddLogCallback_impl;
        std::function<void(Discord_Client* self)> Discord_Client_ClearRichPresence_impl;
        std::function<uint64_t(Discord_Client* self)> Discord_Client_GetApplicationId_impl;
        std::function<void(Discord_Client* self)> Discord_Client_Drop_impl;
        std::function<void(Discord_Client* self)> Discord_Client_Init_impl;
        std::function<void(Discord_Client* self, uint64_t applicationId)> Discord_Client_SetApplicationId_impl;
        std::function<void(Discord_Client* self, Discord_Activity* activity, Discord_Client_UpdateRichPresenceCallback cb, Discord_FreeFn cb__userDataFree, void* cb__userData)> Discord_Client_UpdateRichPresence_impl;
        std::function<int32_t(Discord_ClientResult* self)> Discord_ClientResult_ErrorCode_impl;
        std::function<bool(Discord_ClientResult* self)> Discord_ClientResult_Successful_impl;
        std::function<void()> Discord_RunCallbacks_impl;
    };
}
