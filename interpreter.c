/*
Interpreter
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
int getNum();
void emit(char *fmt, ...);

int factor();
int term();
int expression();

/* PROGRAMA PRINCIPAL */
int main()
{
	init();
	printf("%d\n", expression());

	return 0;
}

/* inicialização do compilador */
void init()
{
	nextChar();
}

/* lê próximo caracter da entrada */
void nextChar()
{
	look = getchar();
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
	va_list args;
	
	fputs("Error: ", stderr);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	
	fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
	va_list args;
	
	fputs("Error: ", stderr);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	
	fputc('\n', stderr);

	exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
	va_list args;
	
	fputs("Error: ", stderr);

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	
	fputs(" expected!\n", stderr);
	
	exit(1);
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
	if (look != c)
		expected("'%c'", c);
	nextChar();
}

/* recebe o nome de um identificador */
char getName()
{
	char name;

	if (!isalpha(look))
		expected("Name");
	name = toupper(look);
	nextChar();

	return name;
}

/* recebe um número inteiro */
int getNum()
{
	int i;

	i = 0;

	if (!isdigit(look))
		expected("Integer");

	while (isdigit(look)) {
		i *= 10;
		i += look - '0';
		nextChar();
	}
	
        return i;
}


/* emite uma instrução seguida por uma nova linha */
void emit(char *fmt, ...)
{
	va_list args;
	
	putchar('\t');

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);

	putchar('\n');
}

/* reconhece operador aditivo */
int isAddOp(char c)
{
        return (c == '+' || c == '-');
}

/* reconhece operador multiplicativo */
int isMulOp(char c)
{
        return (c == '*' || c == '/');
}

/* avalia um fator */
int factor()
{
        int val;

        if (look == '(') {
                match('(');
                val = expression();
                match(')');
        } else
                val = getNum();

        return val;
}

/* avalia um termo */
int term()
{
        int val;

        val = factor();
        while (isMulOp(look)) {
                switch (look) {
                  case '*':
                        match('*');
                        val *= factor();
                        break;
                  case '/':
                        match('/');
                        val /= factor();
                        break;
                }
        }

        return val;
}

/* avalia o resultado de uma expressão */
int expression()
{
	int val;

	if (isAddOp(look))
		val = 0;
	else
		val = term();

	while (isAddOp(look)) {
		switch (look) {
		  case '+':
		  	match('+');
		  	val += term();
		  	break;
		  case '-':
		  	match('-');
		  	val -= term();
		  	break;
		}
	}
	
	return val;
}


