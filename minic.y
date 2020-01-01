%{
#include "stdio.h"
#include "def.h"

extern int yylineno;
extern char *yytext;
extern FILE *yyin;
extern FILE *yyout;

int yylex();
void yyerror(const char * msg);
void initialize();
%}

%union{
    int num;
    char id[32];
    char idType[32];
    EXPRSTRUCT *es;
    int codeLen;
};

%token VOID INT WHILE IF ELSE RETURN AND OR LS RS
%token CONTINUE BREAK
%token EQ NE LE GE

%token<id> ID
%token<num> INTEGER
%token<num> HEX

%type<num> int_literal
%type<es> expr
%type<idType> type_spec
%type<codeLen> stmt_list stmt expr_stmt block_stmt if_stmt while_stmt return_stmt continue_stmt break_stmt

%left OR AND  

%left EQ NE LE GE '<' '>'  
%left '+' '-' 

%left '&' '^' 
'|' 
%left '*' '/' 
%left '%'   
%right LS RS 

%right '!' '~' '$'
 

%nonassoc UMINUS 

%nonassoc ELSE

%start program 

%%

/*�����ɱ�����������������ɣ�decl��*/
program  : decl_list        {fprintf(yyout,"line %d\n",yylineno);}
	; 

decl_list  : decl_list decl         {fprintf(yyout,"line %d\n",yylineno);}
	| decl         {fprintf(yyout,"line %d\n",yylineno);}
	; 


decl  : var_decl	{;}
	| fun_decl	         {;}
	; 

/*���������򵥱�����һά�������*/
var_decl  : type_spec ID ';'         {installGlobalVar($2,$1);/*���������ܱ�*/}
	| type_spec ID '[' int_literal ']' ';'         {installGlobalArray($2,$1,$4);/*��������ܱ�*/}
	; 

/*��������ֵ���ͻ�������Ͱ������ͻ�VOID*/
type_spec  : VOID	{strcpy($$,"void"); fprintf(yyout,"line %d\n",yylineno);}
	| INT         {strcpy($$,"int"); fprintf(yyout,"line %d\n",yylineno);}
	;

/*Ҫ��������ȫ�ֺ�����ϢΪ��*/ 
fun_decl   : type_spec ID '(' params ')' compound_stmt        {installFunc($2,$1); /*�����������ܱ����ܻ��庯�����ű�*/}
	;  

/*��������������Ϊ0��������Ϊ0ʱ�������б�Ҫ��VOID*/
params   :  param_list         {;}
	| VOID         {;}
	;  

param_list  : param_list ',' param         {fprintf(yyout,"line %d\n",yylineno);}
	| param         {fprintf(yyout,"line %d\n",yylineno);}
	; 

param  :  type_spec ID        {installPara($2,$1); /*�βμ�����ű�����ǰ���л�����������������Ŀ�����ޣ����½���������������Ŀ*/}
	; 

/*�����ڲ������������ֲ��������������*/
compound_stmt : '{' local_decls stmt_list '}'          {fprintf(yyout,"line %d\n",yylineno);}
	; 

/*�����ڲ���������*/
local_decls  :         
	| local_decls local_decl         {fprintf(yyout,"line %d\n",yylineno);}
	;  


local_decl   : type_spec ID ';'         {installVar($2,$1);/*������ű�����ǰ���л�����������������Ŀ�����ޣ����½���������������Ŀ*/}
	| type_spec ID '[' int_literal ']' ';'         {installArray($2,$1,$4); /*������ű�����ǰ���л�����������������Ŀ�����ޣ����½���������������Ŀ*/}
	; 


stmt_list  :  stmt         {$$=$1;fprintf(yyout,"stmt_list ->stmt   at line %d\n",yylineno);}
	| stmt_list stmt         {$$=$1+$2;}
	;

stmt  :  expr_stmt         {$$=$1;fprintf(yyout,"stmt->expr_stmt  at line %d\n",yylineno);}
	| block_stmt         {$$=$1;}
	| if_stmt         {$$=$1;}
	| while_stmt         {$$=$1;}
	| return_stmt         {$$=$1;}
	| continue_stmt         {$$=$1;}
	| break_stmt        {$$=$1;}

/*��ֵ���*/
expr_stmt  : ID '=' expr ';' %prec ELSE        {$$=$3->len+1; gen("=",$3->exprId,"",$1);fprintf(yyout,"expr_stmt->ID=expr  at line %d\n",yylineno);}
	| ID '[' expr ']' '=' expr ';' %prec ELSE         {$$=$3->len+$6->len+1;gen("[]=",$3->exprId,$6->exprId,$1);}
	| '$' expr '=' expr ';' %prec ELSE         {$$=$2->len+$4->len+1;gen("$=",$4->exprId,"",$2->exprId);}
	| ID '(' args ')' ';' %prec ELSE         {;}
	;  

/*����*/ 
block_stmt : '{' stmt_list '}'          {$$=$2;fprintf(yyout,"block_stmt->{ stmt_list }  at line %d\n",yylineno);}
	;

if_stmt  :  IF '(' expr ')' stmt %prec UMINUS         {$$=$3->len+$5+2; genIf($3,$5);fprintf(yyout,"if_stmt ->IF '(' expr ')' stmt  at line %d\n",yylineno);}
	| IF '(' expr ')'  stmt  ELSE stmt         {$$=$3->len+$5+$7+2; genIfElse($3,$5,$7);}
	; 

/*WHILE ���*/ 
while_stmt : WHILE '(' expr ')' stmt         {$$=$3->len+$5+2; genWhile($3,$5);}
	; 

return_stmt  : RETURN ';'         {$$=1; gen("ret","","","");}
	| RETURN expr ';'         {$$=$2->len+1; gen("ret","","",$2->exprId);}
	; 


expr  :  expr OR expr         {$$=newTemp($1,$3); gen1("or",$$,$1,$3);}
	| expr EQ expr         {$$=newTemp($1,$3); gen1("eq",$$,$1,$3);}
	| expr NE expr          {$$=newTemp($1,$3); gen1("ne",$$,$1,$3);}
	| expr LE expr         {$$=newTemp($1,$3); gen1("le",$$,$1,$3);}
	| expr '<' expr         {$$=newTemp($1,$3); gen1("<",$$,$1,$3);}
	| expr GE expr         {$$=newTemp($1,$3); gen1("ge",$$,$1,$3);}
	| expr '>' expr         {$$=newTemp($1,$3); gen1(">",$$,$1,$3);}
	| expr AND expr  
          {$$=newTemp($1,$3); gen1("and",$$,$1,$3);}
	| '!' expr %prec UMINUS         {$$=newTemp($2,$2); gen2("!",$$,$2);}
	| expr '+' expr         {$$=newTemp($1,$3); gen1("+",$$,$1,$3);}
	| expr '-' expr          {$$=newTemp($1,$3); gen1("-",$$,$1,$3);}
	| expr '*' expr         {$$=newTemp($1,$3); gen1("*",$$,$1,$3);}
	| expr '/' expr         {$$=newTemp($1,$3); gen1("/",$$,$1,$3);}
	| expr '%' expr          {$$=newTemp($1,$3); gen1("%",$$,$1,$3);}
	| expr '&' expr           {$$=newTemp($1,$3); gen1("&",$$,$1,$3);}
	| expr '^' expr           {$$=newTemp($1,$3); gen1("^",$$,$1,$3);}
	| '~' expr          {$$=newTemp($2,$2); gen2("~",$$,$2);}
	| expr LS expr           {$$=newTemp($1,$3); gen1("ls",$$,$1,$3);}
	| expr RS expr  
          {$$=newTemp($1,$3); gen1("rs",$$,$1,$3);}
	| expr '|' expr           {$$=newTemp($1,$3); gen1("|",$$,$1,$3);}
	| '(' expr ')' 
          {$$=$2;}
	| ID         {$$=constructExprId($1);/*����ID������ű�����expr*/;fprintf(yyout,"expr->ID  at line %d\n",yylineno);}
	| ID '[' expr ']'         {/*����ID������ű�����expr*/;}
	| ID '(' args ')'         {/*����ID������ű�����expr*/;}
	| int_literal  
          {$$=constructExpr(newLiteral($1));/* ���볣���� ��������expr*/}
	;

/*��ֵ������ʮ��������*/
int_literal  :  INTEGER        {$$=$1;}
	|  HEX          {$$=$1;}
	;
args  :         
	| arg_list        {fprintf(yyout,"line %d\n",yylineno);}
	;

arg_list  :  arg_list ',' expr         {fprintf(yyout,"line %d\n",yylineno);}
	| expr        {fprintf(yyout,"line %d\n",yylineno);}
	;

continue_stmt : CONTINUE ';'         {fprintf(yyout,"line %d\n",yylineno);}
	; 


break_stmt : BREAK ';'         {fprintf(yyout,"line %d\n",yylineno);}
	; 

%%

void yyerror(const char * msg){
    fprintf(yyout,"Error at line number %d\n%s\n", yylineno, msg);
}

void initialize(){
    funcTable.size=0;
    globalTable.size=0;
    pSymTable = (SYMTABLE*)malloc(sizeof(SYMTABLE));   //�����µĺ�����
    pSymTable->size = 0;
    constTable.size=0;
    cnHead=NULL;
}

int main(int argc, char **argv)
{
    initialize();
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

    yyparse();
    display();

    if (argc >= 2)
    {
        fclose(yyin);
        if (argc >= 3)
            fclose(yyout);
    }
    return 0;
}
