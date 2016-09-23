// Matt Wells, copyright Jan 2007

#ifndef GB_MATCHES2_H
#define GB_MATCHES2_H

#include <inttypes.h>

// use these routines for matching any of a list of substrings in the haystack.
// the Matches array is the list of substrings to match in the "haystack". this
// should be *very* fast.
class Needle {
public:
	const char *m_string;
	char  m_stringSize;
	char  m_id;
	// if m_isSection is true, getMatch() only matches if haystack 
	// ptr is < linkPos
	char  m_isSection; 
	int32_t  m_count;
	char *m_firstMatch;
};


char *getMatches2 ( Needle *needles          , 
		    int32_t    numNeedles       ,
		    char   *haystack         , 
		    int32_t    haystackSize     ,
		    char   *linkPos          ,
		    int32_t   *n                ,
		    bool    stopAtFirstMatch ,
		    bool   *hadPreMatch      ,
		    bool    saveQuickTables  );



#endif // GB_MATCHES2_H
