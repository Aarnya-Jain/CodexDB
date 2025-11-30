# CodexDB

**CodexDB** is a lightweight, CLI-based custom-built **in memory** Database Management System developed in **C++** from the ground up. It features a fully custom lexer, an AST-based parser, and a query execution engine, providing persistent storage and support for SQL-like query execution.

## Overview

<center>

<img width="1016" height="659" alt="Image" src="https://github.com/user-attachments/assets/2e6fec2a-1aa9-494f-a4ee-a43bf98e6f86" />


<img width="932" height="608" alt="Image" src="https://github.com/user-attachments/assets/f93cf7f4-231e-4047-a244-9e5cb5775739" />

</center>


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

## Why Red-Black Tree?

We chose Red-Black Trees for in-memory data storage after benchmarking against AVL Trees and B+ Trees. For 100,000 rows, Red-Black Trees demonstrated the best performance.

| Data Structure | Time (Seconds) |
| :--- | :--- |
| **Red-Black Tree** | **3.383** |
| AVL Tree | 4.148 |
| B+ Tree | 4.860 |

## Technologies Used

* **Language:** C++17
* **Paradigm:** Object-Oriented Programming
* **Storage:** File-based persistence
* **Parsing:** Finite State Machine + AST-based design

## Usage

- Linux
```bash
# Clone the repository
git clone https://github.com/Aarnya-Jain/CodexDB.git

# Navigate to the project directory
cd CodexDB

# Build the project
make

# Run CodexDB CLI
./codex
```
- Windows
```bash
# Clone the repository
git clone https://github.com/Aarnya-Jain/CodexDB.git

# Navigate to the project directory
cd CodexDB

# Build the project
mingw32-make

# Run CodexDB CLI
./codex.exe
```

Once running, you can enter SQL-like commands directly in the prompt:


```sql
CREATE TABLE students VALUES (id , name );
INSERT INTO students VALUES (1, "John Doe");
SELECT * FROM students WHERE id = 1;
```

- Kindly refer to [Syntax Reference](syntaxreference.md) for more information regarding the supported queries.
