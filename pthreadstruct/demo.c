#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 10
#define MAX_WORD_SIZE 100
#define DICTIONARY_SIZE 1000
#define NUM_THREADS 3

typedef struct {
    char buffer[BUFFER_SIZE][MAX_WORD_SIZE];
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_produce;
    pthread_cond_t cond_consume;
} WordBuffer;

typedef struct {
    char word[MAX_WORD_SIZE];
    char result[MAX_WORD_SIZE];
} ResultBuffer;


WordBuffer word_buffer;
ResultBuffer results[BUFFER_SIZE];
int results_count = 0;
char dictionary[DICTIONARY_SIZE][MAX_WORD_SIZE];
int dictionary_size;

// Function prototypes
void *spell_check(void *arg);
void load_dictionary();

// Main server function
int main() {
    // Load the dictionary
    load_dictionary();

    // Initialize WordBuffer
    word_buffer.count = 0;
    pthread_mutex_init(&word_buffer.mutex, NULL);
    pthread_cond_init(&word_buffer.cond_produce, NULL);
    pthread_cond_init(&word_buffer.cond_consume, NULL);

    // Create threads for spell checking
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, spell_check, NULL);
    }

    char input[MAX_WORD_SIZE];
    while (1) {
        printf("Enter a word to check (type 'exit' to quit): ");
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) {
            break;
        }

        pthread_mutex_lock(&word_buffer.mutex);
        // Wait if buffer is full
        while (word_buffer.count == BUFFER_SIZE) {
            pthread_cond_wait(&word_buffer.cond_produce, &word_buffer.mutex);
        }
        // Add word to buffer
        strcpy(word_buffer.buffer[word_buffer.count], input);
        word_buffer.count++;
        pthread_cond_signal(&word_buffer.cond_consume);
        pthread_mutex_unlock(&word_buffer.mutex);
    }

    // Signal threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cancel(threads[i]);
    }

    // Print results
    printf("Spell Check Results:\n");
    for (int i = 0; i < results_count; i++) {
        printf("%s: %s\n", results[i].word, results[i].result);
    }

    // Cleanup
    pthread_mutex_destroy(&word_buffer.mutex);
    pthread_cond_destroy(&word_buffer.cond_produce);
    pthread_cond_destroy(&word_buffer.cond_consume);

    return 0;
}

// Spell checker thread function
void *spell_check(void *arg) {
    while (1) {
        char word[MAX_WORD_SIZE];
        pthread_mutex_lock(&word_buffer.mutex);
        // Wait if buffer is empty
        while (word_buffer.count == 0) {
            pthread_cond_wait(&word_buffer.cond_consume, &word_buffer.mutex);
        }
        // Retrieve word from buffer
        strcpy(word, word_buffer.buffer[word_buffer.count - 1]);
        word_buffer.count--;
        pthread_cond_signal(&word_buffer.cond_produce);
        pthread_mutex_unlock(&word_buffer.mutex);

        // Check if the word is in the dictionary
        int found = 0;
        for (int i = 0; i < dictionary_size; i++) {
            if (strcasecmp(word, dictionary[i]) == 0) {
                found = 1;
                break;
            }
        }

        // Store result
        pthread_mutex_lock(&word_buffer.mutex);
        strcpy(results[results_count].word, word);
        strcpy(results[results_count].result, found ? "Correct" : "Incorrect");
        results_count++;
        pthread_mutex_unlock(&word_buffer.mutex);
    }

    return NULL;
}

// Function to load example dictionary
void load_dictionary() {
    // Example words, replace with an actual file read for a larger dictionary
    strcpy(dictionary[0], "apple");
    strcpy(dictionary[1], "banana");
    strcpy(dictionary[2], "orange");
    strcpy(dictionary[3], "grape");
    strcpy(dictionary[4], "peach");
    dictionary_size = 5; // Update this to the actual size
}