#include <stdio.h>
int readFile(int codewords[][12])
{
    FILE *fp;
    int count = 0, i, j;
    fp = fopen("transmitted.txt", "r");
    if (fp == NULL) return 0;
    if (fscanf(fp, "%d", &count) != 1)
    {
        fclose(fp);
        return 0;
    }
    for (i = 0; i < count; i++) for (j = 0; j < 12; j++) fscanf(fp, "%d", &codewords[i][j]);
    fclose(fp);
    return count;
}
void writeFile(int codewords[][12], int count)
{
    FILE *fp;
    int i, j;
    fp = fopen("transmitted.txt", "w");
    if (fp == NULL)
    {
        printf("Error updating transmitted.txt file.\n");
        return;
    }
    fprintf(fp, "%d\n", count);
    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 12; j++)
        {
            fprintf(fp, "%d", codewords[i][j]);
            if (j < 11) fprintf(fp, " ");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
void displayTransmission(int codewords[][12], int count)
{
    int i, j;
    printf("\nReceived Extended Hamming Transmission:\n");
    printf("Layout Reference: [P0 P1 P2 D1 P4 D2 D3 D4 P8 D5 D6 D7]\n");
    printf("Indices:          [ 0  1  2  3  4  5  6  7  8  9 10 11]\n\n");

    for (i = 0; i < count; i++)
    {
        printf("Char #%d:\n", i + 1);
        for (j = 0; j < 12; j++)
            printf("%d ", codewords[i][j]);
        printf("\n");
    }
}
void flipBit(int codewords[][12], int charIdx, int bitIdx)
{
    int oldVal = codewords[charIdx][bitIdx];
    if (codewords[charIdx][bitIdx] == 0) codewords[charIdx][bitIdx] = 1;
    else codewords[charIdx][bitIdx] = 0;
    printf("\n[Channel Noise]: Flipping bit at Character %d, Codeword Bit %d (%d -> %d)\n",
           charIdx + 1, bitIdx, oldVal, codewords[charIdx][bitIdx]);
}
int calcSyndrome(int codeword[12])
{
    int s1, s2, s4, s8;
    s1 = codeword[1] ^ codeword[3] ^ codeword[5] ^ codeword[7] ^ codeword[9] ^ codeword[11];
    s2 = codeword[2] ^ codeword[3] ^ codeword[6] ^ codeword[7] ^ codeword[10] ^ codeword[11];
    s4 = codeword[4] ^ codeword[5] ^ codeword[6] ^ codeword[7];
    s8 = codeword[8] ^ codeword[9] ^ codeword[10] ^ codeword[11];
    return (s8 * 8) + (s4 * 4) + (s2 * 2) + (s1 * 1);
}
int checkOverallParity(int codeword[12]) {
    int count = 0, i;
    for (i = 0; i < 12; i++) if (codeword[i] == 1) count++;
    return count % 2;
}
void checkAndFix(int codeword[12], int charNum)
{
    int errorPosition = calcSyndrome(codeword);
    int overallParityError = checkOverallParity(codeword);
    printf("Char #%d Status: ", charNum);
    if (errorPosition == 0 && overallParityError == 0) printf("ERROR-FREE.\n");
    else if (errorPosition != 0 && overallParityError == 1)
    {
        printf("SINGLE-BIT ERROR DETECTED at Position %d.\n", errorPosition);
        if (codeword[errorPosition] == 0) codeword[errorPosition] = 1;
        else codeword[errorPosition] = 0;
        printf("         Action: Auto-corrected bit at position %d.\n", errorPosition);
    }
    else if (errorPosition != 0 && overallParityError == 0)
    {
        printf("DOUBLE-BIT ERROR DETECTED.\n");
        printf("         Action: Error cannot be corrected safely!\n");
    }
    else if (errorPosition == 0 && overallParityError == 1)
    {
        printf("ERROR DETECTED IN OVERALL PARITY BIT (P0).\n");
        if (codeword[0] == 0) codeword[0] = 1;
        else codeword[0] = 0;
        printf("         Action: Auto-corrected P0 bit at position 0.\n");
    }
}
char bitsToChar(int codeword[12])
{
    int dataBits[7];
    int asciiVal = 0, i;
    dataBits[0] = codeword[3];
    dataBits[1] = codeword[5];
    dataBits[2] = codeword[6];
    dataBits[3] = codeword[7];
    dataBits[4] = codeword[9];
    dataBits[5] = codeword[10];
    dataBits[6] = codeword[11];
    for (i = 0; i < 7; i++) asciiVal = (asciiVal * 2) + dataBits[i];
    return (char)asciiVal;
}
void recvProcess(int codewords[][12], int count)
{
    char decodedMessage[100];
    int i;
    for (i = 0; i < count; i++)
    {
        checkAndFix(codewords[i], i + 1);
        decodedMessage[i] = bitsToChar(codewords[i]);
    }
    decodedMessage[count] = '\0';
    printf("\nDecoded Message: %s\n", decodedMessage);
}

int main()
{
    int codewords[100][12];
    int count, choice, charNum, bitIdx;
    count = readFile(codewords);
    if (count == 0)
    {
        printf("Error: Could not read transmitted.txt or file is empty.\n");
        return 1;
    }
    displayTransmission(codewords, count);
    printf("\n--- Initial Verification ---\n");
    recvProcess(codewords, count);
    while (1)
    {
        printf("\n=========================================\n");
        printf("RECEIVER TRANSMISSION SIMULATION\n");
        printf("=========================================\n");
        printf("MENU\n1. Modify a bit in transmission (Inject Channel Error)\n2. Exit\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            printf("\nEnter character number (1 to %d): ", count);
            scanf("%d", &charNum);
            printf("Enter codeword bit index (0 to 11): ");
            scanf("%d", &bitIdx);
            if (charNum < 1 || charNum > count || bitIdx < 0 || bitIdx > 11)
            {
                printf("\nInvalid indices provided! Please try again.\n");
                continue;
            }
            flipBit(codewords, charNum - 1, bitIdx);
            writeFile(codewords, count);
            printf("\n--- Updated Transmission State ---\n");
            displayTransmission(codewords, count);
            printf("\n--- Syndrome & SECDED Verification ---\n");
            recvProcess(codewords, count);
        }
        else if (choice == 2)
        {
            printf("\nSimulation terminated.\n");
            break;
        }
        else printf("\nInvalid menu option!\n");
    }
    return 0;
}
