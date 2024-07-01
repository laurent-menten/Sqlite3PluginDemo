# Sqlite3 plugin for unreal engine

## Sqlite3 version
This plugin contains the Sqlite amalgamation embedded into the Sqlite3 module.

- Current Sqlite version is 3.46.0 (2024-05-23 13:25:27).
- Current sqllog version is 2012 November 26.

### How to update Sqlite3 amalgamation
To change the version of Sqlite embedded in the plugin, you need to:
1. Go to https://www.sqlite.org/download.html and download desired amalgamation.
2. Copy *sqlite3.h* into the *Sqlite3\Public\sqlite* folder and rename it to *sqlite3.h-inline*.
3. Copy *sqlite3.c* into the *Sqlite3\Private\sqlite* folder and rename it to *sqlite3.c-inline*.

### How to update the sqllog extention
4. Add or uncomment the following line in file *Sqlite3.Build.cs*
```c#
PrivateDefinitions.Add( "SQLITE_ENABLE_SQLLOG" );
```
5. Download https://www.sqlite.org/src/doc/trunk/src/test_sqllog.c
6. Copy *test_sqllog.c* into the *Sqlite3\Private\sqlite* folder and rename it to *test_sqllog.c-inline*
7. In file *test_sqllog.c-inline*:
   1. change *#include "sqlite3.h"* to *#include "sqlite/sqlite3.h-inline"*
   2. comment out the line *#include <unistd.h>*
   3. add the following line right after
```c++
#define F_OK 0
```
