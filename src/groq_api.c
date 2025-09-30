#include "groq_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

// A struct to hold the response data from our HTTP request
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function for libcurl to write received data into our MemoryStruct
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// Main function to interact with the Groq API
char *ask_groq(const char *system_prompt, const char *user_prompt) {
    CURL *curl;
    CURLcode res;
    char *groq_api_key = getenv("GROQ_API_KEY");

    if (!groq_api_key) {
        fprintf(stderr, "Error: GROQ_API_KEY environment variable not set.\n");
        return NULL;
    }

    // Sanitize user prompt to escape quotes for JSON
    char *escaped_user_prompt = json_string_n(user_prompt, strlen(user_prompt))->value;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        char post_fields[8192];
        snprintf(post_fields, sizeof(post_fields),
                 "{\"messages\": [{\"role\": \"system\", \"content\": \"%s\"}, {\"role\": \"user\", \"content\": \"%s\"}], \"model\": \"llama3-8b-8192\"}",
                 system_prompt, escaped_user_prompt);
        
        free(escaped_user_prompt); // free the sanitized string

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", groq_api_key);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.groq.com/openai/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(chunk.memory);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return NULL;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

    json_error_t error;
    json_t *root = json_loads(chunk.memory, 0, &error);
    free(chunk.memory);

    if (!root) {
        fprintf(stderr, "error parsing groq response: on line %d: %s\n", error.line, error.text);
        return NULL;
    }

    json_t *choices = json_object_get(root, "choices");
    if (!json_is_array(choices) || json_array_size(choices) == 0) {
        fprintf(stderr, "Error: 'choices' is not a valid array in Groq response.\n");
        json_decref(root);
        return NULL;
    }
    json_t *first_choice = json_array_get(choices, 0);
    json_t *message = json_object_get(first_choice, "message");
    json_t *content = json_object_get(message, "content");
    
    char *response_text = strdup(json_string_value(content));
    
    json_decref(root);
    return response_text;
}
