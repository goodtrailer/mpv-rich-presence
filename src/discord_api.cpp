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

#include "mpv_rich_presence/discord_api.hpp"

namespace dll = boost::dll;
using namespace mpvrp;

discord_api::discord_api(const dll::shared_library& sdk)
    : Discord_Activity_Drop_impl { dll::import_symbol<void(Discord_Activity* self)>(sdk, "Discord_Activity_Drop") }
    , Discord_Activity_Init_impl { dll::import_symbol<void(Discord_Activity* self)>(sdk, "Discord_Activity_Init") }
    , Discord_Activity_SetName_impl { dll::import_symbol<void(Discord_Activity* self, Discord_String value)>(sdk, "Discord_Activity_SetName") }
    , Discord_Activity_SetState_impl { dll::import_symbol<void(Discord_Activity* self, Discord_String* value)>(sdk, "Discord_Activity_SetState") }
    , Discord_Activity_SetTimestamps_impl { dll::import_symbol<void(Discord_Activity* self, Discord_ActivityTimestamps* value)>(sdk, "Discord_Activity_SetTimestamps") }
    , Discord_Activity_SetType_impl { dll::import_symbol<void(Discord_Activity* self, Discord_ActivityTypes value)>(sdk, "Discord_Activity_SetType") }
    , Discord_ActivityTimestamps_Drop_impl { dll::import_symbol<void(Discord_ActivityTimestamps* self)>(sdk, "Discord_ActivityTimestamps_Drop") }
    , Discord_ActivityTimestamps_Init_impl { dll::import_symbol<void(Discord_ActivityTimestamps* self)>(sdk, "Discord_ActivityTimestamps_Init") }
    , Discord_ActivityTimestamps_SetEnd_impl { dll::import_symbol<void(Discord_ActivityTimestamps* self, uint64_t value)>(sdk, "Discord_ActivityTimestamps_SetEnd") }
    , Discord_ActivityTimestamps_SetStart_impl { dll::import_symbol<void(Discord_ActivityTimestamps* self, uint64_t value)>(sdk, "Discord_ActivityTimestamps_SetStart") }
    , Discord_Client_AddLogCallback_impl { dll::import_symbol<void(Discord_Client* self, Discord_Client_LogCallback callback, Discord_FreeFn callback__userDataFree, void* callback__userData, Discord_LoggingSeverity minSeverity)>(sdk, "Discord_Client_AddLogCallback") }
    , Discord_Client_ClearRichPresence_impl { dll::import_symbol<void(Discord_Client* self)>(sdk, "Discord_Client_ClearRichPresence") }
    , Discord_Client_GetApplicationId_impl { dll::import_symbol<uint64_t(Discord_Client* self)>(sdk, "Discord_Client_GetApplicationId") }
    , Discord_Client_Drop_impl { dll::import_symbol<void(Discord_Client* self)>(sdk, "Discord_Client_Drop") }
    , Discord_Client_Init_impl { dll::import_symbol<void(Discord_Client* self)>(sdk, "Discord_Client_Init") }
    , Discord_Client_SetApplicationId_impl { dll::import_symbol<void(Discord_Client* self, uint64_t applicationId)>(sdk, "Discord_Client_SetApplicationId") }
    , Discord_Client_UpdateRichPresence_impl { dll::import_symbol<void(Discord_Client* self, Discord_Activity* activity, Discord_Client_UpdateRichPresenceCallback cb, Discord_FreeFn cb__userDataFree, void* cb__userData)>(sdk, "Discord_Client_UpdateRichPresence") }
    , Discord_ClientResult_ErrorCode_impl { dll::import_symbol<int32_t(Discord_ClientResult* self)>(sdk, "Discord_ClientResult_ErrorCode") }
    , Discord_ClientResult_Successful_impl { dll::import_symbol<bool(Discord_ClientResult* self)>(sdk, "Discord_ClientResult_Successful") }
    , Discord_RunCallbacks_impl { dll::import_symbol<void()>(sdk, "Discord_RunCallbacks") }
{
}

void discord_api::Discord_Activity_Drop(Discord_Activity* self) const
{
    Discord_Activity_Drop_impl(self);
}

void discord_api::Discord_Activity_Init(Discord_Activity* self) const
{
    Discord_Activity_Init_impl(self);
}

void discord_api::Discord_Activity_SetName(Discord_Activity* self, Discord_String value) const
{
    Discord_Activity_SetName_impl(self, value);
}

void discord_api::Discord_Activity_SetState(Discord_Activity* self, Discord_String* value) const
{
    Discord_Activity_SetState_impl(self, value);
}

void discord_api::Discord_Activity_SetTimestamps(Discord_Activity* self, Discord_ActivityTimestamps* value) const
{
    Discord_Activity_SetTimestamps_impl(self, value);
}

void discord_api::Discord_Activity_SetType(Discord_Activity* self, Discord_ActivityTypes value) const
{
    Discord_Activity_SetType_impl(self, value);
}

void discord_api::Discord_ActivityTimestamps_Drop(Discord_ActivityTimestamps* self) const
{
    Discord_ActivityTimestamps_Drop_impl(self);
}

void discord_api::Discord_ActivityTimestamps_Init(Discord_ActivityTimestamps* self) const
{
    Discord_ActivityTimestamps_Init_impl(self);
}

void discord_api::Discord_ActivityTimestamps_SetEnd(Discord_ActivityTimestamps* self, uint64_t value) const
{
    Discord_ActivityTimestamps_SetEnd_impl(self, value);
}

void discord_api::Discord_ActivityTimestamps_SetStart(Discord_ActivityTimestamps* self, uint64_t value) const
{
    Discord_ActivityTimestamps_SetStart_impl(self, value);
}

void discord_api::Discord_Client_AddLogCallback(Discord_Client* self, Discord_Client_LogCallback callback, Discord_FreeFn callback__userDataFree, void* callback__userData, Discord_LoggingSeverity minSeverity) const
{
    Discord_Client_AddLogCallback_impl(self, callback, callback__userDataFree, callback__userData, minSeverity);
}

void discord_api::Discord_Client_ClearRichPresence(Discord_Client* self) const
{
    Discord_Client_ClearRichPresence_impl(self);
}

auto discord_api::Discord_Client_GetApplicationId(Discord_Client* self) const -> uint64_t
{
    return Discord_Client_GetApplicationId_impl(self);
}

void discord_api::Discord_Client_Drop(Discord_Client* self) const
{
    Discord_Client_Drop_impl(self);
}

void discord_api::Discord_Client_Init(Discord_Client* self) const
{
    Discord_Client_Init_impl(self);
}

void discord_api::Discord_Client_SetApplicationId(Discord_Client* self, uint64_t applicationId) const
{
    Discord_Client_SetApplicationId_impl(self, applicationId);
}

void discord_api::Discord_Client_UpdateRichPresence(Discord_Client* self, Discord_Activity* activity, Discord_Client_UpdateRichPresenceCallback cb, Discord_FreeFn cb__userDataFree, void* cb__userData) const
{
    Discord_Client_UpdateRichPresence_impl(self, activity, cb, cb__userDataFree, cb__userData);
}

auto discord_api::Discord_ClientResult_ErrorCode(Discord_ClientResult* self) const -> int32_t
{
    return Discord_ClientResult_ErrorCode_impl(self);
}

auto discord_api::Discord_ClientResult_Successful(Discord_ClientResult* self) const -> bool
{
    return Discord_ClientResult_Successful_impl(self);
}

void discord_api::Discord_RunCallbacks() const
{
    Discord_RunCallbacks_impl();
}
