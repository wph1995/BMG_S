#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>	
#include "lua.hpp"
#include "hiredis.h"
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

#include "OraMeta.h"

//lua用到
#define KEY -2
#define VAL -1

#define ACCESS access  


class lessLevelST_ColMeta
{
public:
  bool operator() (const ST_ColMeta &lhs, const ST_ColMeta &rhs) const   //按iVal的升序排
  { return lhs.nColLevel < rhs.nColLevel; }
};

class lessIDST_ColMeta
{
public:
  bool operator() (const ST_ColMeta &lhs, const ST_ColMeta &rhs) const   //按iVal的升序排
  { return lhs.nColId < rhs.nColId; }
};
class lessIDST_CollMeta
{
public:
  bool operator() (const ST_CollMeta &lhs, const ST_CollMeta &rhs) const   //按iVal的升序排
  { return lhs.nColId < rhs.nColId; }
};
map <int, int> mapNCurLvlObj; //当前层obj个数

//###########GLOBAL DEFINITION ###################

int output_Meta(ST_TabMeta & aTabMeta)
{
	sort(aTabMeta.BottomMeta.begin(), aTabMeta.BottomMeta.end(), lessIDST_ColMeta());             //??
	mapObjIter mapObjOutPut;
	for( mapObjOutPut = aTabMeta.mapObjMeta.begin(); mapObjOutPut != aTabMeta.mapObjMeta.end() ; ++mapObjOutPut)
	{
		vector <ST_ColMeta> &vctColMeta = mapObjOutPut->second;
		sort(vctColMeta.begin(), vctColMeta.end(), lessIDST_ColMeta());
		/*int nColSize = vctColMeta.size();
		for (int i = 0; i < nColSize; i++)
		{
			ST_ColMeta & aColMeta = vctColMeta[i];
		printf(" ColLevel:%d ,ColId:%d,ColName:%s,   ColLen:%d ,  ColType:%s,  ColPrecs:%d,  ColScale:%d, DisLen:%d\n",aColMeta.nColLevel,
			aColMeta.nColId,  aColMeta.strColName.c_str(), aColMeta.nColLen ,  aColMeta.strSqlType.c_str(),  aColMeta.nPrecs,  aColMeta.nScale,aColMeta.nDisLen);
		}*/
	}
		mapCollIter mapCollOutPut;
	for( mapCollOutPut = aTabMeta.mapCollMeta.begin(); mapCollOutPut != aTabMeta.mapCollMeta.end() ; ++mapCollOutPut)
	{
		vector <ST_CollMeta> &vctCollMeta = mapCollOutPut->second;
		sort(vctCollMeta.begin(), vctCollMeta.end(), lessIDST_CollMeta());
		/*int nColSize = vctCollMeta.size();
		for (int i = 0; i < nColSize; i++)
		{
			ST_CollMeta & aCollMeta = vctCollMeta[i];
			printf(" ColLevel:%d ,ColId:%d,ColName:%s,   ColLen:%d ,  ColType:%s,  ColPrecs:%d,  ColScale:%d, DisLen:%d\n",aCollMeta.nColLevel,
				aCollMeta.nColId,  aCollMeta.strCollName.c_str(), aCollMeta.nColLen ,  aCollMeta.strSqlType.c_str(),  aCollMeta.nPrecs,  aCollMeta.nScale,aCollMeta.nDisLen);
		}*/
	}
	return 0;
}
//获取第一层的Meta放到aTabMeta.BottomMeta(vector)
bool GenVScalar_Meta(ST_TabMeta & aTabMeta,  ST_TmpColMeta & aTmpColMeta)
{
	ST_ColMeta aColMeta;
	//aColMeta.nKeyIdx =aTmpColMeta.nKeyIdx; 11-14
	strcpy(aColMeta.cKey,aTmpColMeta.cKey);
	aColMeta.nColId = aTmpColMeta.nColId; 
	aColMeta.nColLevel = aTmpColMeta.nColLevel;    
	aColMeta.strColName = aTmpColMeta.strColName;
	aColMeta.strSqlType = aTmpColMeta.strSqlType;
	aColMeta.nColLen = aTmpColMeta.nColLen;
	aColMeta.nDisLen = aTmpColMeta.nDisLen;
	aColMeta.nPrecs = aTmpColMeta.nPrecs;
	aColMeta.nScale	= aTmpColMeta.nScale;
	aColMeta.chNull = aTmpColMeta.chNull;
	aColMeta.nMainType =  aTmpColMeta.nMainType; 
	aColMeta.nSubType =  aTmpColMeta.nSubType;  
	aTabMeta.BottomMeta.push_back(aColMeta);
	return true;
}
//获取第二层及以后的ObjMeta放到aTabMeta.mapObjMeta（Map)
bool GenMObj_Meta(ST_TabMeta & aTabMeta,  ST_TmpColMeta & aTmpColMeta)
{
	ST_ColMeta aColMeta;
	//aColMeta.nKeyIdx =aTmpColMeta.nKeyIdx;  11-14
	strcpy(aColMeta.cKey,aTmpColMeta.cKey);
	aColMeta.nColId = aTmpColMeta.nColId; 
	aColMeta.nColLevel = aTmpColMeta.nColLevel;    
	aColMeta.strColName = aTmpColMeta.strColName;
	aColMeta.strSqlType = aTmpColMeta.strSqlType;
	aColMeta.nColLen = aTmpColMeta.nColLen;
	aColMeta.nDisLen = aTmpColMeta.nDisLen;
	aColMeta.nPrecs = aTmpColMeta.nPrecs;
	aColMeta.nScale	= aTmpColMeta.nScale;
	aColMeta.chNull = aTmpColMeta.chNull;
	aColMeta.nMainType =  aTmpColMeta.nMainType; 
	aColMeta.nSubType =  aTmpColMeta.nSubType; 
	mapObjIter MObjFind = aTabMeta.mapObjMeta.find(aTmpColMeta.MapKey);
	if(MObjFind != aTabMeta.mapObjMeta.end()) {    //有该Key，则只更改Map中的Vct即可
		//aTabMeta.mapObjMeta.erase (MObjFind);     //去掉该节点
		vector <ST_ColMeta> & Vct_oldMeta = MObjFind->second;
		Vct_oldMeta.push_back(aColMeta);
		//aTabMeta.mapObjMeta.insert(make_pair(aTmpColMeta.MapKey,Vct_oldMeta)); //Insert
	} else {    
                   //没有该Key，则需要是用insert。
		vector <ST_ColMeta> Vct_newMeta;
		Vct_newMeta.clear();
 
		Vct_newMeta.push_back(aColMeta);     //Push_backe
		aTabMeta.mapObjMeta.insert(make_pair(aTmpColMeta.MapKey,Vct_newMeta)); //Insert
	}
	return true;
}
//获取二层及以后的MapColl放到aTabMeta.mapCollMeta（map)中
bool GenMColl_Meta(ST_TabMeta & aTabMeta, ST_TmpColMeta & aTmpColMeta)
{
	ST_CollMeta aCollMeta;
	//aCollMeta.nKeyIdx =aTmpColMeta.nKeyIdx; 11-14
	strcpy(aCollMeta.cKey,aTmpColMeta.cKey);
	aCollMeta.nColId = aTmpColMeta.nColId; 
	aCollMeta.nColLevel = aTmpColMeta.nColLevel;    
	aCollMeta.strCollName = aTmpColMeta.strColName;
	aCollMeta.strSqlType = aTmpColMeta.strSqlType;
	aCollMeta.nColLen = aTmpColMeta.nColLen;
	aCollMeta.nDisLen = aTmpColMeta.nDisLen;
	aCollMeta.nPrecs = aTmpColMeta.nPrecs;
	aCollMeta.nScale	= aTmpColMeta.nScale;
	aCollMeta.chNull = aTmpColMeta.chNull;
	aCollMeta.nMainType =  aTmpColMeta.nMainType; 
	aCollMeta.nSubType =  aTmpColMeta.nSubType; 
	aCollMeta.nCollType  =  aTmpColMeta.nCollType;
	aCollMeta.nCollMax  =  aTmpColMeta.nCollMax;
	mapCollIter MCollFind = aTabMeta.mapCollMeta.find(aTmpColMeta.MapKey);
	if(MCollFind != aTabMeta.mapCollMeta.end()) {   //有该Key，则只更改Map中的Vct即可
		vector <ST_CollMeta> & Vct_oldMeta = MCollFind->second;
		Vct_oldMeta.push_back(aCollMeta);
	} else {                       //没有该Key，则需要是用insert。
		vector <ST_CollMeta> Vct_newMeta;
		Vct_newMeta.clear();
 
		Vct_newMeta.push_back(aCollMeta);     //Push_backe
		aTabMeta.mapCollMeta.insert(make_pair(aTmpColMeta.MapKey,Vct_newMeta)); //Insert
	}
	return true;
}


//根据当前字段的TMpColMEta分析判断该字段在表中的位置且使用哪种结构来存放
bool GenTM_Struct(ST_TabMeta & aTabMeta, ST_TmpColMeta & aTmpColMeta)
{
	if(aTmpColMeta.nColLevel == 1) { //该字段处于表中第一层 ---使用ST_ColMeta表示 存放到Vct中
		GenVScalar_Meta(aTabMeta, aTmpColMeta);
	}
	else {
		if(aTmpColMeta.nCollType == 0) { //该字段属于obj类型下的子类型 --使用MapObj存放
			GenMObj_Meta(aTabMeta, aTmpColMeta);
		} else if (aTmpColMeta.nCollType == 1) { //该字段输入Coll类型下的子类型 --使用MapColl存放
			GenMColl_Meta(aTabMeta, aTmpColMeta);
		}else if (aTmpColMeta.nCollType == -1){
			return true;
		}
		 else {
			printf("Nest TABLE ,This is invalid\n");
		}
	}
	return true;
}

//key类型有：LUA_TNUMBER /LUA-TSTRING value类型有：LUA_TNUMBER/LUA_TSTRING/LUA_TTABLE
int  print_pair (lua_State *L,ST_TabMeta & aTabMeta)
{
	char cValBuf[200] = "";
	char cSqlType[40]="";
	char cMapKey[300]="";
	string strSqlType;
	string strSubSqlType;
	string strColName;
	ST_TmpColMeta aTmpColMeta;
	
    switch (lua_type(L, KEY)) {
        case LUA_TNUMBER:
             break;
        case LUA_TSTRING:
			 strColName = lua_tostring(L, KEY);
			 //printf("strConName:%s\n",strColName.c_str() );
			 if(strcmp(strColName.substr(0, 4).c_str(), "desc" ) !=0 )
				aTmpColMeta.strColName = strColName; //不是desc说明是标量数据类型，不是lua中的表    
    }
    
    switch (lua_type(L, VAL)) {
        case LUA_TNUMBER:
             break;
        case LUA_TSTRING:
            strcpy(cValBuf , lua_tostring(L, VAL));
            //printf("cValBuf: %s\n",cValBuf);
            sscanf(cValBuf, "I:%d, MP:'%[^']', K:%[^,], F:%d,  T:'%[^']', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:'%c', T:%d_%d", 
				 &aTmpColMeta.nColId,cMapKey, aTmpColMeta.cKey,&aTmpColMeta.nColLevel, cSqlType,&aTmpColMeta.nCollType,&aTmpColMeta.nCollMax,
				 &aTmpColMeta.nColLen , &aTmpColMeta.nDisLen,&aTmpColMeta.nPrecs,  &aTmpColMeta.nScale , &aTmpColMeta.chNull, 
				 &aTmpColMeta.nMainType, &aTmpColMeta.nSubType); 	 
				strSqlType = cSqlType;	
				aTmpColMeta.MapKey=cMapKey; 			 
				if(strcmp(strSqlType.c_str(),"") != 0) 
				{	
					strSubSqlType = strSqlType.substr(0,4);
					aTmpColMeta.strSqlType = strSqlType.substr(4,strlen(strSqlType.c_str()));
				}
             break;
        case LUA_TTABLE:
             walk(L, lua_gettop(L),aTabMeta);	//获取栈顶信息
             break;
        default:
             printf("val: other\n");
    }
    GenTM_Struct(aTabMeta, aTmpColMeta);
	//printf("%s = I:%d, K:%d, F:%d,T:'NOR_%s', L:%d, P:%d, S:%d , N:'%c',T:%d_%d\n ",aTmpColMeta.strColName.c_str(), aTmpColMeta.nColId, aTmpColMeta.nKeyIdx,aTmpColMeta.nColLevel, aTmpColMeta.strSqlType.c_str(),  aTmpColMeta.nColLen , aTmpColMeta.nPrecs,  aTmpColMeta.nScale , aTmpColMeta.chNull, aTmpColMeta.nMainType, aTmpColMeta.nSubType);
	return 0;
}
/*
 *  Walk The nested Table
 */
void walk (lua_State *L, int idx,ST_TabMeta & aTabMeta) 
{
	
    lua_pushnil(L);			//把当前的表中的key/value赋值 为nil--------相当于空
   // printf("idx=%i\n", idx);
    while (lua_next(L, idx) != 0) {		//把L中的value/key去除，遍历L的值， ---相当于OCI_FetchNext
       //ST_ColMeta *pColMeta = 
       print_pair(L,aTabMeta);			//打印L中的key/value 
        lua_pop(L, 1);			//重新设置lua中的表的位置 为1
    }
}


int GetAllocRowLen(vctColMeta_t & MyGetMeta)
{
	int nRowMenLen = 0 ;
	int nColSize = MyGetMeta.size();   //一行记录的长度
	for ( int i = 0; i < nColSize; i++)
	{
		ST_ColMeta & aColMeta = MyGetMeta[i];      //aColMeta: ST_ColMeta的一个对象 
		nRowMenLen += aColMeta.nDisLen + 3;
	}
	 return nRowMenLen;
}
bool GetTabMeta(const char *pMetaFile, ST_TabMeta & aTabMeta)
{
    lua_State *L;					//定义lua状态变量
    int t_idx;
    L = luaL_newstate();			//为lua状态变量赋值 ---创建写的lua状态
    luaL_openlibs(L);				//打开lua库
    luaL_loadfile(L, pMetaFile);		//打开一个.lua文件----表示表结构的meta文件
    //printf("pMetaFile:%s\n",cLuaPath);				
    if (lua_pcall(L,0, LUA_MULTRET,0) != 0) 
    {			//meta文件的内容是否在lua中执行一次，相当于sql语句执行
        printf("Got Err In LuaFile:%s Msg:%s \n", pMetaFile, lua_tostring(L, -1));
        return -1;
        //exit(-1);
    }
    lua_getglobal(L,"Meta");			//获取lua中的根表层的名字----在meta.lua中为Meta		
    t_idx = lua_gettop(L);			//获取栈顶的位置  正数 ------采用栈（先进后出）存储
    aTabMeta.BottomMeta.clear(); //清除aTabMeta中的数据
    aTabMeta.mapObjMeta.clear();
    aTabMeta.mapCollMeta.clear();
    walk(L, t_idx,aTabMeta);					//遍历meta表结构内容

	output_Meta(aTabMeta);
	C_GenMetaFile(aTabMeta);
	
    lua_close(L);
    
  
    return true;
}
bool GetObjOrCollKey(char *pOCkey,char *pKey,int nLevel,char cLevel,int nColId)
{
	int nPerKeyid = 3*(nLevel -1);	 //  理论上前一个objKEy的cLevel的位置
	int nObjOrCollKeyLen = strlen(pKey); int nPerObjOrCollKeycLevelId = nObjOrCollKeyLen -2;//实际上
	char cPerObjOrCollKeycLevel = pKey[nPerObjOrCollKeycLevelId];
	//判断前一个cLevel的值是否大于等于key中的最后一个的前一个cLevel值。 //这是处理嵌套对象中包含多个对象
	if(cPerObjOrCollKeycLevel >= cLevel) //>的情况是在当前的obj是上个处理的层数的上面（如a1(b1(c1,c2),b2)，上次处理的是c2,这次处理的是b2);
								   //=的情况出现在当前的obj与上次处理的obj在相同层面上，如c1,c2
	{	
		char cObjOrCollKey2[300] = "";
	 	char cObjOrCollKey3[300] = "";	
		strncpy(cObjOrCollKey2,pKey,nPerKeyid+2);
		cObjOrCollKey2[nPerKeyid+2]='\0';
		strcpy(cObjOrCollKey3,cObjOrCollKey2);
		strcpy(pKey,cObjOrCollKey3);
	}

	char cKey1[200] = "";
	sprintf(cKey1,":%c%d",cLevel,nColId);
	strcat(pKey,cKey1);
	sprintf(pOCkey,"%s",pKey);
	return true;
}
bool C_GenMetaFile(ST_TabMeta & aTabMeta)
{
	//1.0 把表的结构按照lua格式先写入到缓冲区cbuf中
	
	char cbuf[4096] = "";
	char pMapKey[300] = "";
	char cObjBuf[1024]="";
	strcpy(cbuf, "Meta = { \n");
	char cColMetaBuf[1024] = "";	
	char cCollBuf[1024] = "";
	
	mapNCurLvlObj.clear();
	char pObjMetaBuf[2048] = "";	
	char pCollMetaBuf[2048] = "";
	char cLevel = 'a';
	int nLevel = 1;		
	int nColSize = aTabMeta.BottomMeta.size();		
	for ( int i = 0; i < nColSize; i++)
	{
		//printf("i ----------%d\n",i);
		int nColMetaColId = i + 1;
		ST_ColMeta & aColMeta = aTabMeta.BottomMeta[i];
		if((aColMeta.nMainType != OCI_CDT_OBJECT) && (aColMeta.nMainType !=  OCI_CDT_COLLECTION)) {
			if(nColMetaColId < nColSize) {
			sprintf(cColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \",\n",
				  aColMeta.strColName.c_str(), aColMeta.nColId,"N", aColMeta.cKey, aColMeta.nColLevel,aColMeta.strSqlType.c_str(), 0,0,
				  aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull, aColMeta.nMainType, aColMeta.nSubType);
			} else {
			sprintf(cColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \"\n",
				  aColMeta.strColName.c_str(), aColMeta.nColId,"N", aColMeta.cKey, aColMeta.nColLevel,aColMeta.strSqlType.c_str(), 0,0,
				aColMeta.nColLen , aColMeta.nDisLen,aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull,aColMeta.nMainType, aColMeta.nSubType);
			}
			strcat(cbuf, cColMetaBuf);
		}else if(aColMeta.nMainType == OCI_CDT_OBJECT) {
			sprintf(pMapKey, "%c%d", cLevel, nColMetaColId);
			//printf("cKey:%s\n", pMapKey);
			sprintf(cObjBuf,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'OBJ_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \", desc%d%d= {\n",
			 aColMeta.strColName.c_str(), aColMeta.nColId,"N", aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(),0,0,
				aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull,aColMeta.nMainType, aColMeta.nSubType,aColMeta.nColLevel,aColMeta.nColId);
			strcat(cbuf,cObjBuf);
			mapNCurLvlObj.insert(make_pair(nLevel,0));
			C_GetObjMeta2Buf(pObjMetaBuf,pMapKey,nLevel,'b',1,aTabMeta);
			strcat(cbuf,pObjMetaBuf);
			if( nColMetaColId < nColSize) {
				strcat(cbuf, " }\n	,\n");
			} else {	
				strcat(cbuf, " 	}\n	\n");
			}
		} else if(aColMeta.nMainType ==  OCI_CDT_COLLECTION) {
			sprintf(pMapKey,"%c%d",cLevel,nColMetaColId);			
			sprintf(cCollBuf,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'COL_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \", desc%d%d= {\n", 
			aColMeta.strColName.c_str(), aColMeta.nColId, "N",aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(), 0,0,
				aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull,aColMeta.nMainType, 
				aColMeta.nSubType,aColMeta.nColLevel,aColMeta.nColId);
			strcat(cbuf,cCollBuf);
			mapNCurLvlObj.insert(make_pair(nLevel,0));

			C_GetCollMeta2Buf(pCollMetaBuf,pMapKey,nLevel,'b',1,aTabMeta);
			strcat(cbuf,pCollMetaBuf);
			if( nColMetaColId < nColSize) {
				strcat(cbuf, " }\n	,\n");
			} else {
				strcat(cbuf, " 	}\n  \n");				
			}	
		} 
	}
	strcat(cbuf, " } \n");
	//printf("buf: %s\n",cbuf);
	//2.0 打开文件，把cbuf中的内容写入到文件中，并关闭文件
	/*FILE *pTabMetaFp;
	pTabMetaFp = fopen("./1.lua", "w+");
	if (!pTabMetaFp) {
		printf("Cant't open Table Meta File : %s\n"," 1.lua");
		return FALSE;
	}
	//向文件添加内容使用fprintf
	fprintf(pTabMetaFp,"%s",cbuf);
	fclose(pTabMetaFp);*/
	return true;
}


//把对象类型类型的结构放到缓冲区中
bool C_GetObjMeta2Buf(char *pObjMetaBuf,char *pKey,int nLevel,char cLevel,int nColId,ST_TabMeta & aTabMeta)
{
	
	char cObjBuf[2048] = "";
	char cSubObjBuf[1024]="";
	char cObjColMetaBuf[1024] = "";
	char cSubObjBuf1[500] = "";
	char cCollBuf1[500] = "";
	int nCurLevel;
	char pObjKeyBuf[300] = "";
	
	char pCollMetaBuf[2048] = "";
	GetObjOrCollKey(pObjKeyBuf,pKey,nLevel,cLevel,nColId);	 
	map <string,vctColMeta_t>::iterator mapObjIterFind;
	
	mapObjIterFind = aTabMeta.mapObjMeta.find(pObjKeyBuf);
	if(mapObjIterFind != aTabMeta.mapObjMeta.end())
		printf("map ObjKey:%s=== \n",mapObjIterFind->first.c_str());
	vector <ST_ColMeta> &vctColMeta = mapObjIterFind->second;
	int nObjColSize = vctColMeta.size();
	//初始化当前subObj/subColl的个数为0

	for(int j = 0; j < nObjColSize; j++)
	{
		int nObjMetaColId = j + 1;
		ST_ColMeta & aColMeta = vctColMeta[j];
		if((aColMeta.nMainType != OCI_CDT_OBJECT) && (aColMeta.nMainType !=  OCI_CDT_COLLECTION)){
			if(nObjMetaColId < nObjColSize) {
			sprintf(cObjColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \",\n",
				  aColMeta.strColName.c_str(), aColMeta.nColId, pObjKeyBuf, aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(), 0,0,
				  aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull, aColMeta.nMainType, aColMeta.nSubType);
			} else {
			sprintf(cObjColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \"\n",
				  aColMeta.strColName.c_str(),  aColMeta.nColId, pObjKeyBuf,aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(), 0,0,
				  aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull, aColMeta.nMainType, aColMeta.nSubType);
			}
			
			//printf("cObjColMetaBuf : %s\n",cObjColMetaBuf);
			//strcat(cObjBuf,cObjColMetaBuf);
		} else if(aColMeta.nMainType == OCI_CDT_OBJECT) {   //如果字段类型为obj，则先获取字段信息再处理
		   //if(nObjMetaColId == 1) {nLevel++;     //嵌套对象中可能又包含多个对象，只在首次处理时，层数+1
		  // }
		  
		//  printf("nLevel find iterator:%d\n",nLevel);
		  map <int ,int>::iterator mapSubObjFind;
		  mapSubObjFind = mapNCurLvlObj.find(nLevel);
		  if(mapSubObjFind != mapNCurLvlObj.end()) {
			  if(mapSubObjFind->second == 0) {
				nCurLevel = nLevel + 1;  
				  // 初始化当前subObj/subColl的个数为0
				mapNCurLvlObj.insert(make_pair(nCurLevel,0));
				}
		  }	
		   char cCurLevel = 'a' + nCurLevel -1;
		   cLevel= cCurLevel + 1;
		   sprintf(cSubObjBuf1,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'OBJ_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \", desc%d%d= {\n",
		   aColMeta.strColName.c_str(),  nObjMetaColId, pObjKeyBuf,	aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(), 0,0,
				  aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull,aColMeta.nMainType, aColMeta.nSubType,aColMeta.nColLevel,aColMeta.nColId);
			strcat(cSubObjBuf,cSubObjBuf1);
		   C_GetObjMeta2Buf(pObjMetaBuf,pKey,nCurLevel,cLevel,nObjMetaColId,aTabMeta);	
		   strcat(cSubObjBuf,pObjMetaBuf);	
			if( nObjMetaColId < nObjColSize) {
				strcat(cSubObjBuf, " 			}	,\n");
			} else {
				strcat(cSubObjBuf, " }		\n");
			}
			//printf("	cObjBuf1:%s\n",cSubObjBuf);
			strcpy(cObjColMetaBuf,cSubObjBuf);
			memset(cSubObjBuf, 0x0, 1024 * sizeof(char));; //清空cSubObjBuf 操作的是内存
	   } else if(aColMeta.nMainType == OCI_CDT_COLLECTION) { //如果字段类型为coll，则先获取字段信息再处理
		   if(nObjMetaColId == 1) {
			  nCurLevel= nLevel+1;
		   }
		   char cCurLevel = 'a' + nCurLevel -1;
		   cLevel = cCurLevel +1;
		   sprintf(cCollBuf1,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'COL_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \", desc%d%d= {\n",
		   aColMeta.strColName.c_str(),  nObjMetaColId, pObjKeyBuf, aColMeta.cKey,aColMeta.nColLevel, aColMeta.strSqlType.c_str(), 0,0,
				  aColMeta.nColLen ,aColMeta.nDisLen, aColMeta.nPrecs,  aColMeta.nScale , aColMeta.chNull,aColMeta.nMainType, aColMeta.nSubType,aColMeta.nColLevel,aColMeta.nColId);		  
			strcat(cSubObjBuf,cCollBuf1);
		  // string strCollSubBuf = GetCollMeta(cObjKey,nLevel,cLevel,nObjMetaColId);
			C_GetCollMeta2Buf(pCollMetaBuf,pKey,nCurLevel,cLevel,nObjMetaColId,aTabMeta);		
		    strcat(cSubObjBuf,pCollMetaBuf);
		   if( nObjMetaColId < nObjColSize) {
				strcat(cSubObjBuf, " 			},\n");
			} else {
				strcat(cSubObjBuf, " }			\n");
			}
			strcpy(cObjColMetaBuf,cSubObjBuf);
		}
		strcat(cObjBuf,cObjColMetaBuf);
	}	
	//printf("cObjBuf:%s\n",cObjBuf);
	strcpy(pObjMetaBuf,cObjBuf);
	
	return true;
}
//把变长数组的结构放到文件中
bool C_GetCollMeta2Buf(char *pCollMetaBuf,char *pKey,int nLevel,char cLevel,int nColId,ST_TabMeta & aTabMeta)
{
	char cCollBuf[3072] = "";
	char cCollColMetaBuf[1024] = "";
	char cSubObjBuf1[500] = "";
	char cCollBuf1[500] = "";
	int nCurLevel;
	char pCollKeyBuf[300] = "";
	char pObjMetaBuf[2048] ="";
	
	GetObjOrCollKey(pCollKeyBuf,pKey, nLevel, cLevel, nColId);

	map <string, vctCollMeta_t>::iterator mapCollIterFind;
	mapCollIterFind = aTabMeta.mapCollMeta.find(pCollKeyBuf);
	if(mapCollIterFind != aTabMeta.mapCollMeta.end()) {
		printf("CollKey: %s\n",mapCollIterFind->first.c_str());
	}
	vector <ST_CollMeta> &vctCollMeta = mapCollIterFind->second;
	int nCollSize = vctCollMeta.size();	
	for( int i = 0; i < nCollSize; i++)
	{
		int nCollMetaColId = i + 1;
		ST_CollMeta &aCollMeta = vctCollMeta[i];
		if((aCollMeta.nMainType != OCI_CDT_OBJECT) && (aCollMeta.nMainType !=  OCI_CDT_COLLECTION)){
			if(nCollMetaColId < nCollSize) {
			sprintf(cCollColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \",\n",
				  aCollMeta.strCollName.c_str(), nCollMetaColId,pCollKeyBuf, aCollMeta.cKey, aCollMeta.nColLevel, aCollMeta.strSqlType.c_str(), aCollMeta.nCollType, aCollMeta.nCollMax,
				  aCollMeta.nColLen ,aCollMeta.nDisLen, aCollMeta.nPrecs,  aCollMeta.nScale , aCollMeta.chNull,aCollMeta.nMainType, aCollMeta.nSubType);
			} else {
			sprintf(cCollColMetaBuf, "%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'NOR_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \"",
				  aCollMeta.strCollName.c_str(), nCollMetaColId, pCollKeyBuf, aCollMeta.cKey, aCollMeta.nColLevel, aCollMeta.strSqlType.c_str(), aCollMeta.nCollType, aCollMeta.nCollMax,
				aCollMeta.nColLen , aCollMeta.nDisLen,aCollMeta.nPrecs,  aCollMeta.nScale , aCollMeta.chNull,aCollMeta.nMainType, aCollMeta.nSubType);
			}
			//printf("cCollColMetaBuf : %s\n",cCollColMetaBuf);
		} else if(aCollMeta.nMainType == OCI_CDT_OBJECT) {   //如果字段类型为obj，则先获取字段信息再处理
		   map <int ,int>::iterator mapSubObjFind;
		 mapSubObjFind = mapNCurLvlObj.find(nLevel);
		  if(mapSubObjFind != mapNCurLvlObj.end()) {
			  if(mapSubObjFind->second == 0) {
				nCurLevel = nLevel + 1;  
				  // 初始化当前subObj/subColl的个数为0
				mapNCurLvlObj.insert(make_pair(nCurLevel,0));
				}
		  }	 
		   char cCurLevel = 'a' + nCurLevel -1;
		   cLevel= cCurLevel + 1;
		   sprintf(cSubObjBuf1,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'OBJ_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d, N:\'%c\', T:%d_%d \", desc%d%d= {\n",
		   aCollMeta.strCollName.c_str(), nCollMetaColId, pCollKeyBuf, aCollMeta.cKey, aCollMeta.nColLevel,aCollMeta.strSqlType.c_str(), aCollMeta.nCollType, aCollMeta.nCollMax,
				  aCollMeta.nColLen ,aCollMeta.nDisLen, aCollMeta.nPrecs,  aCollMeta.nScale , aCollMeta.chNull,aCollMeta.nMainType, aCollMeta.nSubType,aCollMeta.nColLevel,aCollMeta.nColId);	  
		   strcat(cCollBuf,cSubObjBuf1);
		   C_GetObjMeta2Buf(pObjMetaBuf,pKey,nCurLevel,cLevel,nCollMetaColId,aTabMeta);	
		   strcat(cCollBuf,pObjMetaBuf);	
			if( nCollMetaColId < nCollSize) {
				strcat(cCollBuf, " 			}		,\n");
			} else {
				strcat(cCollBuf, " }		\n");
			}
	   } else if(aCollMeta.nMainType == OCI_CDT_COLLECTION) { //如果字段类型为coll，则先获取字段信息再处理
		   if(nCollMetaColId == 1) {
			   nLevel++;
		   }
		   char cCurLevel = 'a' + nLevel -1;
		   cLevel = cCurLevel +1;
		   sprintf(cCollBuf1,"%s = \"I:%d, MP:\'%s\', K:%s, F:%d, T:\'COL_%s\', C:%d, M:%d, L:%d, D:%d, P:%d, S:%d , N:\'%c\', T:%d_%d \", desc%d%d= {\n",
		   aCollMeta.strCollName.c_str(), nCollMetaColId, pCollKeyBuf, aCollMeta.cKey, aCollMeta.nColLevel, aCollMeta.strSqlType.c_str(), aCollMeta.nCollType, aCollMeta.nCollMax,
				  aCollMeta.nColLen ,aCollMeta.nDisLen, aCollMeta.nPrecs,  aCollMeta.nScale , aCollMeta.chNull,aCollMeta.nMainType, aCollMeta.nSubType,aCollMeta.nColLevel,aCollMeta.nColId);	  
		   strcat(cCollBuf,cCollBuf1);
		   C_GetCollMeta2Buf(pCollMetaBuf,pKey,nLevel,cLevel,nCollMetaColId,aTabMeta);	
		   strcat(cCollBuf,pCollMetaBuf);
		   if( nCollMetaColId < nCollSize) {
				strcat(cCollBuf, "		 }			   ,\n");
			} else {
				strcat(cCollBuf, " }		 \n");
			}	
		}
	   strcat(cCollBuf,cCollColMetaBuf); 	
	}
	strcpy(pCollMetaBuf,cCollBuf);
	return true;
}


