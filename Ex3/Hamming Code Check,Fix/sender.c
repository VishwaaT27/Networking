#include <stdio.h>
int readInput(char message[], int binaryMatrix[][7])
{
    FILE *fp;
    char ch;
    int count = 0, i, val;
    fp = fopen("input.txt", "r");
    if (fp == NULL) return 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n' || ch == '\r') continue;
        message[count] = ch;
        val = (int)ch;
        for (i = 6; i >= 0; i--)
        {
            binaryMatrix[count][i] = val % 2;
            val = val / 2;
        }
        count++;
        if (count >= 100) break;
    }
    fclose(fp);
    return count;
}
void displayBinary(char message[], int binaryMatrix[][7], int count)
{
    int i, j;
    printf("Loaded Message: \"");
    for (i = 0; i < count; i++)
        printf("%c", message[i]);
    printf("\"\n\n");
    printf("Total Number of Characters: %d\n\n", count);
    printf("Binary Value of Message:\n");
    for (i = 0; i < count; i++)
    {
        printf("'%c' : ", message[i]);
        for (j = 0; j < 7; j++)
            printf("%d", binaryMatrix[i][j]);
        printf("\n");
    }
}
void genHamm(int data[7], int codeword[12])
{
    codeword[3]  = data[0];
    codeword[5]  = data[1];
    codeword[6]  = data[2];
    codeword[7]  = data[3];
    codeword[9]  = data[4];
    codeword[10] = data[5];
    codeword[11] = data[6];
    codeword[1] = codeword[3] ^ codeword[5] ^ codeword[7] ^ codeword[9] ^ codeword[11];/* P1 1, 3, 5, 7, 9, 11 */
    codeword[2] = codeword[3] ^ codeword[6] ^ codeword[7] ^ codeword[10] ^ codeword[11];/* P2 2, 3, 6, 7, 10, 11 */
    codeword[4] = codeword[5] ^ codeword[6] ^ codeword[7];/* P4 4, 5, 6, 7 */
    codeword[8] = codeword[9] ^ codeword[10] ^ codeword[11];/* P8 8, 9, 10, 11 */
}
int calcParity(int codeword[12])
{
    int count = 0, i;
    for (i = 1; i <= 11; i++) if (codeword[i] == 1) count++;
    if (count % 2 == 0) return 0;
    else return 1;
}

void writeFile(int binaryMatrix[][7], int count)
{
    FILE *fp;
    int codeword[12];
    int i, j;
    fp = fopen("transmitted.txt", "w");
    if (fp == NULL)
    {
        printf("Error opening transmitted.txt for writing.\n");
        return;
    }
    printf("\nGenerated 12-bit Extended Hamming Codewords:\n[P0 P1 P2 D1 P4 D2 D3 D4 P8 D5 D6 D7]\n");
    fprintf(fp, "%d\n", count);
    for (i = 0; i < count; i++)
    {
        genHamm(binaryMatrix[i], codeword);
        codeword[0] = calcParity(codeword);
        printf("Char #%d: ", i + 1);
        for (j = 0; j < 12; j++) printf("%d ", codeword[j]);
        printf("\n");
        for (j = 0; j < 12; j++)
        {
            fprintf(fp, "%d", codeword[j]);
            if (j < 11) fprintf(fp, " ");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
int main()
{
    char message[100];
    int binaryMatrix[100][7];
    int count;
    printf("=========================================\n");
    printf("        SENDER TRANSMISSION SIMULATION    \n");
    printf("=========================================\n\n");
    count = readInput(message, binaryMatrix);
    if (count == 0)
    {
        printf("Error: Could not read input.txt or file is empty.\n");
        return 1;
    }
    displayBinary(message, binaryMatrix, count);
    writeFile(binaryMatrix, count);
    printf("\nTransmission data successfully written to 'transmitted.txt'.\n");
    return 0;
}
