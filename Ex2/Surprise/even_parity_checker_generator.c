#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int even(int *s)
{
   int c = 0, i;
   for(i = 0; i < 7; i++)
      if(s[i] == 1) c++;
   if(c % 2 == 0) return 0;
   else return 1;
}

void display(int *s, int l)
{
   int i;
   for(i = 0; i < l; i++)
      printf("%d",s[i]);
   printf("\n");
}

char par(int *s)
{
   int c = 0, i;
   for(i = 0; i < 8; i++)
      if(s[i] == 1) c++;
   if(c % 2 == 0) return 'E';
   else return 'O';
}

void print(char a)
{
   if(a == 'O') printf("ODD!\n");
   else if(a == 'E') printf("EVEN!\n");
   else printf("Invalid!\n");
}

int main()
{
   char str[100];
   printf("Enter string input: \n");
   fgets(str, sizeof(str), stdin);
   str[strcspn(str, "\n")] = '\0';
   int len = strlen(str);
   if(len == 0)
   {
      printf("Empty string entered!\n");
      return 0;
   }

   // Dynamically allocate a 2D array for the 8-bit streams of all characters
   int (*s)[8] = malloc(len * sizeof(*s));

   int i, j;
   for(i = 0; i < len; i++)
   {
      int val = (int)str[i];
      for(j = 6; j >= 0; j--)
      {
         s[i][j] = val % 2;
         val /= 2;
      }
      printf("Character '%c' 7_bit stream: ", str[i]);
      display(s[i], 7);
      int p = even(s[i]);
      s[i][7] = p;
      printf("The generated even parity bit of the given 7-bit string is: %d\n", p);
      printf("Parity added 8_bit stream: ");
      display(s[i], 8);
      printf("\n");
   }

   printf("--- All 8-bit Streams ---\n");
   for(i = 0; i < len; i++)
   {
      printf("Char %d ('%c'): ", i, str[i]);
      display(s[i], 8);
   }
   printf("\n");

   int option;
   while(1)
   {
      printf("Options:\n1. Exit and display\n2. Change a bit\nEnter option: ");
      if(scanf("%d", &option) != 1) break;
      if(option == 1)
      {
         printf("\nFinal status of all 8-bit streams:\n");
         for(i = 0; i < len; i++)
         {
            printf("Char %d ('%c'): ", i, str[i]);
            display(s[i], 8);
         }
         break;
      }
      else if(option == 2)
      {
         int char_num, pos, bit_val;
         printf("Enter the character number (0-%d): ", len - 1);
         scanf("%d", &char_num);
         if(char_num >= len || char_num < 0)
         {
            printf("Invalid character number!\n\n");
            continue;
         }

         printf("Enter the change position(0-7) and bit to check for parity: ");
         scanf("%d %d", &pos, &bit_val);
         if(pos > 7 || pos < 0)
         {
            printf("Invalid position!\n\n");
            continue;
         }
         if(bit_val != 0 && bit_val != 1)
         {
            printf("Invalid bit value! Must be 0 or 1.\n\n");
            continue;
         }

         s[char_num][pos] = bit_val;

         printf("The new string for Char %d: ", char_num);
         display(s[char_num], 8);

         char parity = par(s[char_num]);
         printf("The parity of the new string is: ");
         print(parity);
         printf("\n");

         printf("Current status of all 8-bit streams:\n");
         for(i = 0; i < len; i++)
         {
            printf("Char %d ('%c'): ", i, str[i]);
            display(s[i], 8);
         }
         printf("\n");
      }
      else printf("Invalid option! Please choose 1 or 2.\n\n");
   }
   free(s);
   return 0;
}
