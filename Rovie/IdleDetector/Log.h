#ifndef LOG_H
#define LOG_H

#include "SQLiteC++.h"
#include <iostream>
#include <time.h>

using namespace std;

SQLite::Database* initLog();
void releaseLog( SQLite::Database* historyDB );
void Log( SQLite::Database* historyDB, int eventType, int extraInfo1, int extraInfo2 );

#endif