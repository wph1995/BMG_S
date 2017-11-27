#ifndef _M_BMGS_H__
#define _M_BMGS_H__



enum ENUM_DMS_State                  //分析状态
{	
	E_ConnBus=0,					//准备开始
	E_ScrMeta,
	E_ScrSql,
	E_Exit
};

struct BMG_RedisEnv
{
	redisContext *ctx;
	redisReply *reply;
	char *pJobName;	  //工作文件            
	char *pMotaDir;	  //工作文件的路径
	char *sysname;   //管理用户
	char *syspasswd; //管理用户密码
	char aIP[20];	  //总线ip
	int  nPort;  	  //总线端口
	int	 nPid;		  //进程ID
	
	BMG_RedisEnv()
	: nPort(6379)
	{
		ctx=NULL;
		strcpy(aIP,"127.0.0.1");
	}
};

struct BMG_OraEnv     
{
	char dbname[30];    //数据库别名
	char username[30];  //用户名
	char passwd[30];    //密码
	char tabname[30];   //表名
	int startsb;		//状态表开始批次
	int endsb;			//状态表结束批次
	//~ char pSrcOwner[30];
	OCI_Connection *pSrcOraConn;    //Oracle physical connection
	//~ OCI_Connection *pTgtOraConn;    //Oracle physical connection
	OCI_TypeInfo   *pOraTabInfo;   //Type info metadata
	OCI_Statement  *pOraStmt;    //A Statement object allows users to prepare,   execute SQL orders or PL/SQL blocks
	OCI_Resultset  *pOraRS;      // A resultset object is the result of 'select' SQL Statement.				
};

struct ST_TabInfo
{
	char cOwn[100];
	char cTab[100];
	
	ST_TabInfo()
	{
		bzero(cOwn,sizeof(cOwn));
		bzero(cTab,sizeof(cTab));
	}
	
};

typedef  vector<ST_TabInfo>   vctTabInfo_t	;		
#endif
