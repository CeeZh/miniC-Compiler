#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAXSIZE 500

/*
globalTable和symTable的表项
*/
typedef struct {
	char name[32];
	char type[32];
	char cat[32];
	int len;
	int offset;
}SYMITEM;

/*
symTable
*/
typedef struct {
	SYMITEM table[MAXSIZE];
	int size;
}SYMTABLE;

/*
golbalTable
*/
typedef struct {
	SYMITEM table[MAXSIZE];
	int size;
}GLOBALTABLE;

/*
funcTable表项
*/
typedef struct {
	char name[32];
	char type[32];
	SYMTABLE* ptr;
}FUNCTABLEITEM;

/*
funcTable
*/
typedef struct {
	FUNCTABLEITEM table[MAXSIZE];
	int size;
}FUNCTABLE;

/*
constTable表项
*/
typedef struct {
	char name[32];
	int val;
	int offset;
}CONSTTABLEITEM;

/*
constTable
*/
typedef struct {
	CONSTTABLEITEM table[MAXSIZE];
	int size;
}CONSTTABLE;

/*
expr非终结符
*/
typedef struct {
	char exprId[32];
	char exprType[32];
	int len; //对应四元式的数量
	int no; //数组变量的索引
}EXPRSTRUCT;

/*
四元式节点
*/
typedef struct NODE{
	char n1[32];
	char n2[32];
	char n3[32];
	char n4[32];
	int offset;
	struct NODE *next;
	struct NODE *pre;
}CODENODE;

FUNCTABLE funcTable;
///////////////////////////main函数开始一定要写 funcTable.size=0 !!!!!!!!!!!!!!!!!!
GLOBALTABLE globalTable;
///////////////////////////main函数开始一定要写 globalTable.size=0 !!!!!!!!!!!!!!!!!!

SYMTABLE* pSymTable;
/*      main函数开始时一定要写 ！！！！！！！！！！
pSymTable = (SYMTABLE*)malloc(sizeof(SYMTABLE));   //定义新的函数表
pSymTable->size = 0;
*/

//表示中间变量的数量
////////////////////main函数开始时和函数符号表汇总时一定要初始化//////////////////

CONSTTABLE constTable;
///////////////////////////main函数开始一定要写 constTable.size=0 !!!!!!!!!!!!!!!!!!

CODENODE* cnHead;
CODENODE* cnTail;
///////////////////////////main函数开始一定要初始化 !!!!!!!!!!!!!!!!!!

int htoi(const char *str);
int funcTableSearch(const FUNCTABLEITEM *fti);
int globalTableSearch(const SYMITEM *si);
int symTableSearch(const SYMITEM *si);
void funcTableAdd(const FUNCTABLEITEM *fti);
void globalTableAdd(const SYMITEM *si);
void symTableAdd(const SYMITEM *si);
int installGlobalVar(const char *id, const char *type);
int installGlobalArray(const char *id, const char *type, const int size);
int installFunc(const char *id, const char *type);
int installPara(const char *id, const char *type);
int installVar(const char *id, const char *type);
int installArray(const char *id, const char *type, const int size);
void displayFuncTable();
void displaySymTable();
void displayGlobalTable();
void displayConstantTable();
void display();
char* newLiteral(const int val);
EXPRSTRUCT* newTemp(EXPRSTRUCT* es1, EXPRSTRUCT* es2);
CODENODE* makeNode(const char *n1, const char *n2, const char *n3, const char* n4);
CODENODE* makeNode2(const char * n1, const char * n2, const char * n3, const char * n4, int offset);
void gen1(const char *str, EXPRSTRUCT* es, EXPRSTRUCT* es1, EXPRSTRUCT* es2);
void gen2(const char *str, EXPRSTRUCT* es, EXPRSTRUCT* es1);
void gen(const char *n1, const char* n2, const char* n3, const char* n4);
void insert(CODENODE *cur, int num, CODENODE *src);
void insertBack(CODENODE* cn);
EXPRSTRUCT* constructExpr(const char* id);
EXPRSTRUCT* constructExprId(const char* id);
void genIf(EXPRSTRUCT* es, const int len2);
void genIfElse(EXPRSTRUCT* es, const int len2, const int len3);
void genWhile(EXPRSTRUCT* es, const int len2);





