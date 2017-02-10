

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */
int lblcount; /* indica o rótulo atual */

#define SYMTBL_SZ 1000
#define KWLIST_SZ 4

char *symtbl[SYMTBL_SZ]; /* tabela de símbolos */
char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "END"}; /* palavras-chave */

char *kwcode = "ilee"; /* código para as palavras-chave */

#define MAXTOKEN 10
#define MAXNAME 8
#define MAXNUM 5
#define MAXOP 2

char token; /* token codificado */
char value[MAXTOKEN+1]; /* valor do token não codificado */

/* protótipos */
void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void match(char c);
void getname();
void getnum();
int isaddop(char c);
int ismulop(char c);
void skipwhite();
void newline();

int newlabel();

int lookup(char *s, char *list[], int size);
void scan();

void ident();
void factor();
void signedfactor();
void term1();
void firstterm();
void term();
void expression();
void assignment();
void do_add();
void do_sub();
void do_mul();
void do_div();
void condition();

void doif();
void block();
void program();


int main()
{
	init();
        program();

	return 0;
}

void init()
{
        lblcount = 0;
	nextchar();
        skipwhite();
}

void nextchar()
{
	look = getchar();
}

void error(char *s)
{
	fprintf(stderr, "Error: %s\n", s);
}

void fatal(char *s)
{
	error(s);
	exit(1);
}

void expected(char *s)
{
	fprintf(stderr, "Error: %s expected\n", s);
	exit(1);
}

void match(char c)
{
	char s[2];

	if (look != c) {
		s[0] = c; /* uma conversao rápida (e feia) */
		s[1] = '\0'; /* de um caracter em string */
		expected(s);
	}
        nextchar();
        skipwhite();
}

void getname()
{
	int i;
	
        while (look == '\n')
                newline();
	if (!isalpha(look))
		expected("Name");
	for (i = 0; isalnum(look) && i < MAXNAME; i++) {
		value[i] = toupper(look);
		nextchar();
	}
	value[i] = '\0';
        token = 'x';
        skipwhite();
}

void getnum()
{
	int i;
	
	if (!isdigit(look))
		expected("Integer");
	for (i = 0; isdigit(look) && i < MAXNUM; i++) {
		value[i] = look;
		nextchar();
	}
	value[i] = '\0';
        token = '#';
        skipwhite();
}

int isaddop(char c)
{
        return (c == '+' || c == '-');
}

int ismulop(char c)
{
        return (c == '*' || c == '/');
}

void skipwhite()
{
	while (look == ' ' || look == '\t')
		nextchar();
}

void newline()
{
	if (look == '\n')
		nextchar();
        skipwhite();
}

int newlabel()
{
	return lblcount++;
}

int lookup(char *s, char *list[], int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if (strcmp(list[i], s) == 0)
			return i;
	}

	return -1;
}

void scan()
{
        int kw;

	getname();
        kw = lookup(value, kwlist, KWLIST_SZ);
        if (kw == -1)
                token = 'x';
        else
                token = kwcode[kw];
}

void matchstring(char *s)
{
        if (strcmp(value, s) != 0)
                expected(s);
}

void ident()
{
        getname();
        if (look == '(') {
                match('(');
                match(')');
                printf("\tcall %s\n", value);
        } else
                printf("\tmov ax, [%s]\n", value);
}

void factor()
{

        if (look == '(') {
                match('(');
                expression();
                match(')');
        } else if(isalpha(look)) {
                ident();
        } else  {
                getnum();
        	printf("\tmov ax, %s\n", value);
        }
}

void signedfactor()
{
        int s;

        s = (look == '-');
        if (isaddop(look)) {
                nextchar();
                skipwhite();
        }
        factor();
        if (s)
                printf("\tneg ax\n");
}

void term1()
{
	while (ismulop(look)) {
		printf("\tpush ax\n");
		switch(look) {
		  case '*':
		  	do_mul();
		  	break;
		  case '/':
		  	do_div();
		  	break;
		}
	}
}

void firstterm()
{
        signedfactor();
        term1();
}

void term()
{
        factor();
        term1();
}

void expression()
{
       	firstterm();
	while (isaddop(look)) {
		printf("\tpush ax\n");
		switch(look) {
		  case '+':
		  	do_add();
		  	break;
		  case '-':
		  	do_sub();
		  	break;
		}
	}
}

void assignment()
{
        char name[MAXNAME+1];

        strcpy(name, value);
	match('=');
	expression();
	printf("\tlea bx, [%s]\n", name);
	printf("\tmov [bx], ax\n");
}

void do_add()
{
        match('+');
        term();
      	printf("\tpop bx\n");
        printf("\tadd ax, bx\n");
}

void do_sub()
{
        match('-');
        term();
      	printf("\tpop bx\n");
        printf("\tsub ax, bx\n");
        printf("\tneg ax\n");
}

void do_mul()
{
        match('*');
        factor();
      	printf("\tpop bx\n");
        printf("\tmul bx\n");
}

void do_div()
{
        match('/');
        factor();
      	printf("\tpop bx\n");
        printf("\txchg ax, bx\n");
        printf("\tdiv bx\n");
}

void condition()
{
	printf("\t## condition ##\n");
}

void doif()
{
	int l1, l2;
	
	condition();
	l1 = newlabel();
	l2 = l1;
	printf("\tjz L%d\n", l1);
	block();
	if (token == 'l') {
		l2 = newlabel();
		printf("\tjmp L%d\n", l2);
		printf("L%d:\n", l1);
		block();
	}
	printf("L%d:\n", l2);
	matchstring("ENDIF");
}

void block()
{
	int follow = 0;
	
	do {
                scan();
		switch (token) {
		  case 'i':
		   	doif(); break;
		  case 'e':
		  case 'l':
		   	follow = 1;
		   	break;
		  default:
		   	assignment(); break;
                }
	} while (!follow);
}

void program()
{
	block();
        matchstring("END");
	printf("\tint 20h\n");
}


