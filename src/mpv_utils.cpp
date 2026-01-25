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

#include "mpv_rich_presence/mpv_utils.hpp"

#include <array>
#include <format>
#include <string>

using namespace mpvrp;

auto mpvrp::mpv_print(mpv_handle* ctx, std::string_view str) -> int
{
    auto full_str = std::format("[rich-presence] {}", str);
    auto args = std::array<const char*, 3> { "print-text", full_str.c_str(), nullptr };
    return mpv_command(ctx, args.data());
}

auto mpvrp::mpv_show(mpv_handle* ctx, const std::string& str) -> int
{
    auto args = std::array<const char*, 3> { "show-text", str.c_str(), nullptr };
    return mpv_command(ctx, args.data());
}
