#pragma once

#include "sqlite3.h"
#include <unistd.h>

typedef const char *PCSTR;
typedef char *PSTR;

// Forward declarations
class CSQLiteDB;

//*****************************************************************************
class CSQLiteRecord
{
protected:
	friend class CSQLiteDB;

public:
	CSQLiteRecord(void);
	virtual ~CSQLiteRecord(void);

	void Close(void);
	bool MoveNext(void);
	bool IsDone(void) const {return m_bDone;}

	template <typename INTTYPE> bool GetInteger(const int nColumn, INTTYPE &nValue) const;
	template <typename FLOATTYPE> bool GetFloat(const int nColumn, FLOATTYPE &fValue) const;
	template <typename STRTYPE> bool GetString(const int nColumn, STRTYPE &strValue) const;

protected:
	sqlite3_stmt *m_pStatement;
	bool m_bDone;
};


//*****************************************************************************
class CSQLiteDB
{
public:
	CSQLiteDB(void);
	virtual ~CSQLiteDB(void);

	bool Open(PCSTR szFilename);
	void Close(void);

	bool IsOpen(void) const {return (m_pDB != NULL);}

	sqlite3 *GetDB(void) {return m_pDB;}

	bool ExecuteQuery(PCSTR szQuery) const;
	bool OpenQuery(PCSTR szQuery, CSQLiteRecord &record) const;

protected:
	sqlite3 *m_pDB;
};


//*****************************************************************************
template <typename INTTYPE>
bool CSQLiteRecord::GetInteger(const int nColumn, INTTYPE &nValue) const
{
	bool bSuccess = false;

	if(m_pStatement && !m_bDone)
	{
		if(sizeof(INTTYPE) > 4)
			nValue = (INTTYPE)sqlite3_column_int64(m_pStatement, nColumn);
		else
			nValue = (INTTYPE)sqlite3_column_int(m_pStatement, nColumn);

		bSuccess = true;
	}

	return bSuccess;
}

//*****************************************************************************
template <typename FLOATTYPE>
bool CSQLiteRecord::GetFloat(const int nColumn, FLOATTYPE &fValue) const
{
	bool bSuccess = false;

	fValue = 0.0;

	if(m_pStatement && !m_bDone)
	{
		fValue = (FLOATTYPE)sqlite3_column_double(m_pStatement, nColumn);
		bSuccess = true;
	}

	return bSuccess;
}

//*****************************************************************************
template <typename STRTYPE>
bool CSQLiteRecord::GetString(const int nColumn, STRTYPE &strValue) const
{
	bool bSuccess = false;

	strValue.Empty();

	if(m_pStatement && !m_bDone)
	{
		strValue = (PCSTR)sqlite3_column_text(m_pStatement, nColumn);
		bSuccess = true;
	}

	return bSuccess;
}
