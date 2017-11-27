#ifndef __ORAMETA_H__
#define __ORAMETA_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "lua.hpp"
#define CO_SIZE  100
#define OCI_CDT_OBJECT                      12
#define OCI_CDT_COLLECTION                  13

struct ST_TmpColMeta
{
	char  cKey[5];   //11-14
	int nColLevel;
	int nColId;         //Column id
	string strColName;
	string strSqlType;	//string of sql type 
	int  nCollType;
	int  nCollMax;
	int  nMainType;		//main type 
	int  nSubType;      //second type 
	int  nScale;		//
	int  nColLen;		//Lengthe of column 
	int	 nDisLen;
	int  nPrecs;			// data precision 
	char chNull;			// can be Null?
	string MapKey;
	ST_TmpColMeta () 
	:	nColLevel(-1)
	,   nColId(-1)
	,	nCollType(-1)
	,	nCollMax(-1)
	,	nMainType (-1)
	,   nSubType( -1)
	,   nScale(0)
	,   nColLen(0)
	,	nDisLen(0)
	,   nPrecs(0)
	,   chNull('?')
	{
		bzero(cKey,sizeof(cKey));
	}
};
//====COL META INFO 
struct ST_ColMeta         //列meta
{
	char  cKey[5];       //11-14
	int nColLevel;
	int nColId;         //Column id 
	string strColName;
	string strSqlType;	//string of sql type 
	int  nMainType;		//main type 
	int  nSubType;      //second type 
	int  nScale;		//
	int  nColLen;		//Lengthe of column 
	int  nDisLen;       //Display Length of column
	int  nPrecs;			// data precision 
	char chNull;			// can be Null?
	const char * pColData;				//指向字段的数据开始部分 -- out
	const char * pColHead;				//指向字段的开始部分 -- out  (vc字段和pColData不等)
	int  nColDataLen;						//存放COL数据的长度    -- out 
	ST_ColMeta() 
	:	nColLevel(-1)
	,   nColId(-1)
	,	nMainType (-1)
	,   nSubType( -1)
	,   nScale(0)
	,   nColLen(0)
	,	nDisLen(0)
	,   nPrecs(0)
	, 	chNull('?')
	,	nColDataLen(0)
	{
		bzero(cKey,sizeof(cKey));
	}
};
struct ST_CollMeta
{
	char  	cKey[5];
	int 	nColLevel;
	int 	nColId;
	string 	strCollName;
	string 	strSqlType;	//string of sql type 
	int 	nCollType;  //Collection的类型 OCI_COLL_VARRAY 1，OCI_COLL_NESTED_TABLE 2
	int 	nCollMax;	//变长数组中值包含的最大个数
	int 	nDisLen;
	int  	nMainType;		//main type 
	int  	nSubType;      //second type 
	int  	nColLen;		//Lengthe of column 
	int  	nPrecs;		// data precision 
	int  	nScale;		//
	char 	chNull;		// can be Null?
	const char * pColData;				//指向字段的数据开始部分 -- out
	const char * pColHead;				//指向字段的开始部分 -- out  (vc字段和pColData不等)
	int  nColDataLen;						//存放COL数据的长度    -- out
	ST_CollMeta()
	:		nColLevel (-1)
	,		nColId (-1)
	,		nCollType (-1)
	,		nCollMax (-1)
	,		nDisLen(-1)
	,		nMainType (-1)
	,   	nSubType( -1)
	,   	nColLen(0)
	,   	nPrecs(0)
	,		nScale(0)
	, 		chNull('?')
	,	nColDataLen(0)
	{
		bzero(cKey,sizeof(cKey));
	}
};

struct ST_SubCollData
{
	int nColDataLen;
	string strData;
	ST_SubCollData () :nColDataLen(-1) {}
};
//===========GLOBAL DECLARATION ===============
typedef  vector<ST_ColMeta>   vctColMeta_t	;		//define colmeta array   vector:array
typedef  vector<ST_CollMeta>  vctCollMeta_t	;		//define collmeta array
typedef	 vector<ST_SubCollData> vctSubCollData_t;
struct ST_TabMeta      //meta定义
{
	vctColMeta_t BottomMeta;
	map <string ,vctColMeta_t > mapObjMeta;  //定义map 处理对象嵌套问题
	map <string, vctCollMeta_t > mapCollMeta; //处理collection多个问题	
};

struct ST_CollData
{
	map <string,vctSubCollData_t > mapCollData;
};
typedef  map <string,vctColMeta_t>::iterator mapObjIter;
typedef  map <string,vctCollMeta_t>::iterator mapCollIter;


  //============// declare function ============
bool MLdr_Clear();
void walk (lua_State *L, int idx,ST_TabMeta & aTabMeta);
int output_Meta(ST_TabMeta & aTabMeta);
int  print_pair (lua_State *L,ST_TabMeta & aTabMeta);
bool GetTabMeta(const char *pMetaFile, ST_TabMeta & aTabMeta);
bool GetObjOrCollKey(char *pOCkey,char *pKey,int nLevel,char cLevel,int nColId);
bool C_GenMetaFile(ST_TabMeta & aTabMeta);
bool  C_GetObjMeta2Buf(char *pObjMetaBuf,char *pKey,int nLevel,char cLevel,int nColId,ST_TabMeta & aTabMeta);
bool C_GetCollMeta2Buf(char *pCollMetaBuf,char *pKey,int nLevel,char cLevel,int nColId,ST_TabMeta & aTabMeta);
int GetAllocRowLen(vctColMeta_t & MyGetMeta);


#endif
