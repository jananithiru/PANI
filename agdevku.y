	/* symbolic tokens */
%{
#include "heading.h"
extern int yyerror(char *s);
extern int yylex(void);
//All variable declarations


%}

%union {
	int intval;
	double floatval;
	char strval[512];
	char dummyCopy[512];
	int subtok;
}

	
%token NAME
%token STRING
%token INTNUM APPROXNUM

	/* operators */

%left OR
%left AND
%left NOT
%left <strval> COMPARISON /* = <> < > <= >= */
%left '+' '-'
%left '*' '/'
%left '='
%nonassoc UMINUS

	/* literal keyword tokens */

%token ALL AMMSC AND ANY AS ASC AUTHORIZATION BETWEEN BY
%token CHARACTER CHECK CLOSE COMMIT CONTINUE CREATE CURRENT
%token CURSOR DATABASE DECIMAL DECLARE DEFAULT DELETE DESC DESCRIBE DISTINCT DOUBLE DROP
%token ESCAPE EXISTS FETCH FLOAT FOR FOREIGN FOUND FROM GOTO
%token GRANT GROUP HAVING IN INDEX INDICATOR INSERT INTEGER INTO
%token IS KEY LANGUAGE LIKE MODULE NOT NULLX NUMERIC OF ON OR
%token OPEN OPTION ORDER PRECISION PRIMARY PRIVILEGES PROCEDURE
%token PUBLIC REAL REFERENCES ROLLBACK SCHEMA SELECT SET SHOW
%token SMALLINT SOME SQLCODE SQLERROR TABLE TABLES TO UNION
%token UNIQUE UPDATE USER VALUES VARCHAR VIEW WHENEVER WHERE WITH WORK
%token COBOL FORTRAN PASCAL PLI C ADA DATABASES USE

%type<strval> column
%type<strval> data_type
%type<strval> TABLE 
%type<strval> column_def_opt
%type<strval> table
%type<strval> CREATE
%type<strval> NAME
%type<strval> INTEGER 
%type<strval> CHARACTER
%type<strval> FLOAT
%type<strval> REAL
%type<strval> INTNUM
%type<strval> index_name
%type<strval> db_name
%type<strval> OR
%type<strval> AND
%type<strval> NOT
%type<strval> STRING
%type<strval> VARCHAR
%type<strval> NULLX
%type<strval> PRIMARY
%type<strval> KEY
%type<strval> column_ref


%%
/*********************************RULES SECTION************************************/
query:
	query_list ';'
	;
	
query_list:
		base_table_def 
	|	insert_statement
	|	delete_statement_searched
	|	update_statement_searched
	|	select_query_spec
	|	drop_query
	|	describe_table
	|	show_tables
	|	show_databases
	|   use_database
	;

base_table_def:
		CREATE TABLE table '(' base_table_element_commalist ')' /*Rule for creating table*/
		{
			//DEBUG("$3= %s\n",$3);
			executeStructures->schema.tableName = $3;
			executeStructures->executeStatement = ExecuteStructures::CREATE_TABLE_STATEMENT;
			DEBUG("executeStructures->schema="<<executeStructures->schema.toString());
		}
		
	|	CREATE DATABASE db_name						/*Rule for creating database*/
		
		{
			executeStructures->databaseName = Util::char_to_string($3);
			executeStructures->executeStatement = ExecuteStructures::CREATE_DATABASE_STATEMENT;
			DEBUG("executeStructures->databaseName="<<$3<<","<<executeStructures->databaseName);
				
		}
		
	|	CREATE INDEX index_name ON table '(' column_commalist ')'      /*Rule for creating index*/
		
		{
			executeStructures->executeStatement = ExecuteStructures::CREATE_INDEX_STATEMENT;
			executeStructures->indexName = Util::char_to_string($3);
			executeStructures->tableName = Util::char_to_string($5);
			DEBUG("$3=%s\n$5=%s"<<$3<<","<<$5);
				
		}
		
	;
	;

base_table_element_commalist:
		base_table_element
	|	base_table_element_commalist ',' base_table_element
	;

base_table_element:
		column_def
	|	table_constraint_def
	;

column_def:
		column data_type column_def_opt_list{
		}
	;

column_def_opt_list:
		/* empty */
	|	column_def_opt_list column_def_opt
	;

column_def_opt:
		NOT NULLX   { DEBUG("$1= %s \n$2= %s\n"<<$1<<","<<$2);}
	|	NOT NULLX UNIQUE
	|	NOT NULLX PRIMARY KEY	{ DEBUG("$1= %s\n $2= %s\n $3= %s\n $4= %s\n"<<$1<<","<<$2<<","<<$3<<","<<$4);}
	|   	PRIMARY KEY		{ DEBUG("$1= %s \n$2= %s\n"<<","<<$1<<","<<$2);}
	|	DEFAULT literal
	|	DEFAULT NULLX
	|	DEFAULT USER
	//|	CHECK '(' search_condition ')'
	|	REFERENCES table
	|	REFERENCES table '(' column_commalist ')'
	;

table_constraint_def:
		UNIQUE '(' column_commalist ')'
	|	PRIMARY KEY '(' column_commalist ')'
		{
			DEBUG("$1= %s\n $2= %s\n"<<","<<$1<<","<<$2);
		}
	
	|	FOREIGN KEY '(' column_commalist ')'
			REFERENCES table 
	|	FOREIGN KEY '(' column_commalist ')'
			REFERENCES table '(' column_commalist ')'
	//|	CHECK '(' search_condition ')'
	;

column_commalist:
		column
	|	column_commalist ',' column
	;

table:
		NAME { DEBUG("table=%s\n"<<","<<$1);
		executeStructures->afterFromKeyWord = true;
		};
	|	NAME '.' NAME
	;
	
db_name:	NAME
	
	;
	
index_name:	NAME

	;
	
column:		NAME  {
			DEBUG("column= %s\n"<<","<<$1);
			executeStructures->schema.numOfColumns++;
			executeStructures->schema.columnNames.push_back(Util::char_to_string($1));
			executeStructures->updateQueryColumnNames.push_back(Util::char_to_string($1));
			executeStructures->indexColumnNames.push_back(Util::char_to_string($1));
		  }

	;
	
literal:STRING  {		
				DEBUG("entering into STRING\n");																			/*Have changed from String to Name*/
			  	DEBUG("literal:NAME,$1= %s\n"<<","<<$1);
				executeStructures->insertValues.push_back(Util::char_to_string($1));
				executeStructures->updateQueryColumnValues.push_back(Util::char_to_string($1));
				ExprInfo exprInfo(ExprInfo::LITERAL_TYPE,Util::char_to_string($1));
				  executeStructures->whereConditionExprList.push_back(exprInfo);
			   }
	|	INTNUM	{
				  DEBUG("literal:INTNUM,$1= %s\n"<<","<<$1);
				  //entering only for insert stuff ah??
				  executeStructures->insertValues.push_back(Util::char_to_string($1));
				  executeStructures->updateQueryColumnValues.push_back(Util::char_to_string($1));
				  ExprInfo exprInfo(ExprInfo::LITERAL_TYPE,Util::char_to_string($1));
				  executeStructures->whereConditionExprList.push_back(exprInfo);
				  }
	|	APPROXNUM
	;

data_type:
		CHARACTER  {
					executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
					}
	|	CHARACTER '(' INTNUM ')' {
									executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
									}
	|	VARCHAR '(' INTNUM ')' {
								  DEBUG("varchar(intnum)=%s\n"<<","<<$3);
								 executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
								 executeStructures->schema.fieldLengths.push_back(Util::string_to_int($3));
								}
	|	NUMERIC
	|	NUMERIC '(' INTNUM ')'
	|	NUMERIC '(' INTNUM ',' INTNUM ')'
	|	DECIMAL
	|	DECIMAL '(' INTNUM ')'
	|	DECIMAL '(' INTNUM ',' INTNUM ')'
	|	INTEGER   {
					executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
					executeStructures->schema.fieldLengths.push_back(FieldLengthLookup::getType($1));
				  }
	|	SMALLINT
	|	FLOAT    {
					executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
					executeStructures->schema.fieldLengths.push_back(FieldLengthLookup::getType($1));
				 }
	|	FLOAT '(' INTNUM ')'	{
									executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
								}
	|	REAL	{
					executeStructures->schema.fieldTypes.push_back(DataTypeLookup::getType($1));
				}
	//|	//DOUBLE PRECISION
	      
	;
	
insert_statement:					/*Insert query*/
		INSERT INTO table opt_column_commalist values_or_query_spec
		{
			DEBUG("$3=%s\n"<<","<<$3);
			executeStructures->executeStatement = ExecuteStructures::INSERT_STATEMENT;
			executeStructures->tableName = Util::char_to_string($3);
		}
	;

opt_column_commalist:
							/*Empty*/
	|	'(' column_commalist ')'
	;
	
values_or_query_spec:
		VALUES para_insert_atom_commalist
	//|	query_spec
	;

para_insert_atom_commalist:	
		'(' insert_atom_commalist ')'
	|	para_insert_atom_commalist ',' '(' insert_atom_commalist ')'
		;
			
insert_atom_commalist:
		insert_atom
	|	insert_atom_commalist ',' insert_atom
	;
	
insert_atom:
		atom
	|	NULLX
	;
	
atom:
		parameter_ref 
	|	literal
	;
	
parameter_ref:
		parameter 
	|	parameter parameter
	//|	parameter INDICATOR parameter
	;
	
parameter:
							/*Empty*/
	|	':' NAME 
	;
	
delete_statement_searched:				/*Delete query*/
		DELETE FROM table opt_where_clause	
		{
			executeStructures->executeStatement = ExecuteStructures::DELETE_STATEMENT;
			DEBUG("$3= %s\n"<<","<<$3);
			executeStructures->tableName=Util::char_to_string($3);
		}
	;
	
opt_where_clause:
							/*Empty*/
	|	where_clause
	;
	
where_clause:
		WHERE search_condition { DEBUG("where_clause");}
	;
	
search_condition:
		search_condition OR search_condition
		{
			DEBUG("$2= %s\n"<<","<<$2);
			ExprInfo expr(ExprInfo::OPERATOR_TYPE,"OR");
			executeStructures->whereConditionExprList.push_back(expr);
		}
	|	search_condition AND search_condition
		{
			DEBUG("$2= %s\n"<<","<<$2);
			ExprInfo expr(ExprInfo::OPERATOR_TYPE,"AND");
			executeStructures->whereConditionExprList.push_back(expr);
		}
	|	NOT search_condition
		{
			DEBUG("$1= %s\n"<<","<<$1);
			ExprInfo expr(ExprInfo::OPERATOR_TYPE,"NOT");
			executeStructures->whereConditionExprList.push_back(expr);
		}
	|	'(' search_condition ')'
	|	predicate
	;
	
predicate:
		comparison_predicate
	;
	
comparison_predicate:
		scalar_exp COMPARISON scalar_exp
		{
			DEBUG("$2= %s\n"<<","<<$2);
			ExprInfo expr(ExprInfo::OPERATOR_TYPE,Util::char_to_string($2));
			executeStructures->whereConditionExprList.push_back(expr);
		}
	//	scalar_exp COMPARISON subquery
	;
	
scalar_exp:
		scalar_exp '+' scalar_exp
		{
			DEBUG("$2= %s\n"<<","<<'+');
		}
	|	scalar_exp '-' scalar_exp
	|	scalar_exp '*' scalar_exp
	|	scalar_exp '/' scalar_exp
	|	'+' scalar_exp %prec UMINUS
	|	'-' scalar_exp %prec UMINUS
	|	atom
	|	column_ref 
	;
	

	
column_ref:
		NAME { 
		if(executeStructures->afterFromKeyWord == true){
		DEBUG("after,column_ref=%s\n"<<","<<$1);
		ExprInfo expr(ExprInfo::IDENTIFIER_TYPE,Util::char_to_string($1));
		executeStructures->whereConditionExprList.push_back(expr);
		}else{
		DEBUG("before,column_ref=%s\n"<<","<<$1);
		executeStructures->resultSetColumnList.push_back(Util::char_to_string($1));
		}
		}
	|	NAME '.' NAME
	|	NAME '.' NAME '.' NAME
	;
	
update_statement_searched:											/*Update Query*/
		UPDATE table SET assignment_commalist opt_where_clause
		{
			executeStructures->executeStatement = ExecuteStructures::UPDATE_STATEMENT;
			executeStructures->tableName = Util::char_to_string($2);
			DEBUG("update table started \n$2= %s\n"<<","<<$2);
		}
	;

assignment_commalist:
		assignment {DEBUG("returned from assignment_commalist_1");}
	|	assignment_commalist ',' assignment {DEBUG("returned from assignment_commalist_2");}
	;
	
assignment:
		column COMPARISON scalar_exp								/*Repalced '=' with COMPARISON, temporal arrangement*/
	|	column COMPARISON NULLX
	;

select_query_spec:													/*Select Query*/
		SELECT opt_all_distinct selection FROM table table_exp
		{
			executeStructures->executeStatement = ExecuteStructures::SELECT_STATEMENT;
			DEBUG("$5= %s\n"<<","<<$5);
			executeStructures->tableName=Util::char_to_string($5);
		}
	;
	
opt_all_distinct:
							/*Empty*/
	|	ALL
	|	DISTINCT
	;
	
selection:
		scalar_exp_commalist {executeStructures->afterFromKeyWord = true; 
				DEBUG("returing FROM keyword");
				}
	|	'*'
		{
			executeStructures->afterFromKeyWord = true;
			executeStructures->allColumns = true;
			DEBUG("*\n");
		}
	;

scalar_exp_commalist:
		scalar_exp
	|	scalar_exp_commalist ',' scalar_exp
	;

table_exp:
		//from_clause
	|	opt_where_clause
	|	opt_group_by_clause
	|	opt_having_clause
	;
	
//from_clause:
//		FROM table_ref_commalist
//	;
	
table_ref_commalist:
		table_ref
	|	table_ref_commalist ',' table_ref
	;
	
table_ref:
		table
	|	table range_variable
	;
	
range_variable:
		NAME
	;
	
opt_group_by_clause:
							/*Empty*/
	|	GROUP BY column_ref_commalist
	;
	
column_ref_commalist:
		column_ref
	|	column_ref_commalist ',' column_ref
	;
	
opt_having_clause:
							/*Empty*/
	|	HAVING search_condition
	;
drop_query:																	/*Drop table or database*/
		DROP db_struct
		
		;
		
db_struct:
		DATABASE db_name
		{
			executeStructures->executeStatement = ExecuteStructures::DROP_DATABASE_STATEMENT;
			executeStructures->databaseName = Util::char_to_string($2); 
			DEBUG("$2= %s\n"<<","<<$2);
		}
	|	TABLE table
		{
			executeStructures->executeStatement = ExecuteStructures::DROP_TABLE_STATEMENT;
			executeStructures->tableName = Util::char_to_string($2);
			DEBUG("$2= %s\n"<<","<<$2);
		}
	;	
describe_table:															/*Describe table*/
		DESC table
		{
			executeStructures->executeStatement = ExecuteStructures::DESCRIBE_STATEMENT;
			executeStructures->tableName = Util::char_to_string($2);
			DEBUG("$2= %s\n"<<","<<$2);
		}
	|	DESCRIBE table
		{
			executeStructures->executeStatement = ExecuteStructures::DESCRIBE_STATEMENT;
			executeStructures->tableName = Util::char_to_string($2);
			DEBUG("$2= %s\n"<<","<<$2);
		}
	;
	
show_tables:															/*Show tables*/
		SHOW TABLES
		{
			executeStructures->executeStatement = ExecuteStructures::SHOW_TABLES_STATEMENT;
			DEBUG("This should print the names of all tables in the current database\n");
		}
		;	
		
show_databases:															/*Show tables*/
		SHOW DATABASES
		{
			executeStructures->executeStatement = ExecuteStructures::SHOW_DATABASES_STATEMENT; 
			//DEBUG("This should print the names of all tables in the current database\n");
		}
		;
use_database:
		USE db_name
		{
			executeStructures->executeStatement	= ExecuteStructures::OPEN_DATABASE_STATEMENT;
			executeStructures->databaseName = Util::char_to_string($2);
		}		
%%
