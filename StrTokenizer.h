// StrTokenizer.h: interface for the StrTokenizer class.
//
//////////////////////////////////////////////////////////////////////
#pragma once 

#include <vector>
using namespace std;
#include <functional>

class StrTokenizer  
{
public:
	StrTokenizer(const char * pszString, const char * delims);	// ascii 
	StrTokenizer(const char * pszString, size_t  slen, const char *  delims ); 
	StrTokenizer(const StrTokenizer& Orig);
	StrTokenizer(const char * pszString, char delim, int handle_consecutive_delim ); 
	virtual ~StrTokenizer();

public:
	const char *  	GetToken(size_t   idx);
	const char *  	LastToken();
	int      		GetTokenLen(size_t   idx);
	size_t			GetCount();
	void			each_token( std::function<void(const char* tok)> func);

private:
	bool			IntTokenize();	    // use strtok
	bool			IntTokenize_Chr();  // new version using strchr(..) 

private:
	vector<int>	    m_vTokenArr;		// string array of tokens

	size_t			m_CopySize;			// includes null term
	char *	 		m_Copy;				// copy

	size_t			m_DelimsSize;		// includes null term
	char * 			m_Delims;			// copy

};

