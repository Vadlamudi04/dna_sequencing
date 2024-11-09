// dna.h
#ifndef DNA_H
#define DNA_H

#define ALPHABET_SIZE 256  // ASCII characters
#define MAX_LENGTH 1000     // Max length of input sequence
#define MAX_OCCURRENCES 100

// FMIndex structure to hold BWT, suffix array, and other metadata
// typedef struct {
//     char bwt[MAX_LENGTH];
//     char f[MAX_LENGTH];
//     int suffix_array[MAX_LENGTH];
//     int first[ALPHABET_SIZE];
//     int checkpoints[ALPHABET_SIZE][MAX_LENGTH / 1 + 1];
//     int text_length;
// } FMIndex;

// // Function prototypes
// void fillSuffixArray(char *txt, int n, int suffix_array[]);
// void createBWT(FMIndex *fm_index, char *text);
// void createF(FMIndex *fm_index, char *text);
// void createCheckpoints(FMIndex *fm_index);
// void createFirstOccurrence(FMIndex *fm_index);
// void findRange(FMIndex *fm_index, char *pattern, int *l, int *r);
// //int search(FMIndex *fm_index, char *pattern, int positions[], int max_positions);
// int createFmIndex(FMIndex *fm_index, char *text, char *pattern, int positions[], int max_positions);
int topfunction(char *dna_seq, char *pattern, int positions[]);
#endif

