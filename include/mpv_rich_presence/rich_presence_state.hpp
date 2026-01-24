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

#include <discordpp.h>
#include <mpv/client.h>

#include <memory>
#include <optional>
#include <string>

namespace mpvrp
{
    struct rich_presence_state
    {
        mpv_handle* mpv_client;
        std::unique_ptr<discordpp::Client> discord_client;

        std::optional<bool> is_enabled = std::nullopt;

        bool media_has_audio = false;
        bool media_has_video = false;
        std::string media_artist = "";
        std::string media_title = "";
    };
}
