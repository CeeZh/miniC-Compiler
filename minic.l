%{
#include "stdio.h" 
#include "minic.tab.h"
#include "string.h"

#define ERRORCHAR 800
#define NEWLINE 801

extern YYSTYPE yylval;
extern int htoi(const char *str);

%}
%option yylineno

delim		[ \t \n]
ws		{delim}+
letter		[A-Za-z]
digit		[0-9]
id		{letter}({letter}|{digit})*
num		{digit}|([1-9]{digit}*)
hex		0(x|X)([a-f]|[A-F]|[0-9])+

%s COMMENT
%s COMMENT2
%%

<INITIAL>"/*"		{BEGIN COMMENT;}
<COMMENT>"*/"		{BEGIN INITIAL;}
<COMMENT>.|\n		{;}
<INITIAL>"//"		{BEGIN COMMENT2;}
<COMMENT2>\n		{BEGIN INITIAL;}
<COMMENT2>.		{;}

<INITIAL>while		{return (WHILE);}
<INITIAL>if		{return (IF);}
<INITIAL>else		{return (ELSE);}
<INITIAL>return		{return (RETURN);}
<INITIAL>void		{return VOID;}
<INITIAL>int		{return INT;}
<INITIAL>break		{return BREAK;}
<INITIAL>continue		{return CONTINUE;}

<INITIAL>{id}	          	{strcpy(yylval.id , yytext); return ID;}
<INITIAL>{hex}	      	{yylval.num=htoi(yytext); return HEX;}
<INITIAL>{num}	      	{yylval.num=atoi(yytext); return (INTEGER);}
<INITIAL>{ws}	          	{;}

<INITIAL>"<="	          	{return LE;}
<INITIAL>">="	         	{return GE;}
<INITIAL>"=="		{return EQ;}
<INITIAL>"!="	          	{return NE;}
<INITIAL>"&&"		{return AND;}
<INITIAL>"||"		{return OR;}
<INITIAL>"<<"		{return LS;}
<INITIAL>">>"		{return RS;}
<INITIAL>"<"	          	{return '<';}
<INITIAL>">"	          	{return '>';}
<INITIAL>"~"		{return '~';}
<INITIAL>"+"	          	{return '+';}
<INITIAL>"-"	          	{return '-';}
<INITIAL>"*"	          	{return '*';}
<INITIAL>"/"	          	{return '/';}
<INITIAL>"%"		{return '%';}
<INITIAL>"="	          	{return '=';}
<INITIAL>"&"		{return '&';}
<INITIAL>"$"		{return '$';}
<INITIAL>"("	          	{return '(';}
<INITIAL>")"	          	{return ')';}
<INITIAL>"["	          	{return '[';}
<INITIAL>"]"	          	{return ']';}
<INITIAL>"{"	          	{return '{';}
<INITIAL>"}"	          	{return '}';}
<INITIAL>";"	          	{return ';';}
<INITIAL>","		{return ',';}
<INITIAL>.		{return ERRORCHAR;}

%%

int yywrap()
{
    return 1;
}

/*
void writeout(int c)
{
    switch (c)
    {
    case ERRORCHAR:
        fprintf(yyout, "(ERRORCHAR, \"%s\") ", yytext);
        fprintf(yyout, "(ERRORCHAR, %d) ", yylineno);
        break;
    case WHILE:
        fprintf(yyout, "(WHILE, \"%s\") ", yytext);
        fprintf(yyout, "(WHILE, %d) ", yylineno);
        break;
    case IF:
        fprintf(yyout, "(IF, \"%s\") ", yytext);
        break;
    case ELSE:
        fprintf(yyout, "(ELSE, \"%s\") ", yytext);
        break;
    case INTEGER:
        fprintf(yyout, "(INTEGER, \"%s\") ", yytext);
        break;
    case HEX:
        fprintf(yyout, "(HEX, \"%s\") ", yytext);
        break;
    case ID:
        fprintf(yyout, "(ID, \"%s\") ", yytext);
        break;
    case NEWLINE:
        fprintf(yyout, "\n");
        break;
    case '+':
        fprintf(yyout, "(ADD, ");
        ECHO;
        fprintf(yyout, ") ");
        break;
    case RETURN:
        fprintf(yyout, "(RETURN, \"%s\") ", yytext);
        break;
    default:
        fprintf(yyout, "line: %d\n", yylineno);
        break;
    }
    return;
}


int main(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i)
    {
        printf("%s\n", argv[i]);
    }
    int c, j = 0;
    if (argc >= 2)
    {
        if ((yyin = fopen(argv[1], "r")) == NULL)
        {
            printf("Can't open file %s\n", argv[1]);
            return 1;
        }
        if (argc >= 3)
        {
            yyout = fopen(argv[2], "w");
        }
    }

    while (c = yylex())
    {
        writeout(c);
        writeout(NEWLINE);
    }

    if (argc >= 2)
    {
        fclose(yyin);
        if (argc >= 3)
            fclose(yyout);
    }
    return 0;
}
*/