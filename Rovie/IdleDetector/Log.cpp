#include "Log.h"

SQLite::Database* initLog()
{
	SQLite::Database* historyDB;

	try
	{
		// Open a database file
		historyDB = new SQLite::Database("C:\\OSHistory.s3db", SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
	
		if( historyDB->tableExists("history") == false )
		{
			historyDB->exec("CREATE TABLE history (id INTEGER PRIMARY KEY, event INTEGER, time INTEGER, extraInfo1 INTEGER, extraInfo2 INTEGER)");
		}
	}
	catch (std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}

	return historyDB;
}

void releaseLog(SQLite::Database* historyDB)
{
	delete historyDB;
}

void Log( SQLite::Database* historyDB, int eventType, int extraInfo1, int extraInfo2 )
{
	try
	{
		if( historyDB->tableExists("history") == false )
		{
			historyDB->exec("CREATE TABLE history (id INTEGER PRIMARY KEY, event INTEGER, time INTEGER, extraInfo INTEGER)");
		}

		SQLite::Statement   query(*historyDB, "INSERT INTO history VALUES (NULL, ?,?,?,?)");
		query.bind(1, eventType);
		query.bind(2, (long)time(NULL));
		query.bind(3, extraInfo1);
		query.bind(4, extraInfo2);
		query.exec();
	}
	catch (std::exception& e)
	{
		std::cout << "exception: " << e.what() << std::endl;
	}
}