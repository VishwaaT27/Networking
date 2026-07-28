#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int getParity(int count1s)
{
    if (count1s % 2 != 0) return 1;
    return 0;
}
void readInput(char message[], int matrix[][8], int *rowCount)
{
    int i;
    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL)
    {
        printf("Could not open input.txt!\n");
        return;
    }
    char ch;
    int index = 0;
    while ((ch = fgetc(fp)) != EOF && index < 100)
    {
        if (ch == '\n' || ch == '\r') continue;
        message[index] = ch;
        for (i = 7; i >= 0; i--) matrix[index][7 - i] = (ch >> i) & 1;
        index++;
    }
    *rowCount = index;
    fclose(fp);
}
void printBinaryMatrix(char message[], int matrix[][8], int rowCount)
{
    int i, j;
    printf("Binary Matrix Representation:\n-----------------------------\n");
    for (i = 0; i < rowCount; i++)
    {
        printf("Row %2d ('%c'): ", i + 1, message[i]);
        for (j = 0; j < 8; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}
void generateParity(int matrix[][8], int rowCount, int rowParity[], int colParity[], int *interParity)
{
    int i, j;
    for (i = 0; i < rowCount; i++)
    {
        int ones = 0;
        for (j = 0; j < 8; j++) if (matrix[i][j] == 1) ones++;
        rowParity[i] = getParity(ones);
    }
    for (j = 0; j < 8; j++)
    {
        int ones = 0;
        for (i = 0; i < rowCount; i++) if (matrix[i][j] == 1) ones++;
        colParity[j] = getParity(ones);
    }
    int rowParityOnes = 0;
    for (i = 0; i < rowCount; i++) if (rowParity[i] == 1) rowParityOnes++;
    *interParity = getParity(rowParityOnes);
}

void sendData(int matrix[][8], int rowCount, int rowParity[], int colParity[], int interParity)
{
    int i, j;
    FILE *fp = fopen("transmitted.txt", "w");
    if (fp == NULL)
    {
        printf("Error opening transmitted.txt for writing.\n");
        return;
    }
    fprintf(fp, "%d\n", rowCount);
    for (i = 0; i < rowCount; i++)
    {
        for (j = 0; j < 8; j++) fprintf(fp, "%d ", matrix[i][j]);
        fprintf(fp, "\n");
    }
    for (i = 0; i < rowCount; i++) fprintf(fp, "%d ", rowParity[i]);
    fprintf(fp, "\n");
    for (j = 0; j < 8; j++) fprintf(fp, "%d ", colParity[j]);
    fprintf(fp, "\n");
    fprintf(fp, "%d\n", interParity);
    fclose(fp);
    printf("Data successfully processed and written to 'transmitted.txt'.\n");
}
int main()
{
    char message[100];
    int matrix[100][8];
    int rowParity[100];
    int colParity[8];
    int interParity = 0, rowCount = 0;
    readInput(message, matrix, &rowCount);
    if (rowCount == 0)
    {
        printf("Error: 'input.txt' is empty or missing!\n");
        return 1;
    }
    printBinaryMatrix(message, matrix, rowCount);
    generateParity(matrix, rowCount, rowParity, colParity, &interParity);
    sendData(matrix, rowCount, rowParity, colParity, interParity);
    return 0;
}
