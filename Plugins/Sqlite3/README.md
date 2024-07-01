This plugin contains the Sqlite amalgamation that we embed into the Sqlite3 module.
Current version is 3.46.0 (2024-05-23 13:25:27)

To change the version of Sqlite that is embedded, you need to:
    1) Go to https://www.sqlite.org/download.html and download a new amalgamation
	2) Copy sqlite3.h into Sqlite3\Public\sqlite and rename it to sqlite3.h-inline
	3) Copy sqlite3.c into Sqlite3\Private\sqlite and rename it to sqlite3.c-inline

For SQLLOG support:
	4) Add or uncomment the following line in file Sqlite3.Build.cs
		PrivateDefinitions.Add("SQLITE_ENABLE_SQLLOG");
	5) Download https://www.sqlite.org/src/doc/trunk/src/test_sqllog.c
	6) Copy test_sqllog.c into Sqlite3\Private\sqlite and rename it to test_sqllog.c-inline
	7) In test_sqllog.c-inline, change #include "sqlite3.h" to #include "sqlite/sqlite3.h-inline"
	8) In test_sqllog.c-inline, comment out #include <unistd.h> and define F_OK as zero.
