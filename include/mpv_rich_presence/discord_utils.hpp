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

#include <optional>

namespace mpvrp
{
    template <typename Derived, typename Inner>
    concept discord_managed = requires {
        requires std::default_initializable<Inner>;
        requires std::same_as<decltype(Derived::init(std::declval<Inner*>())), void>;
        requires std::same_as<decltype(Derived::drop(std::declval<Inner*>())), void>;
    };

    template <typename Derived, typename Inner>
    class discord_object
    {
    public:
        discord_object()
        {
            static_assert(discord_managed<Derived, Inner>);
            handle = std::make_optional<Inner>();
            Derived::init(&*handle);
        }

        discord_object(Derived&& other) noexcept
        {
            static_assert(discord_managed<Derived, Inner>);
            handle = other.handle;
            other.handle = std::nullopt;
        }

        auto operator=(Derived&& other) noexcept -> Derived&
        {
            handle = other.handle;
            other.handle = std::nullopt;
            return *(Derived*)this;
        }

        ~discord_object()
        {
            static_assert(discord_managed<Derived, Inner>);
            if (!is_valid())
                return;

            Derived::drop(&*handle);
            handle = std::nullopt;
        }

        discord_object(const Derived&) = delete;
        auto operator=(const Derived&) -> Derived& = delete;

        auto is_valid() const -> bool { return handle.has_value(); }

        auto get() -> Inner& { return *handle; }

    private:
        std::optional<Inner> handle = {};
    };

    class discord_client : public discord_object<discord_client, Discord_Client>
    {
    public:
        static void init(Discord_Client*);
        static void drop(Discord_Client*);
    };

    class discord_activity : public discord_object<discord_activity, Discord_Activity>
    {
    public:
        static void init(Discord_Activity*);
        static void drop(Discord_Activity*);
    };

    class discord_activity_timestamps : public discord_object<discord_activity_timestamps, Discord_ActivityTimestamps>
    {
    public:
        static void init(Discord_ActivityTimestamps*);
        static void drop(Discord_ActivityTimestamps*);
    };
}
