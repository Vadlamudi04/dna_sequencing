#include <stdio.h>
#include <string.h>  // Include string.h to resolve strcmp warning
#include "dna.h"
#include <stdbool.h>

// Declare Suffix struct outside functions
struct Suffix {
    int index;
    char suff[MAX_LENGTH];
};


int topfunction(char *dna_seq, char *pattern, int positions[])
{
    int length = 0;
    while (dna_seq[length] != '\0') {
        length++;
    }

    //suffix array
    struct Suffix suffixes[MAX_LENGTH];
    fillSuffixArray_label1:for (int i = 0; i < length; i++) {
        suffixes[i].index = i;
        for (int j = i; j < length; j++) {
            suffixes[i].suff[j - i] = dna_seq[j];
        }
        suffixes[i].suff[length - i] = '\0';
    }

    //sorting the suffix array

        generateSuffixArray_label2:
    for (int j = 0; j < length - 1; j++) {
        int min_idx = j;

//        #pragma HLS PIPELINE  // Enable pipelining for the inner loop
        generateSuffixArray_label3:
        for (int k = j + 1; k < length; k++) {
            // Custom strcmp logic
            int l;
            for (l = 0; suffixes[k].suff[l] && 
                        (suffixes[k].suff[l] == suffixes[min_idx].suff[l]); l++) {
                // Continue as long as characters are equal and not null
            }
            if ((unsigned char)suffixes[k].suff[l] < (unsigned char)suffixes[min_idx].suff[l]) {
                min_idx = k;
            }
        }

        struct Suffix temp = suffixes[j];
        suffixes[j] = suffixes[min_idx];
        suffixes[min_idx] = temp;
    }

    // storing the suffix array
    int suffix_array[MAX_LENGTH];
    generateSuffixArray_label4:
    for (int z = 0; z < length; z++) {
        suffix_array[z] = suffixes[z].index;
    }

    // Create BWT from suffix array
    char bwt[MAX_LENGTH];
    for(int w = 0; w < length; w++)
    {
        int suffix_index = suffix_array[w];
        if(suffix_index == 0)
        {
            bwt[w] = '$';
        }
        else
        {
            bwt[w] = dna_seq[suffix_index-1];
        }
    }
    bwt[length] = '\0';

    //Create F array
    char f[MAX_LENGTH];
    for (int q = 0; q < length; q++)
    {
        int suf_in = suffix_array[q];
        f[q] = dna_seq[suf_in];
    }
    f[length] = '\0';

    //Create checkpoints for rank calculation
    int tally[ALPHABET_SIZE] = {0};
    int checkpoints[ALPHABET_SIZE][MAX_LENGTH/ 1 + 1];
//    #pragma HLS ARRAY_PARTITION variable=tally complete
    for (int a = 0; a < length; a++) {
        tally[(unsigned char)bwt[a]]++;

        if (a % 1 == 0) {  // CP_INTERVAL = 1
            for (int b = 0; b < ALPHABET_SIZE; b++) {
//                #pragma HLS PIPELINE
                checkpoints[b][a / 1] = tally[b];
            }
        }
    }

    //First Occurrence
    int first[ALPHABET_SIZE];

    int local_first[ALPHABET_SIZE] = {0};  // Use a local array to avoid modifying fm_index->first directly

//    #pragma HLS ARRAY_PARTITION variable=tally complete  // Partition tally for independent access
//    #pragma HLS ARRAY_PARTITION variable=local_first complete  // Partition local_first to avoid memory conflicts
//    #pragma HLS PIPELINE II=1  // Pipeline the loop for improved parallelism

    for (int u = 0; u < length; u++) {
        unsigned char c = f[u];

        // Only set local_first[c] once when it's zero to mark the first occurrence
        if (local_first[c] == 0) {
            local_first[c] = u + 1;  // Offset by 1 to avoid initial zero conflict
        }
    }

    for (int v = 0; v < ALPHABET_SIZE; v++) {
        first[v] = (local_first[v] > 0) ? (local_first[v] - 1) : 0;
    }


    //finding the range
    int l = 0;
    int r = length - 1;

    int pattern_length = 0;
    while (pattern[pattern_length] != '\0') {
        pattern_length++;
    }
    findRange_label1:
    for (int d = pattern_length - 1; d >= 0; d--) {
        unsigned char c = pattern[d];
        int x = checkpoints[c][l / 1];
        int y = checkpoints[c][r / 1];
        l = first[c] + (x ? x - 1 : 0);
        r = first[c] + (y ? y - 1 : 0);
//		#pragma HLS PIPELINE II=1
        if (r < l) 
        {
            break;
        }

//        #pragma HLS PIPELINE
    }

    if(r<l)
    {
        printf("Pattern not found.\n");
        return 0;
    }
    int occurrences = r - l + 1;  // Calculate the number of occurrences
    printf("Pattern found at positions: ");
    int count = 0;

    for (int p = l; p <= r && count < MAX_OCCURRENCES; p++) {
        positions[count++] = suffix_array[p];
        printf("%d ", suffix_array[p]);
    }
    printf("\n");

    return occurrences;


}
