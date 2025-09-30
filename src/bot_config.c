#include "bot_config.h"
#include "slash_commands.h" // We need to know about our command handlers
#include <stdio.h>

// This function is called when the bot is ready and has connected to Discord
void on_ready(struct discord *client) {
    const struct discord_user *bot = discord_get_self(client);
    printf("Logged in as %s!\n", bot->username);

    // Register the slash command globally
    struct discord_create_global_application_command chat_cmd = {
        .name = "chat",
        .description = "Chat with the Groq-powered AI",
        .options =
            (struct discord_application_command_option[]){
                {
                    .type = DISCORD_APPLICATION_COMMAND_OPTION_STRING,
                    .name = "prompt",
                    .description = "What you want to ask the AI",
                    .required = true,
                },
                {0}}, // Sentinel
    };
    
    discord_create_global_application_command(client, bot->id, &chat_cmd, NULL);
}

// Central place to set up all event handlers for the bot
void bot_setup_callbacks(struct discord *client) {
    // Set the callbacks for events we care about
    discord_set_on_ready(client, &on_ready);
    discord_set_on_interaction_create(client, &on_chat_command);
}
