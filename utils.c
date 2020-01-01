#include "def.h"

extern void yyerror(const char *msg);


int htoi(const char *str)
{
	int i = 0;
	double power = 0.0;
	int length = 0;
	int sum = 0;
	int digit = 0;

	if (str[i] == '0') //Case starting with 0X
	{
		i++;
		if (str[i] == 'x' || str[i] == 'X')
		{
			i++;
			length = strlen(str) - 2;
			for (int j = length - 1 + 2; j >= 2; j--)
			{
				if (str[j] >= '0' && str[j] <= '9')
				{
					digit = str[j] - '0';
				}
				else if (str[j] >= 'a' && str[j] <= 'f')
				{
					digit = str[j] - 'a' + 10;
				}
				else if (str[j] >= 'A' && str[j] <= 'F')
				{
					digit = str[j] - 'A' + 10;
				}

				sum = sum + (digit * (pow(16.0, power)));
				power++;
			}
		}
	}
	else //Case without 0X
	{
		length = strlen(str);
		for (int j = length - 1; j >= 0; j--)
		{
			if (str[j] >= '0' && str[j] <= '9')
			{
				digit = str[j] - '0';
			}
			else if (str[j] >= 'a' && str[j] <= 'f')
			{
				digit = str[j] - 'a' + 10;
			}
			else if (str[j] >= 'A' && str[j] <= 'F')
			{
				digit = str[j] - 'A' + 10;
			}

			sum = sum + (digit * (pow(16.0, power)));
			power++;
		}
	}
	return sum;
}

/*
在函数总表funcTable中找fti表项，若已存在，则返回1
*/
int funcTableSearch(const FUNCTABLEITEM *fti)
{
	for (int i = 0; i < funcTable.size; i++)
	{
		if ((strcmp(funcTable.table[i].name, fti->name) == 0) && (strcmp(funcTable.table[i].type, fti->type) == 0))
		{
			return 1;
		}
	}
	return 0;
}

/*
在全局变量表globalTable中找si表项，若已存在，则返回1
*/
int globalTableSearch(const SYMITEM *si) {
	for (int i = 0; i<globalTable.size; ++i) {
		if ((strcmp(globalTable.table[i].name, si->name) == 0) && (strcmp(globalTable.table[i].type, si->type) == 0)) {
			return 1;
		}
	}
	return 0;
}

/*
在当前函数符号表pSymTable中找si表项，若已存在，则返回1
*/
int symTableSearch(const SYMITEM *si)
{
	for (int i = 0; i < pSymTable->size; i++)
	{
		if ((strcmp(pSymTable->table[i].name, si->name) == 0) && (strcmp(pSymTable->table[i].type, si->type) == 0) )
		{
			return 1;
		}
	}
	return 0;
}

/*
在funcTable中添加si项
*/
void funcTableAdd(const FUNCTABLEITEM *fti)
{
	strcpy(funcTable.table[funcTable.size].name, fti->name);
	strcpy(funcTable.table[funcTable.size].type, fti->type);
	funcTable.table[funcTable.size].ptr = fti->ptr;
	funcTable.size++;
}

/*
在全局变量表globalTable中添加si项
*/
void globalTableAdd(const SYMITEM *si) {
	strcpy(globalTable.table[globalTable.size].cat, si->cat);
	strcpy(globalTable.table[globalTable.size].name, si->name);
	strcpy(globalTable.table[globalTable.size].type, si->type);
	globalTable.table[globalTable.size].len = si->len;
	//计算offset
	if (globalTable.size == 0) {
		globalTable.table[globalTable.size].offset = 0;
	}
	else {
		globalTable.table[globalTable.size].offset = globalTable.table[globalTable.size - 1].offset + 4 * globalTable.table[globalTable.size - 1].len;
	}
	globalTable.size++;
}

/*
在当前函数符号表pSymTable中添加si项
*/
void symTableAdd(const SYMITEM *si)
{
	strcpy(pSymTable->table[pSymTable->size].cat, si->cat);
	strcpy(pSymTable->table[pSymTable->size].name, si->name);
	strcpy(pSymTable->table[pSymTable->size].type, si->type);
	pSymTable->table[pSymTable->size].len = si->len;
	//计算offset
	if (pSymTable->size == 0) {
		pSymTable->table[pSymTable->size].offset = 0;
	}
	else {
		pSymTable->table[pSymTable->size].offset = pSymTable->table[pSymTable->size - 1].offset + 4 * pSymTable->table[pSymTable->size - 1].len;
	}
	pSymTable->size++;
}

/*
id:变量名
type:变量类型
返回1：重复声明错误；返回0：正常
*/
int installGlobalVar(const char *id, const char *type)
{
	//错误处理（void a）：
	if (type == "void")
	{
		yyerror("variable type: void");
	}
	SYMITEM symItem;
	strcpy(symItem.name, id);
	strcpy(symItem.type, type);
	strcpy(symItem.cat, "var");
	symItem.len = 1;

	//错误处理（重复定义）:
	int f = globalTableSearch(&symItem);
	if (f == 0)
	{
		globalTableAdd(&symItem);
	}
	else
	{
		yyerror("variable already declared");
	}
	return f;
}

/*
id:数组名
type:数组类型
size:数组长度
*/
int installGlobalArray(const char *id, const char *type, const int size)
{
	//错误处理（int a[-1]）：
	if (size < 0)
	{
		yyerror("array size less than 0");
	}
	//错误处理（void a[5]）：
	if (type == "void")
	{
		yyerror("array type: void");
	}
	SYMITEM symItem;
	strcpy(symItem.name, id);
	strcpy(symItem.type, type);
	strcpy(symItem.cat, "arr");
	symItem.len = size;

	//错误处理（重复定义）:
	int f = globalTableSearch(&symItem);
	if (f == 0)
	{
		globalTableAdd(&symItem);
	}
	else
	{
		yyerror("array already declared");
	}
	return f;
}

/*
id:函数名
type:函数返回类型
*/
int installFunc(const char *id, const char *type)
{
	FUNCTABLEITEM funcTableItem;
	strcpy(funcTableItem.name, id);
	strcpy(funcTableItem.type, type);
	funcTableItem.ptr = pSymTable;						 //函数的符号表地址加入总表
	pSymTable = (SYMTABLE *)malloc(sizeof(SYMTABLE)); //定义新的函数表
	pSymTable->size = 0;

	//错误处理（重复定义）:
	int f = funcTableSearch(&funcTableItem);
	if (f == 0)
	{
		funcTableAdd(&funcTableItem);
	}
	else
	{
		yyerror("function already declared");
	}
	return f;
}

int installPara(const char *id, const char *type)
{
	//错误处理（void a）：
	if (type == "void")
	{
		yyerror("parameter type: void");
	}

	SYMITEM symItem;
	strcpy(symItem.name, id);
	strcpy(symItem.type, type);
	strcpy(symItem.cat, "para");
	symItem.len = 1;

	//错误处理（重复定义）:
	int f = symTableSearch(&symItem);
	if (f == 0)
	{
		symTableAdd(&symItem);
	}
	else
	{
		yyerror("parameter already declared");
	}
	return f;
}

int installVar(const char *id, const char *type)
{
	//错误处理（void a）：
	if (type == "void")
	{
		yyerror("variable type: void");
	}
	SYMITEM symItem;
	strcpy(symItem.name, id);
	strcpy(symItem.type, type);
	strcpy(symItem.cat, "var");
	symItem.len = 1;

	//错误处理（重复定义）:
	int f = symTableSearch(&symItem);
	if (f == 0)
	{
		symTableAdd(&symItem);
	}
	else
	{
		yyerror("variable already declared");
	}
	return f;
}

int installArray(const char *id, const char *type, const int size)
{
	//错误处理（int a[-1]）：
	if (size < 0)
	{
		yyerror("array size less than 0");
	}
	//错误处理（void a）：
	if (type == "void")
	{
		yyerror("array type: void");
	}
	SYMITEM symItem;
	strcpy(symItem.name, id);
	strcpy(symItem.type, type);
	strcpy(symItem.cat, "array");
	symItem.len = size;

	//错误处理（重复定义）:
	int f = symTableSearch(&symItem);
	if (f == 0)
	{
		symTableAdd(&symItem);
	}
	else
	{
		yyerror("array already declared");
	}
	return f;
}

void displayFuncTable() {
	printf("function table below:\n");
	FUNCTABLEITEM* fti;
	for (int i = 0; i < funcTable.size; ++i) {
		fti = &funcTable.table[i];
		printf("%s\t", fti->name);
		printf("%s\t", fti->type);
		printf("%s\t", "look");
		printf("\n");
	}
	printf("\n");
}

void displaySymTable() {
	printf("symbol table below:\n");
	FUNCTABLEITEM* fti;
	SYMITEM* s;
	for (int i = 0; i < funcTable.size; ++i) {
		fti = &funcTable.table[i];
		printf(fti->name);
		printf(":\n");
		SYMTABLE* p = fti->ptr;
		for (int i = 0; i < p->size; i++) {
			s = &(p->table[i]);
			printf("%s\t", s->name);
			printf("%s\t", s->type);
			printf("%s\t", s->cat);
			printf("%d\t", s->len);
			printf("%d\t", s->offset);
			printf("\n");
		}
		printf("\n");
	}
	printf("\n");
}

void displayGlobalTable() {
	printf("global table below:\n");
	SYMITEM* s;
	for (int i = 0; i < globalTable.size; i++) {
		s = &globalTable.table[i];
		printf("%s\t", s->name);
		printf("%s\t", s->type);
		printf("%s\t", s->cat);
		printf("%d\t", s->len);
		printf("%d\t", s->offset);
		printf("\n");
	}
	printf("\n");
}

void displayConstantTable() {
	printf("constant table below:\n");
	CONSTTABLEITEM* c;
	for (int i = 0; i < constTable.size; i++) {
		c = &constTable.table[i];
		printf("s%\t", c->name);
		printf("d%\t", c->val);
		printf("d%\t", c->offset);
		printf("\n");
	}
	printf("\n");
}

void display() {
	displayConstantTable();
	displayGlobalTable();
	displayFuncTable();
	displaySymTable();
}

/*
在cur节点的num个节点之后(num<0则之前)插入src
*/
void insert(CODENODE *cur, int num, CODENODE *src)
{
	CODENODE *c;
	c = cur;
	CODENODE *t;
	if (num >= 0) {
		for (int i = 0; i < num; i++) {
			c = c->next;
		}
		t = c->next;
		c->next = src;
		src->next = t;
	}
	else {
		num = 0 - num;
		for (int i = 0; i < num; i++) {
			c = c->pre;
		}
		t = c->pre;
		c->pre = src;
		src->pre = t;
	}
}

/*
在node链尾部插入节点cn
*/
void insertBack(CODENODE* cn) {
	if (cnHead == NULL) {
		cnHead = cn;
		cnTail = cn;
	}
	cnTail->next = cn;
	cnTail = cn;
}

/*
生成新的常量，加入常量表，返回该常量的名称
*/
char* newLiteral(const int val)
{
	CONSTTABLEITEM *cti = &(constTable.table[constTable.size]);
	char t[32];
	char t1[32];
	itoa(constTable.size, t1, 10);
	strcpy(t, "_c");
	strcpy(cti->name, strcat(t, t1));  // 例：  name = "_c10"
	cti->val = val;
	cti->offset = constTable.size * 4;
	constTable.size++;
	return cti->name;
}

/*
生成新的中间变量，加入符号表，并返回EXPRSTRUCT指针(id+type)
*/
EXPRSTRUCT* newTemp(EXPRSTRUCT* es1, EXPRSTRUCT* es2) {
	SYMITEM si;
	char t[32];
	char t1[32];
	itoa(pSymTable->size, t1, 10);
	strcpy(t, "_t");
	strcpy(si.name, strcat(t, t1));  // 例：  name = "_t10"
	strcpy(si.type, es1->exprId);  //按照第一个操作数类型进行强制转换
	strcpy(si.cat, "var");
	si.len = 1;
	symTableAdd(&si);

	EXPRSTRUCT* res=(EXPRSTRUCT*)malloc(sizeof(EXPRSTRUCT));
	strcpy(res->exprId, si.name);
	strcpy(res->exprType, si.type);
	res->len = 0;
	res->no = 0;
	return res;
}

EXPRSTRUCT* constructExpr(const char* id) {
	EXPRSTRUCT* res = (EXPRSTRUCT*)malloc(sizeof(EXPRSTRUCT));
	strcpy(res->exprId, id);
	strcpy(res->exprType, "int");
	res->len = 0;
	res->no = 0;
	return res;
}

EXPRSTRUCT* constructExprId(const char* id) {
	SYMITEM si;
	strcpy(si.name, id);
	strcpy(si.type, "int");
	strcpy(si.cat, "var");
	si.len = 1;
	int f = symTableSearch(&si);
	if (f == 0) {
		yyerror("variable not declared");
	}
	EXPRSTRUCT* res = (EXPRSTRUCT*)malloc(sizeof(EXPRSTRUCT));
	strcpy(res->exprId, si.name);
	strcpy(res->exprType, si.type);
	res->len = 0;
	res->no = 0;
	return res;
}

/*
分配一块空间存储codenode，并返回指针
*/
CODENODE* makeNode(const char * n1,const char * n2,const char * n3,const char * n4){
	CODENODE* cn = (CODENODE*)malloc(sizeof(CODENODE));
	strcpy(cn->n1, n1);
	strcpy(cn->n2, n2);
	strcpy(cn->n3, n3);
	strcpy(cn->n4, n4);
	return cn;
}

CODENODE* makeNode2(const char * n1, const char * n2, const char * n3, const char * n4, int offset) {
	CODENODE* cn = (CODENODE*)malloc(sizeof(CODENODE));
	strcpy(cn->n1, n1);
	strcpy(cn->n2, n2);
	strcpy(cn->n3, n3);
	strcpy(cn->n4, n4);
	cn->offset = offset;
	return cn;
}

void gen1(const char *str, EXPRSTRUCT* dst, EXPRSTRUCT* src1, EXPRSTRUCT* src2) {
	CODENODE* cn;
	EXPRSTRUCT es;
	EXPRSTRUCT* pes;
	char* pl;
	if (strcmp(str, "or") == 0) {
		cn = makeNode("|", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}else if (strcmp(str, "eq") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j=", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "ne") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j!=", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "le") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j>", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "<") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j<", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "ge") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j<", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, ">") == 0) {
		pes = newTemp(&es,&es);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j>", src1->exprId, src2->exprId, "2", 2);
		insertBack(cn);
		cn = makeNode2("j","","","2", 2);
		insertBack(cn);
		pl= newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "and") == 0) {
		cn = makeNode("&", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "+") == 0) {
		cn = makeNode("+", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "-") == 0) {
		cn = makeNode("-", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "*") == 0) {
		cn = makeNode("*", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "/") == 0) {
		cn = makeNode("/", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "%") == 0) {
		cn = makeNode("%", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "&") == 0) {
		cn = makeNode("&", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "^") == 0) {
		cn = makeNode("^", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "ls") == 0) {
		cn = makeNode("<<", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "rs") == 0) {
		cn = makeNode(">>", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else if (strcmp(str, "|") == 0) {
		cn = makeNode("|", src1->exprId, src2->exprId, dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else {
		;
	}
}

void gen2(const char *str, EXPRSTRUCT* dst, EXPRSTRUCT* src) {
	CODENODE* cn;
	EXPRSTRUCT es;
	EXPRSTRUCT* pes;
	char* pl;
	if (strcmp(str, "!") == 0) {
		pes = newTemp(&es, &es);
		pl = newLiteral(0);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		cn = makeNode2("j=", src->exprId, pl, "2", 2);
		insertBack(cn);
		cn = makeNode2("j", "", "", "2", 2);
		insertBack(cn);
		pl = newLiteral(1);
		cn = makeNode("=", pl, "", pes->exprId);
		insertBack(cn);
		dst->len += 4;
	}
	else if (strcmp(str, "~") == 0) {
		cn = makeNode("~", src->exprId, "", dst->exprId);
		insertBack(cn);
		dst->len++;
	}
	else {
		;
	}
}

void gen(const char *n1, const char* n2, const char* n3, const char* n4) {
	CODENODE* cn;
	cn = makeNode(n1, n2, n3, n4);
	insertBack(cn);
}

void genIf(EXPRSTRUCT* es, const int len2)
{
	int len1 = es->len;
	CODENODE* cn;
	char* pl;
	char t[32];
	pl = newLiteral(0);
	cn = makeNode2("j!=",es->exprId,pl,"2",2);
	insert(cnTail, 0 - (len2 - 1), cn);
	itoa(len2 + 1, t, 10);
	cn = makeNode2("j", "", "", t, len2+1);
	insert(cnTail, 0 - (len2 - 1), cn);
}

void genIfElse(EXPRSTRUCT* es, const int len2, const int len3)
{
	int len1 = es->len;
	CODENODE* cn;
	char* pl;
	pl = newLiteral(0);
	char t[32];
	itoa(len2 + 2, t, 10);
	cn = makeNode2("j=", es->exprId, pl, t, len2 + 2);
	insert(cnTail, 0 - (len2 + len3), cn);
	char t1[32];
	itoa(len3 + 1, t1, 10);
	cn = makeNode2("j", "", "", t1, len3 + 1);
	insert(cnTail, 0 - (len3 - 1), cn);
}

void genWhile(EXPRSTRUCT* es, const int len2)
{
	int len1 = es->len;
	CODENODE* cn;
	char* pl;
	pl = newLiteral(0);
	char t[32];
	itoa(len2 + 2, t, 10);
	cn = makeNode2("j=", es->exprId, pl, t, len2 + 2);
	insert(cnTail, 0 - (len2 - 1), cn);
	char t1[32];
	itoa(0 - (len1 + len2 + 1), t1, 10);
	cn = makeNode2("j", "", "", t1, 0 - (len1 + len2 + 1));
	insertBack(cn);
}
