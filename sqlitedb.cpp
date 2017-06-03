#include "sqlitedb.h"

/////////////////////////////////////////////////
// CSQLiteRecord
/////////////////////////////////////////////////

//*****************************************************************************
CSQLiteRecord::CSQLiteRecord(void)
	: m_pStatement(nullptr),
	m_bDone(false)
{
}

//*****************************************************************************
CSQLiteRecord::~CSQLiteRecord(void)
{
	Close();
}

//*****************************************************************************
void CSQLiteRecord::Close(void)
{
	if(m_pStatement)
	{
		sqlite3_finalize(m_pStatement);
		m_pStatement = nullptr;
	}
	m_bDone = false;
}

//*****************************************************************************
bool CSQLiteRecord::MoveNext(void)
{
	bool bSuccess = false;

	if(m_pStatement)
	{
		if(sqlite3_step(m_pStatement) == SQLITE_ROW)
			bSuccess = true;
		else
			m_bDone = true;
	}

	return bSuccess;
}


/////////////////////////////////////////////////
// CSQLiteDB
/////////////////////////////////////////////////

//*****************************************************************************
CSQLiteDB::CSQLiteDB(void)
	: m_pDB(nullptr)
{
}

//*****************************************************************************
CSQLiteDB::~CSQLiteDB(void)
{
	Close();
}

//*****************************************************************************
bool CSQLiteDB::Open(PCSTR szFilename)
{
	Close();

	return (sqlite3_open(szFilename, &m_pDB) == SQLITE_OK);
}

//*****************************************************************************
void CSQLiteDB::Close(void)
{
	if(m_pDB)
	{
		sqlite3_close(m_pDB);
		m_pDB = nullptr;
	}
}

//*****************************************************************************
bool CSQLiteDB::ExecuteQuery(PCSTR szQuery) const
{
	bool bSuccess = false;

	sqlite3_stmt *pStatement;
	if(m_pDB && sqlite3_prepare_v2(m_pDB, szQuery, -1, &pStatement, 0) == SQLITE_OK)
	{
		int nReturn = sqlite3_step(pStatement);
		if(nReturn == SQLITE_ROW || nReturn == SQLITE_DONE || nReturn == SQLITE_OK)
			bSuccess = true;

		sqlite3_finalize(pStatement);
	}

	return bSuccess;
}

//*****************************************************************************
bool CSQLiteDB::OpenQuery(PCSTR szQuery, CSQLiteRecord &record) const
{
	bool bSuccess = false;

	record.Close();
	if(m_pDB && sqlite3_prepare_v2(m_pDB, szQuery, -1, &record.m_pStatement, 0) == SQLITE_OK)
	{
		record.MoveNext();
		bSuccess = true;
	}

	return bSuccess;
}
