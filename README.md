# CodexDB

**CodexDB** is a lightweight, CLI-based custom-built Database Management System developed in **C++** from the ground up. It features a fully custom lexer, an AST-based parser, and a query execution engine, providing persistent storage and support for SQL-like query execution.

## Overview


<img width="1016" height="659" alt="Image" src="https://github.com/user-attachments/assets/2e6fec2a-1aa9-494f-a4ee-a43bf98e6f86" />


<img width="932" height="608" alt="Image" src="https://github.com/user-attachments/assets/f93cf7f4-231e-4047-a244-9e5cb5775739" />


CodexDB replicates the core functionality of a traditional database engine with a strong emphasis on low-level control and understanding of database internals.
The system supports core SQL operations, including:

* **CREATE TABLE** – Define structured tables with typed columns.
* **INSERT INTO** – Add records to tables with data validation.
* **SELECT** – Retrieve and filter records with `WHERE` clause expression evaluation.
* and more ...

## Key Features

* **Custom Tokenizer (Lexer)** – Built using a Finite State Machine for flexible and efficient query parsing.
* **Abstract Syntax Tree (AST)** – Provides structured interpretation and validation of SQL-like commands.
* **Execution Engine** – Executes parsed queries with type checking and condition-based retrieval.
* **Persistent Storage** – Ensures data remains accessible across sessions.
* **Modular Architecture** – Clear separation of lexing, parsing, and execution phases for maintainability.

## Technologies Used

* **Language:** C++17
* **Paradigm:** Object-Oriented Programming
* **Storage:** File-based persistence
* **Parsing:** Finite State Machine + AST-based design

## Usage

```bash
# Clone the repository
git clone https://github.com/Aarnya-Jain/CodexDB.git

# Navigate to the project directory
cd CodexDB

# Build the project
g++ -std=c++17 -o codex main.cpp

# Run CodexDB CLI
./codex
```

Once running, you can enter SQL-like commands directly in the prompt:

```sql
CREATE TABLE students (id INT, name TEXT);
INSERT INTO students VALUES (1, "Aarya");
SELECT * FROM students WHERE id = 1;
```