# CodexDB Syntax Reference

This document provides a reference for the SQL-like syntax supported by CodexDB.

## Database Operations

### Open Database
Switch to a specific database.
```sql
OPEN <database_name>;
```
*Example:* `OPEN TEMP;`

### Create Database
Create a new database.
```sql
CREATE DATABASE <database_name>;
```
*Example:* `CREATE DATABASE MYDB;`

### Drop Database
Delete an existing database.
```sql
DROP DATABASE <database_name>;
```
*Example:* `DROP DATABASE MYDB;`

## Table Operations

### Create Table
Create a new table with specified columns.
```sql
CREATE TABLE <table_name> ( <col1>, <col2>, ... );
```
*Example:* `CREATE TABLE USERS ( id, name, age, salary );`

### Drop Table
Delete an existing table.
```sql
DROP TABLE <table_name>;
```
*Example:* `DROP TABLE USERS;`

### Add Column
Add a new column with values for existing rows.
```sql
ADD COLUMN ( <val1>, <val2>, ... ) INTO <table_name>;
```
*Example:* `ADD COLUMN ( r, r, y ) INTO USERS;`

### Delete Column
Remove a column from a table.
```sql
DELETE COLUMN <column_name> FROM <table_name>;
```
*Example:* `DELETE COLUMN house FROM USERS;`

## Data Manipulation

### Insert Row
Insert a new row into a table.
```sql
INSERT INTO <table_name> VALUES ( <val1>, <val2>, ... );
```
*Example:* `INSERT INTO USERS VALUES ( 1, Aarnya, 18, 500000 );`

### Update Data
Update values in a table based on a condition.
```sql
UPDATE <table_name> SET <column> = <value> WHERE <condition_col> <op> <condition_val>;
```
*Example:* `UPDATE USERS SET salary = 600000 WHERE name = Aarnya;`

### Delete Rows
Delete rows from a table based on a condition.
```sql
DELETE ROWS FROM <table_name> WHERE <condition_col> <op> <condition_val>;
```
*Example:* `DELETE ROWS FROM USERS WHERE id >= 7;`

## Querying Data

### Select All
Retrieve all columns from a table.
```sql
SELECT * FROM <table_name>;
```
*Example:* `SELECT * FROM USERS;`

### Select Specific Columns
Retrieve specific columns from a table.
```sql
SELECT <col1>, <col2> FROM <table_name>;
```
*Example:* `SELECT id, name FROM USERS;`

### Filtering (WHERE)
Filter rows based on a condition. Supported operators: `=`, `>`, `<`, `>=`, `<=`, `!=`.
```sql
SELECT * FROM <table_name> WHERE <column> <op> <value>;
```
*Example:* `SELECT * FROM USERS WHERE age > 18;`

### Sorting
Sort the result set by a specific column.
```sql
SELECT * FROM <table_name> SORT BY <column> <ASC|DESC>;
```
*Example:* `SELECT * FROM USERS SORT BY age ASC;`
