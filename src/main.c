#include <stdio.h>
#include <stdlib.h>

#include <concord/discord.h>
#include "bot_config.h" // Our new header for bot configuration

int main(void) {
    // Get the bot token from the environment variable
    const char *bot_token = getenv("DISCORD_TOKEN");
    if (!bot_token) {
        fprintf(stderr, "Failed to start: DISCORD_TOKEN environment variable is not set.\n");
        return 1;
    }

    // Initialize the Concord client
    struct discord *client = discord_init(bot_token);
    if (!client) {
        fprintf(stderr, "Failed to initialize Discord client.\n");
        return 1;
    }

    // Set up our event callbacks (on_ready, on_interaction_create, etc.)
    bot_setup_callbacks(client);

    // Start the bot and enter the main event loop
    printf("Bot is starting...\n");
    discord_run(client);
    
    // Cleanup when done
    discord_cleanup(client);
    printf("Bot has been cleaned up.\n");
    return 0;
}
