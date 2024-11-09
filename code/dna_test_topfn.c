// dna_test.c
#include <stdio.h>
#include "dna.h"  // Include the header file instead of the .c file


int main() {
    char text[MAX_LENGTH] = "ACGTACGTAGCTAGCTAGCTA$";  // Example DNA sequence
    char pattern[MAX_LENGTH] = "TAG";  // Example pattern to search
    int positions[MAX_OCCURRENCES];  // Array to store positions of occurrences
    int ref[] = {15,11,7};
    int pass = 1;

    printf("Using predefined sequence: %s\n", text);
    printf("Using predefined pattern: %s\n", pattern);
    
    // Initialize the FM index
    int occurrences = topfunction(text, pattern, positions);

    // Call search and get number of occurrences and their positions
//    int occurrences = search(&fm_index, pattern, positions, MAX_OCCURRENCES);
    printf("Number of occurrences: %d\n", occurrences);

    // Display each occurrence position

        printf("Occurrences found at positions: ");
        for (int i = 0; i < occurrences && i < MAX_OCCURRENCES; i++) {
            printf("%d ", positions[i]);
            if(positions[i] != ref[i])
            {
            	pass = 0;
            }
            else
            {
            	pass = 1;
            }

        printf("\n");
    }
    
	if (pass)
	{
		printf("----------Pass!------------\n");
		return 0;
	}
	else
	{
		printf("----------Fail!------------\n");
		return 1;
	}
}

