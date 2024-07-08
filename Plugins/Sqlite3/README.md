# Sqlite3 plugin for unreal engine

This plugins is designed to expose the Sqlite3 API to blueprint and c++. It aims to provide an
experience as seamless as possible to programmers with working knowledge of Sqlite.

There are three separate modules:
- Sqlite3: contains runtime code for accessing databases.
- Sqlite3Tools: contains the custom blueprint nodes.
- Sqlite3Editor: contains the editor and data asset validation code.

The Sqlite3 module provide a game instance subsystem that manage the open databases.
It also provides the SqliteDatabase and SqliteStatement classes.
Database settings are stored in a custom DataAsset.

## Sqlite3 version
This plugin contains Sqlite code embedded into the Sqlite3 module.

- Current Sqlite version is 3.46.0 (2024-05-23 13:25:27).
- Current sqllog extention version is 2012 November 26.

### How to update Sqlite3 amalgamation
To change the version of Sqlite embedded in the plugin, you need to:
1. Go to https://www.sqlite.org/download.html and download desired amalgamation.
2. Copy *sqlite3.h* into the *Sqlite3\Public\sqlite* folder and rename it to *sqlite3.h-inline*.
3. Copy *sqlite3.c* into the *Sqlite3\Private\sqlite* folder and rename it to *sqlite3.c-inline*.

### How to enable/update the sqllog extention
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
