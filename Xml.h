#ifndef _XML_H_
#define _XML_H_

// . this is used for parsing tagdb records
// . used for pasrsing tagdb records, conf file, and html/xml documents
// . NOTE: ALL tags are case INsensitive so <myTag> equals <MYTAG>

#include "XmlNode.h"
#include "Lang.h"

class Xml {
public:
	Xml();

	// . should free m_xml if m_copy is true
	~Xml();

	// do we have any xml in here?
	bool isEmpty() {
		return ( m_xml == NULL );
	}

	// . set this xml class from a string
	// . should be called before calling anything else
	// . if "copy" makes a copy of the string "s" and references into that
	// . s must be NULL terminated
	// . if it's pure xml then set pureXml to true otherwise we assume it
	//   is html or xhtml
	bool set( char *s, int32_t slen, bool ownData, int32_t allocSize, bool pureXml, int32_t version,
			  int32_t niceness, char contentType );

	void  reset ( );

	char *getContent() {
		return m_xml;
	}

	char *getContentEnd() {
		return m_xml + m_xmlLen;
	}

	int32_t getContentLen() {
		return m_xmlLen;
	}

	int32_t getNumNodes() {
		return m_numNodes;
	}

	int32_t getVersion() {
		return m_version;
	}

	// . tagName is compound for xml tags, simple for html tags
	// . xml compound tag name example = "myhouse.bedroom.nightstand"
	// . html simple  tag name example = "title" or "table"
	// . obsolete compound name = myhouse[0].bedroom[2].nightstand[1]
	// . returns -1 if not found
	// . only searches nodes in [n0,n1] node range
	int32_t getNodeNum( int32_t n0, int32_t n1, char *tagName, int32_t tagNameLen ) const;

	// some wrappers for it
	int32_t getNodeNum( char *tagName ) {
		if ( !tagName ) {
			char *xx = NULL; *xx = 0;
		}
		return getNodeNum( 0, m_numNodes, tagName, strlen( tagName ) );
	}

	int32_t getNodeNum( char *tagName, int32_t tagNameLen ) {
		return getNodeNum( 0, m_numNodes, tagName, tagNameLen );
	}

	// . get the back tag node for a given node
	int32_t getEndNode ( int32_t num );

	bool isTag( int32_t n ) {
		return m_nodes[n].isTag();
	}

	bool isBreakingTag( int32_t n ) {
		return m_nodes[n].m_isBreaking;
	}

	bool isBackTag( int32_t n ) {
		return m_nodes[n].m_node[1] == '/';
	}

	bool isXmlTag( int32_t n ) {
		return m_nodes[n].m_nodeId == TAG_XMLTAG;
	}

	char *getNode( int32_t n ) {
		return m_nodes[n].m_node;
	}

	int32_t getNodeLen( int32_t n ) {
		return m_nodes[n].m_nodeLen;
	}

	nodeid_t getNodeId( int32_t n ) {
		return m_nodes[n].m_nodeId;
	}

	int64_t getNodeHash( int32_t n ) {
		return m_nodes[n].m_hash;
	}

	bool isVisible( int32_t n ) {
		return m_nodes[n].m_isVisible;
	}

	// get all nodes!
	XmlNode *getNodes() {
		return m_nodes;
	}

	XmlNode *getNodePtr( int32_t n ) {
		return &m_nodes[n];
	}

	// get like compound name like "node1.node2.node3\0"
	bool getCompoundName( int32_t node, class SafeBuf *sb );

	// . used for parsing xml conf files
	// . used for getting the title in an html doc, etc.
	// . gets the value of the text field immediately following the tag
	// . "tagName" is always compound
	// . only searches nodes in [n0,n1] node range

	int32_t getLong( int32_t n0, int32_t n1, char *tagName, int32_t defaultLong = 0 );

	char *getString( int32_t n0, int32_t n1, char *tagName, int32_t *len,
					 bool skipLeadingSpaces = true ) const;

	// for parsing facebook replies:
	char *getNode( char *tagName, int32_t *len );

	// like above routines but we search all nodes
	int32_t getLong( char *tagName, int32_t defaultLong = 0 ) {
		return getLong( 0, m_numNodes, tagName, defaultLong );
	}

	char *getString( char *tagName, int32_t *len, bool skipLeadingSpaces = true ) const {
		return getString( 0, m_numNodes, tagName, len, skipLeadingSpaces );
	}

	// . used for getting links in the <a href=...> tag
	// . used for getting data from meta tags
	char *getString( int32_t node, const char *field, int32_t *valueLen ) {
		if ( node >= m_numNodes ) {
			char *xx = NULL; *xx = 0;
		}

		return m_nodes[node].getFieldValue( field, valueLen );
	}

	// called by getTextForXmlTag() below
	char *getString( int32_t node, bool skipLeadingSpaces, int32_t *len ) const;

	// used for title/summary generation
	bool getTagContent( const char *fieldName, const char *fieldContent, char *buf, int32_t bufLen,
						int32_t minLength, int32_t maxLength, int32_t *contentLenPtr,
						bool ignoreExpandedIframe = false, nodeid_t expectedNodeId = LAST_TAG );

	// . like getText() below but gets the content from a meta tag
	// . stores it in "buf"  and NULL terminates it
	// . returns the length
	// . field can be stuff like "summary","description","keywords",...
	// . use "http-equiv" for "name" for meta redirect tags
	// . if "convertHtmlEntites" is true we change < to &lt; and > to &gt;
	int32_t getMetaContent( char *buf, int32_t bufLen, char *field, int32_t fieldLen, char *name = "name",
							int32_t startNode = 0, int32_t *matchedNode = NULL );

	// just get a pointer to it
	char *getMetaContentPointer( char *field, int32_t fieldLen, char *name = "name", int32_t *len = NULL );

	// . filters out tags (uses html entities) and stores in "buf"
	// . replaces "line breaking" html tags with 2 returns
	// . only get text of nodes in [node1,node2]
	// . returns # chars written to buf
	// . buf is NULL terminated
	// . bufMaxSize is usually at least getContentLen() + 1 (m_xmlLen+1)
	// . maxDepth is RELATIVE to node # nodeNumber's depth
	// . if "filter" then convert html entities and \r's to spaces
	// . get kid text of node #"nodeNumber" unless it's -1
	// . if "filterSpaces" then don't allow back to back spaces or \n's
	//   and replace tags with ".." not \n (but no back to back ..'s)
	int32_t getText( char *buf, int32_t bufMaxSize, int32_t node1 = 0, int32_t node2 = 999999,
					 bool filterSpaces = false );

	int32_t isRSSFeed();

	char *getRSSTitle( int32_t *titleLen, bool *isHtmlEncoded ) const;
	char *getRSSDescription( int32_t *titleLen, bool *isHtmlEncoded );

	// . used by getValueAsBool/Long/String()
	// . tagName is compound for xml tags, simple for html tags
	char *getTextForXmlTag( int32_t n0, int32_t n1, char *tagName, int32_t *len,
							bool skipLeadingSpaces ) const;

private:
	// used because "s" may have words separated by periods
	int64_t getCompoundHash( char *s, int32_t len ) const;

	XmlNode *m_nodes;
	int32_t m_numNodes;
	int32_t m_maxNumNodes;

	char *m_xml;
	int32_t m_xmlLen;

	// If this is a unicode buffer, then m_xml is encoded in UTF-16
	// m_xmlLen is still the size of the buffer IN BYTES
	int32_t m_version;

	int32_t m_niceness;

	// if we own the data we free m_xml on reset or destruction
	bool m_ownData;

	// size of buffer, if we allocated it
	int32_t m_allocSize;
};

#endif
