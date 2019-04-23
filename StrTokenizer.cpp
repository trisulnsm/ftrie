// StrTokenizer.cpp: implementation of the StrTokenizer class.
//
//////////////////////////////////////////////////////////////////////
#include "StrTokenizer.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// expects a null term 
StrTokenizer::StrTokenizer(const char *  pszString, const char *  delims)
:m_CopySize(0),
 m_Copy(nullptr),
 m_DelimsSize(0),
 m_Delims(nullptr)
{
	m_CopySize = strlen(pszString)+1;
	m_Copy = new char[m_CopySize+4];		// to prevent a valgrind error when HPTRVL..
	memcpy(m_Copy,pszString,m_CopySize-1);
	m_Copy[m_CopySize-1]=0;
	m_Copy[m_CopySize]=0;
	m_Copy[m_CopySize+1]=0;
	m_Copy[m_CopySize+2]=0;
	m_Copy[m_CopySize+3]=0;

	m_DelimsSize = strlen(delims)+1;
	m_Delims  = new char[m_DelimsSize];
	memcpy(m_Delims,delims,m_DelimsSize-1);
	m_Delims[m_DelimsSize-1]=0;


	IntTokenize();
}

// no null term, add one extra byt and a null just for safety 
StrTokenizer::StrTokenizer(const char  * pszString, size_t  slen, const char *  delims)
:m_CopySize(0),
 m_Copy(nullptr),
 m_DelimsSize(0),
 m_Delims(nullptr)
{
	m_CopySize = slen+1;
	m_Copy = new char[m_CopySize+4];
	memcpy(m_Copy,pszString,slen);
	m_Copy[m_CopySize-1]=0;
	m_Copy[m_CopySize]=0;
	m_Copy[m_CopySize+1]=0;
	m_Copy[m_CopySize+2]=0;
	m_Copy[m_CopySize+3]=0;

	m_DelimsSize = strlen(delims)+1;
	m_Delims  = new char[m_DelimsSize];
	memcpy(m_Delims,delims,m_DelimsSize-1);
	m_Delims[m_DelimsSize-1]=0;


	IntTokenize();
}

// expects a null term 
StrTokenizer::StrTokenizer(const char *  pszString, char   delim, int dummy_use_strchr)
:m_CopySize(0),
 m_Copy(nullptr),
 m_DelimsSize(0),
 m_Delims(nullptr)
{
	m_CopySize = strlen(pszString)+1;
	m_Copy = new char[m_CopySize+4];		// to prevent a valgrind error when HPTRVL..
	memcpy(m_Copy,pszString,m_CopySize-1);
	m_Copy[m_CopySize-1]=0;
	m_Copy[m_CopySize]=0;
	m_Copy[m_CopySize+1]=0;
	m_Copy[m_CopySize+2]=0;
	m_Copy[m_CopySize+3]=0;

	m_DelimsSize = 2;
	m_Delims  = new char[m_DelimsSize];
	m_Delims[0]=delim;
	m_Delims[m_DelimsSize-1]=0;


	IntTokenize_Chr();
}

StrTokenizer::StrTokenizer(const StrTokenizer& Orig)
: m_vTokenArr(Orig.m_vTokenArr),
 m_CopySize(0),
 m_Copy(nullptr),
 m_DelimsSize(0),
 m_Delims(nullptr)
{
	m_CopySize = Orig.m_CopySize;
	m_Copy = new char[Orig.m_CopySize+4];
	memcpy(m_Copy,Orig.m_Copy,m_CopySize);
	m_Copy[m_CopySize]=0;
	m_Copy[m_CopySize+1]=0;
	m_Copy[m_CopySize+2]=0;
	m_Copy[m_CopySize+3]=0;

	m_DelimsSize = Orig.m_DelimsSize;
	m_Delims  = new char[Orig.m_DelimsSize];
	memcpy(m_Delims,Orig.m_Delims,m_DelimsSize);

}


StrTokenizer::~StrTokenizer()
{
	delete [] m_Copy;
	delete [] m_Delims;
}


bool		StrTokenizer::IntTokenize()
{
  char  * saveptr = NULL;

  char  * pbase = m_Copy;
  char  * pch = strtok_r(pbase,m_Delims,&saveptr);
  while (pch != NULL)
  {
    m_vTokenArr.push_back(pch-pbase);
    pch = strtok_r(NULL, m_Delims, &saveptr);
  }

  return true;
}

bool		StrTokenizer::IntTokenize_Chr()
{
  char  * pbase = m_Copy;
  char  * pch = strchr(pbase,m_Delims[0]);
  m_vTokenArr.push_back(0);
  while (pch != NULL)
  {
  	*pch++=0;
    m_vTokenArr.push_back(pch-pbase);
    pch = strchr(pch,m_Delims[0]);
  }

  return true;
}

size_t 			StrTokenizer::GetCount()
{
	return m_vTokenArr.size();
}

const char *  	StrTokenizer::GetToken(size_t  idx)
{
	if ( idx >= m_vTokenArr.size())  {
		return NULL;
	}

	return m_Copy + m_vTokenArr.at(idx);
}
const char *  	StrTokenizer::LastToken()
{
    return m_Copy + m_vTokenArr.at( m_vTokenArr.size()-1);
}
int  	StrTokenizer::GetTokenLen(size_t  idx)
{
	if ( idx >= m_vTokenArr.size())  {
		return 0;
	}

	if (idx==m_vTokenArr.size()-1) {
		return m_CopySize-m_vTokenArr.at(idx)-1;
	} else {
		return m_vTokenArr.at(idx+1)-m_vTokenArr.at(idx);
	}

}
void	StrTokenizer::each_token( std::function<void(const char* tok)> func)
{
	for (size_t i=0;i<GetCount();i++) {
		func( GetToken(i));
	}
}
