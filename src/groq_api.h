#ifndef GROQ_API_H
#define GROQ_API_H

// Function to call the Groq API
// Takes a system prompt (personality) and a user prompt
// Returns a dynamically allocated string with the response, which must be freed.
char *ask_groq(const char *system_prompt, const char *user_prompt);

#endif // GROQ_API_H
