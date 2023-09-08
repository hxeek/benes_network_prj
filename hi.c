#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_unique_pairs(int n) {
    int *input = (int *)malloc(n * sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    if (!input || !output) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    
    // Initialize the arrays
    for (int i = 0; i < n; i++) {
        input[i] = i;
        output[i] = i;
    }
    
    // Shuffle the arrays to get random unique pairs
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = input[i];
        input[i] = input[j];
        input[j] = temp;

        j = rand() % (i + 1);
        temp = output[i];
        output[i] = output[j];
        output[j] = temp;
    }
    
    // Print the pairs
    for (int i = 0; i < n; i++) {
        printf("%d %d\n", input[i], output[i]);
    }

    free(input);
    free(output);
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    int size;
    printf("Enter the size of pairs you want: ");
    scanf("%d", &size);

    generate_unique_pairs(size);

    return 0;
}
