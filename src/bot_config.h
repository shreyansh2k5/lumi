#ifndef BOT_CONFIG_H
#define BOT_CONFIG_H

#include <concord/discord.h>

// Sets up all the necessary callbacks for the bot
void bot_setup_callbacks(struct discord *client);

// The callback for when the bot is ready
void on_ready(struct discord *client);

#endif // BOT_CONFIG_H
