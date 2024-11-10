// dna.c
#include <stdio.h>
#include <string.h>  // Include string.h to resolve strcmp warning
#include "dna.h"
#include <stdbool.h>



struct Suffix {
    int index;
    char suff[MAX_LENGTH];
};

// Custom function to calculate string length
int string_length(char *str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
int custom_strcmp(const char str1[], const char str2[]) {
    int i;

    // Iterate through each character of both strings
    for (i = 0; str1[i] && (str1[i] == str2[i]); i++) {
        // Continue iterating as long as characters are equal and not null
    }
    return (unsigned char)str1[i] - (unsigned char)str2[i];
}


void selectionSort(struct Suffix suffixes[], int n) {
    selectionSort_label2:
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;

        #pragma HLS PIPELINE  // Enable pipelining for the inner loop
        selectionSort_label1:
        for (int j = i + 1; j < n; j++) {
            if (custom_strcmp(suffixes[j].suff, suffixes[min_idx].suff) < 0) {
                min_idx = j;
            }
        }

        // Swap suffixes[i] and suffixes[min_idx]
        struct Suffix temp = suffixes[i];
        suffixes[i] = suffixes[min_idx];
        suffixes[min_idx] = temp;
    }
}



// Function to fill the suffix array
void fillSuffixArray(char *txt, int n, int suffix_array[]) {
    struct Suffix suffixes[MAX_LENGTH];
    fillSuffixArray_label1:for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        for (int j = i; j < n; j++) {
            suffixes[i].suff[j - i] = txt[j];
        }
        suffixes[i].suff[n - i] = '\0';
    }
    selectionSort(suffixes, n);
    for (int k = 0; k < n; k++) {
        suffix_array[k] = suffixes[k].index;
    }
}

// Create BWT from suffix array
void createBWT(FMIndex *fm_index, char *text) {
    for (int i = 0; i < fm_index->text_length; i++) {
        int suffix_index = fm_index->suffix_array[i];
        fm_index->bwt[i] = (suffix_index == 0) ? '$' : text[suffix_index - 1];
    }
    fm_index->bwt[fm_index->text_length] = '\0';
}

// Create F array
void createF(FMIndex *fm_index, char *text) {
    for (int i = 0; i < fm_index->text_length; i++) {
        int suffix_index = fm_index->suffix_array[i];
        fm_index->f[i] = text[suffix_index];
    }
    fm_index->f[fm_index->text_length] = '\0';
}

// Create checkpoints for rank calculation
void createCheckpoints(FMIndex *fm_index) {
    int tally[ALPHABET_SIZE] = {0};

    #pragma HLS ARRAY_PARTITION variable=tally complete
    createCheckpoints_label2:
    for (int i = 1; i <=fm_index->text_length; i++) {
        tally[(unsigned char)fm_index->bwt[i-1]]++;

        if (i % 1 == 0) {  // CP_INTERVAL = 1
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                #pragma HLS PIPELINE
                fm_index->checkpoints[j][i / 1] = tally[j];
            }
        }
    }
}


void createFirstOccurrence(FMIndex *fm_index) {
    int tally[ALPHABET_SIZE] = {0};
    int local_first[ALPHABET_SIZE] = {0};  // Use a local array to avoid modifying fm_index->first directly

    #pragma HLS ARRAY_PARTITION variable=tally complete  // Partition tally for independent access
    #pragma HLS ARRAY_PARTITION variable=local_first complete  // Partition local_first to avoid memory conflicts
    #pragma HLS PIPELINE II=1  // Pipeline the loop for improved parallelism

    for (int i = 0; i < fm_index->text_length; i++) {
        unsigned char c = fm_index->f[i];

        // Only set local_first[c] once when it's zero to mark the first occurrence
        if (local_first[c] == 0) {
            local_first[c] = i + 1;  // Offset by 1 to avoid initial zero conflict
        }

        tally[c]++;
    }

    // Copy the local_first array back to fm_index->first
    for (int j = 0; j < ALPHABET_SIZE; j++) {
        fm_index->first[j] = (local_first[j] > 0) ? (local_first[j] - 1) : 0;
    }
}


void findRange(FMIndex *fm_index, char *pattern, int *l, int *r) {
    *l = 0;
    *r = fm_index->text_length - 1;
    int pattern_length = string_length(pattern);

    findRange_label1:
    for (int i = pattern_length - 1; i >= 0; i--) {
        unsigned char c = pattern[i];
        int x = fm_index->checkpoints[c][*l / 1];
        int y = fm_index->checkpoints[c][*r / 1];
        *l = fm_index->first[c] + (x ? x  : 0);
        *r = fm_index->first[c] + (y ? y  : 0);
        // Print the values
        printf("Value of *l: %d\n", *l);
        printf("Value of *r: %d\n", *r);
        if (*r < *l) break;

        #pragma HLS PIPELINE
    }
}

// Initialize FM Index
int createFmIndex(FMIndex *fm_index, char *text, char *pattern, int positions[], int max_positions) {
    fm_index->text_length = string_length(text);
    fillSuffixArray(text, fm_index->text_length, fm_index->suffix_array);
    createBWT(fm_index, text);
    createF(fm_index, text);
    createCheckpoints(fm_index);
    createFirstOccurrence(fm_index);
    int l, r;
    findRange(fm_index, pattern, &l, &r);
    
    if (r < l) {
        printf("Pattern not found.\n");
        return 0;  // No occurrences found
    }

    int occurrences = r - l ;  // Calculate the number of occurrences
    printf("Pattern found at positions: ");
    int count = 0;

    for (int i = l; i < r && count < max_positions; i++) {
        positions[count++] = fm_index->suffix_array[i];
        printf("%d ", fm_index->suffix_array[i]);
    }
    printf("\n");

    return occurrences;  // Return the count of occurrences
}

