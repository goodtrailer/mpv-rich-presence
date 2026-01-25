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

#include "mpv_rich_presence/discord_utils.hpp"

using namespace mpvrp;

void discord_client::init(Discord_Client* h) { Discord_Client_Init(h); }

void discord_client::drop(Discord_Client* h) { Discord_Client_Drop(h); }

void discord_activity::init(Discord_Activity* h) { Discord_Activity_Init(h); }

void discord_activity::drop(Discord_Activity* h) { Discord_Activity_Drop(h); }

void discord_activity_timestamps::init(Discord_ActivityTimestamps* h) { Discord_ActivityTimestamps_Init(h); }

void discord_activity_timestamps::drop(Discord_ActivityTimestamps* h) { Discord_ActivityTimestamps_Drop(h); }
