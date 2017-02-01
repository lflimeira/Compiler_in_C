#include <stdio.h>

main() {
 int c;
 int i=0;
 while((c = getchar()) != -1) if (c == 'a') i++;
 printf("Foram lidos %d a\'s",i);
}