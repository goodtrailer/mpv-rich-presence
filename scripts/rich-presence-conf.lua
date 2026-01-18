local mp_msg = require("mp.msg")
local mp_options = require("mp.options")

local options = {
    application_id = "0",
    on = false,
}
mp_options.read_options(options, "rich-presence")
mp.commandv("script-message-to", "rich_presence", "application_id", options.application_id)

if options.on then
    mp.commandv("script-message-to", "rich_presence", "on")
else
    mp.commandv("script-message-to", "rich_presence", "off")
end
