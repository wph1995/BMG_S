/*
 * 
 * 2017-9-12从总线取信息将源库指定表生成状态表
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "ocilib.h"
#include <vector>
#include <map>
#include <time.h>
#include "hiredis.h"

using namespace std;

#include "OraMeta.h"
#include "BMG_S.h"
#include "util.h"


#define MKDIR(a) mkdir((a),0755) 

BMG_RedisEnv aRedisEnv;
BMG_OraEnv aOraEnv;
FILE * Logfp;
vctTabInfo_t vTabInfo;   //表信息

const char * pMetaDir = "./v3Pro/M_%s.%s.lua";
const char * pSrcSample = "./v3Pro/pro_v3sample.sql";
const char * pTgtSample = "./v3Pro/pro_v3sample_t.sql";
const char * pSrcPro = "./v3Pro/pro_v3%s_%s.sql";
const char * pTgtPro = "./v3Pro/pro_v3%s_%s_t.sql";

const char *prolua = "./genMetaFileNew %s %s nomal ./table.txt";
//~ const char *prosql = "./GenPro ./genProTab.txt";
const char *scsql = "sqlplus %s/%s@%s @./v3Pro/pro_v3%s_%s.sql >/dev/null";

void CheckArgs(int argc, char * argv[] )  //接受文件名和路径
{

	char cKey[3]="";
	char cVlues[200]="";
	if(argc < 5) {
		printf("[Usage] %s  [J:JobName]  [D:MotaDir----../v3Data]  [U:sysname]  [P:syspasswd]\n",argv[0]);    // 20170322 

		printf("\n");
		
		exit(0);
	}
	
	for(int i=1;i<argc;i++)
	{
		sscanf(argv[i],"%[^:]:%s",cKey,cVlues);
		printf("keys:%s,Vlues:%s\n",cKey,cVlues);
		if(!strcasecmp(cKey,"J"))
		{
			aRedisEnv.pJobName = argv[i]+2;
		} else if(!strcasecmp(cKey,"D")) {
			aRedisEnv.pMotaDir = argv[i]+2;
		}else if(!strcasecmp(cKey,"U")) {
			aRedisEnv.sysname = argv[i]+2;
		}else if(!strcasecmp(cKey,"P")) {
			aRedisEnv.syspasswd = argv[i]+2;
		}
	}
	printf("aRedisEnv.pJobName:%s, aRedisEnv.pMotaDir:%s, aRedisEnv.sysname:%s, aRedisEnv.syspasswd:%s\n", 
		aRedisEnv.pJobName, aRedisEnv.pMotaDir, aRedisEnv.sysname, aRedisEnv.syspasswd);
}

bool GenMotaDir(char *pJobName,char *pMotaDir)
{
	char cjobDir[200]="";
	char cdmgDir[200]="";
	sprintf(cjobDir, "%s/%s", pMotaDir, pJobName);
	sprintf(cdmgDir, "%s/dmg_sql", cjobDir);

	int iRet; 
	//如果不存在,创建  
	iRet = access(pMotaDir,0);  
	if (iRet != 0){
		iRet = MKDIR(pMotaDir);
		if (iRet != 0)  
		{  
			printf("cMotaDir Mkdir Failed\n");
			return false;  
		}  
	}
	iRet = access(cjobDir,0);  
	if (iRet != 0){
		iRet = MKDIR(cjobDir);
		if (iRet != 0)  
		{  
			printf("cMotaDir Mkdir Failed\n");
			return false;  
		}  
	}

	iRet = access(cdmgDir,0);  
	if (iRet != 0){
		iRet = MKDIR(cdmgDir);
		if (iRet != 0)  
		{  
			printf("cdmgDir Mkdir Failed\n");
			return false;  
		}  
	}
	
	return true;
}

void WriteLog(char * pJobName,char * pMotaDir)         //写日志  20170322   
{
	char cLogPath[200] = "";
	char cLogconf[200] = "";
	sprintf(cLogconf, "%s/%s/log", pMotaDir,pJobName);
	sprintf(cLogPath,"%s/BMG_S.log", cLogconf);
	
	int iRet; 
	//如果不存在,创建  
	iRet = access(cLogconf,0);  
	if (iRet != 0){
		iRet = MKDIR(cLogconf);
		if (iRet != 0)  
		{  
			printf("cLogconf Mkdir Failed\n");
			perror("conf");
		}  
	}
	printf("Write Log:%s\n",cLogPath);
	Logfp = fopen(cLogPath,"w");
	if(Logfp == NULL)
	{
		perror("fopen");
		printf("fopen error\n");
	}
	fprintf(Logfp,"Write Log Begin!\n");
}

bool BMG_S_Setup(char *pJobName,char *pMotaDir)
{
	GenMotaDir(pJobName,pMotaDir);		//创建相应的文件
	
	WriteLog(pJobName,pMotaDir);        //写日志   20170323
	return true;
}

bool GetTableAndUserIncrease(char *pRunCmd)
{
	//~ Dbname=ora90|Tabname=test|Username=woxin|Passwd=woxin|SB=1|EB=5|
	sscanf(pRunCmd, "Dbname=%[^|]|Tabname=%[^|]|Username=%[^|]|Passwd=%[^|]|SB=%d|EB=%d|", 
		aOraEnv.dbname, aOraEnv.tabname, aOraEnv.username, aOraEnv.passwd, &aOraEnv.startsb, &aOraEnv.endsb);
	
	if(!(aOraEnv.endsb >= aOraEnv.startsb && aOraEnv.endsb >= 0 && aOraEnv.endsb>=0)){
		aOraEnv.endsb = -1;
		aOraEnv.startsb = -1;
	}
		
	printf("===========Run Cmd:%s, db:%s, tab:%s, user:%s, passwd:%s, sb:%d, eb:%d!=============\n",
		pRunCmd, aOraEnv.dbname, aOraEnv.tabname, aOraEnv.username, aOraEnv.passwd, aOraEnv.startsb, aOraEnv.endsb);
	fprintf(Logfp, "===========Run Cmd:%s, db:%s, tab:%s, user:%s, passwd:%s, sb:%d, eb:%d!=============\n",
		pRunCmd, aOraEnv.dbname, aOraEnv.tabname, aOraEnv.username, aOraEnv.passwd, aOraEnv.startsb, aOraEnv.endsb);
	return true;
}

bool GetMsg()	//获取用户名，密码，表名
{
	char cCmd[50] = ""; 

	sprintf(cCmd, "LPOP bmg_s_%s_CR", aRedisEnv.pJobName);
	redisReply * reply = (redisReply *)redisCommand(aRedisEnv.ctx, cCmd);  
	printf("===========BRPOP BMG_S_CR!=============\n"); 
	if ( reply->type == REDIS_REPLY_STRING) {
		printf("reply->str:%s.\n",reply->str);
		GetTableAndUserIncrease(reply->str);
		freeReplyObject(reply);
		
		return true;
	}
	printf("No Msg!\n");
	
	return false;
}

bool ScrMeta()
{
	int fd;
	char tmp[100] = "";
	char buf[100] = "";
	int r;
	
	sprintf(tmp, prolua, aOraEnv.username, aOraEnv.dbname);   //生成lua文件语句
    sprintf(buf, "%s,%s.%s\n", aOraEnv.username, aOraEnv.passwd, aOraEnv.tabname);
	
	fd = open("table.txt", O_RDWR | O_CREAT | O_EXCL, 0666);
	if(fd==-1){
		fd = open("table.txt", O_RDWR | O_TRUNC); 
		if(fd==-1){
			printf("error table.txt:%m\n");
			fprintf(Logfp, "error table.txt:%m\n");
			return false;
		}
	}
	
	do{
		r = write(fd, buf, strlen(buf));
		bzero(buf, sizeof(buf));
	}while(r>0);
	
	int rc = system(tmp);
	if( rc == -1 ||rc == 127 ) {
		printf("system error:%s\n", tmp);
		fprintf(Logfp, "system error:%s\n", tmp);
		return false;
	}
	printf("Create Catalog %s ok\n", tmp);
	fprintf(Logfp, "Create Catalog %s ok\n", tmp);
	
	return true;
}

bool Detercreatetable(char *ptabname)	
{
	fprintf(Logfp,"SrcDbQuery begin!\n");
	char cTabNameBuf[100] = "";		  //搜索该用户下的所有表名命令
	char cCreateTab[200] = "";		  //创建表命令
	char ctabname[100] = "";		  //sysname下的表名 
	char DXsysname[30] = "";          //大写用户名
	
	strncpy(DXsysname, aRedisEnv.sysname, strlen(aRedisEnv.sysname));
	strupr(DXsysname);
	//~ WOXIN_V3STATE_LHS_TEST
	sprintf(ctabname, "%s_V3STATE_%s", DXsysname, ptabname);
	printf("ctabname:%s\n", ctabname);
	
	//1.0 tables  handle 
	sprintf(cTabNameBuf,"select owner||'_'||table_name from all_tables where owner in ('%s')", DXsysname);
	printf("cTabNameBuf=%s\n",cTabNameBuf);
	
	OCI_Statement *srcdb = OCI_StatementCreate(aOraEnv.pSrcOraConn);	
    bool bRet = OCI_ExecuteStmt(srcdb, cTabNameBuf);
    if(!bRet) {
		printf("OCi ExecuteStmt failed\n"); 
		return false;
	}
	 
	//2.0 get rs 
	OCI_Resultset *dbrs = NULL;
    dbrs = OCI_GetResultset(srcdb);
    if(!dbrs) {
		printf("NO Key \n"); 
		return false;
	}
	
	//3.0 fetch all table info 
	while(OCI_FetchNext(dbrs) == true) 
	{
		const char *pSrcTabInfo= OCI_GetString(dbrs, 1) ;
		printf("pSrcTabInfo:%s\n",pSrcTabInfo);
		if(strncmp(ctabname, pSrcTabInfo, strlen(pSrcTabInfo)) == 0){
			printf("SrcDb has the table!\n");
			fprintf(Logfp,"SRC ower.table end!\n");
			OCI_StatementFree(srcdb);
			
			return true;
		}
	}

	sprintf(cCreateTab, "create table V3STATE_%s (batchno int not null primary key, skey varchar2(100),ekey varchar2(100),smd5 varchar2(32),snum int)", ptabname);
	bRet = OCI_ExecuteStmt(srcdb, cCreateTab);
	if(!bRet) {
		printf("OCi ExecuteStmt:%s failed\n", cCreateTab); 
		fprintf(Logfp, "OCi ExecuteStmt:%s failed\n", cCreateTab); 
		OCI_StatementFree(srcdb);
		
		return false;
	}
	OCI_Commit(aOraEnv.pSrcOraConn);
	printf("OCI create table successful!!!\n");
	fprintf(Logfp, "OCI create table successful!!!\n");	
	fprintf(Logfp,"SRC ower.table end!\n");
	OCI_StatementFree(srcdb);
	
	return true;
}

void genOwnerAndTbName(char *cTabPath)
{
	printf("cTabPath=%s\n", cTabPath);
	
	char cLine[1024]="";
	
	FILE * fp = fopen(cTabPath, "r");
	if(!fp) {
		printf("Cann't open File:%s \n", cTabPath);
		return ;
	}
	
	while (!feof(fp))	//--注feof()函数在判断最后一行时会，多get一次。用时注意去掉 
	{
		ST_TabInfo aTabInfo;  
		fgets(cLine, 1024, fp);
		sscanf(cLine, "%[^.].%s", aTabInfo.cOwn, aTabInfo.cTab);
		vTabInfo.push_back(aTabInfo);	
		
	}
	printf("vTabInfo.size():%lu\n", vTabInfo.size());	
	
	if (fp) fclose(fp);
}

//修改行信息
void UpdRow(ST_TabMeta aTabMeta, char *ckeyName, char *cAllName)
{
	char cFmtCol[100]="";
	
	int nCols = aTabMeta.BottomMeta.size();
	
	for(int i = 0; i < nCols; i++)
	{
		ST_ColMeta aColMeta = aTabMeta.BottomMeta[i];
		if ( strncasecmp(aColMeta.cKey, "Y", 1) == 0 )
		{
			strcpy(ckeyName, aColMeta.strColName.c_str() );
		}
		sprintf(cFmtCol,"|| rs.%s", aColMeta.strColName.c_str());
		strcat(cAllName, cFmtCol);
	}
	
}

//修改src信息
void UpdSrcPro(char *pOwn,char *pTab, vector<string> & vctSrcFile, ST_TabMeta aTabMeta)
{
	char cRow19[200] = "";		//CURSOR cur_1 IS select * from cxr.test_cxr where id > 0 order by id;
	char cRow35[1024] = "";	//vs_buf := vs_buf || rs.id ||rs.xxxx;	--行累计
	char cRow39[200] = "";		//skey := rs.id;
	char cRow40[200] = "";		//ekey := rs.id1;"
	char cRow54[200] = "";		//insert into cxr.v3state_test_cxr values(batchno,skey,ekey,smd5,snum);
	char cRow83[200] = "";		//insert into cxr.v3state_test_cxr values(batchno,skey,ekey,smd5,snum);
	char ckeyName[50] = "";	
	char cAllName[1024] = "";	
	
	
	//1.0 get table info
	UpdRow(aTabMeta, ckeyName, cAllName);
	sprintf(cRow19, "	CURSOR cur_1 IS select * from %s.%s order by %s;\n", pOwn,pTab, ckeyName);
	sprintf(cRow35, "		vs_buf := vs_buf %s;\n", cAllName);
	sprintf(cRow39, "			skey := rs.%s;\n", ckeyName);
	sprintf(cRow40, "			ekey := rs.%s;\n", ckeyName);
	sprintf(cRow54, "				insert into v3state_%s_%s values(batchno,skey,ekey,smd5,snum);\n", pOwn,pTab);
	sprintf(cRow83, "		insert into v3state_%s_%s values(batchno,skey,ekey,smd5,snum);\n", pOwn,pTab);//--
	
	
	int nRows = vctSrcFile.size()-1;
	for (int i = 0; i <nRows; i++)
	{
		switch(i+1)	//需要修改的行数,对应到文件	
		{
			case 19:	//CURSOR cur_1 IS select * from cxr.test_cxr where id > 0 order by id;
				vctSrcFile[i] = cRow19;
				break;
			case 35:	//vs_buf := vs_buf || rs.id ;	--行累计
				vctSrcFile[i] = cRow35;
				break;
			case 39:	//skey := rs.id;
				vctSrcFile[i] = cRow39;
				break;
			case 40:	//ekey := rs.id;
				vctSrcFile[i] = cRow40;
				break;
			case 43:	//skey := rs.id;
				vctSrcFile[i] = cRow39;
				break;
			case 44:	//ekey := rs.id;
				vctSrcFile[i] = cRow40;
				break;	
			case 47:	//ekey := rs.id;
				vctSrcFile[i] = cRow40;
				break;		
			case 54:	//--insert into cxr.v3state_test_cxr values(batchno,skey,ekey,smd5,snum); 	
				vctSrcFile[i] = cRow54;	
				break;
			case 62:	//ekey := rs.id;
				vctSrcFile[i] = cRow40;
				break;
			case 83:	//--insert into cxr.v3state_test_cxr values(batchno,skey,ekey,smd5,snum);
				vctSrcFile[i] = cRow83;
				break;
			default:
				//~ printf("No change:%d!\n", i);
				break;
				
		}
	}
	printf("Upd src buf ok!\n");
}

//导入sample
bool ReadSampleFile(const char *pSampleFile, vector<string> & vctSrcFile)
{
	char cLine[1024]="";
	
	FILE * fp = fopen(pSampleFile, "r");
	if(!fp) {
		printf("Cann't open File:%s \n", pSampleFile);
		return false;
	}
	
	while (!feof(fp))	//--注feof()函数在判断最后一行时会，多get一次。用时注意去掉 
	{
		fgets(cLine, 1024, fp);	
		vctSrcFile.push_back(cLine);	
		
	}
	printf("vctInterval.size():%lu\n", vctSrcFile.size());	
	
	if (fp) fclose(fp);
	
	return true;
}

// 生成sql新文件
void WriteFile(char *pNewFile, vector<string>  vctNewFile)
{
	//1.0 open new file
	FILE * fp = fopen(pNewFile, "w");
	if(!fp) {
		printf("Cann't open File:%s \n", pNewFile);
		return ;
	}
	
	//2.0 重写文件
	int nNewRows = vctNewFile.size()-1;	
	for (int i = 0; i <nNewRows; i++)
	{
		fprintf(fp, "%s", vctNewFile[i].c_str() );
	}
	
	//3.0 close
	if (fp) fclose(fp);
	
	printf("Write File:%s\n", pNewFile);
	
}

void GenSrcPro(char *pOwn,char *pTab, ST_TabMeta aTabMeta)
{
	char cSrcFile[300]="";
	vector<string> vctSrcFile;
	
	//1.0 导入sample
	ReadSampleFile(pSrcSample, vctSrcFile);
	
	//2.0 修改src信息
	UpdSrcPro(pOwn,pTab, vctSrcFile,aTabMeta);
	
	//3.0 生成src新文件
	sprintf(cSrcFile, pSrcPro, pOwn,pTab);
	WriteFile(cSrcFile, vctSrcFile);
}

// 修改tgt信息
void UpdTgtPro(char *pOwn,char *pTab, vector<string> & vctTgtFile, ST_TabMeta aTabMeta)
{
	char cRow22[200] = "";		//CURSOR cur_0 IS select * from test.v3state_zf order by batchno;
	char cRow24[200] = "";		//CURSOR cur_1 IS select * from test.zf where col_int >= vs_skey  and col_int <= vs_ekey order by col_int;
	char cRow44[1024] = "";	//vs_buf := vs_buf || rs1.col_int || rs1.col_name || rs1.col_date;
	char cRow79[200] = "";		//update test.v3state_zf set tnum =tmp_tnum, tmd5 = tmp_tmd5, state=tmp_state where batchno = rs0.batchno;
	char ckeyName[50] = "";	
	char cAllName[1024] = "";	
	
	
	//1.0 get table info
	UpdRow(aTabMeta, ckeyName, cAllName);
	sprintf(cRow22, "	CURSOR cur_0 IS select * from v3state_%s_%s order by batchno;\n", pOwn,pTab);
	sprintf(cRow24, "	CURSOR cur_1 IS select * from %s.%s where %s >= vs_skey  and %s <= vs_ekey order by %s;\n", pOwn,pTab,ckeyName,ckeyName,ckeyName);
	sprintf(cRow44, "			vs_buf := vs_buf %s;\n", cAllName);
	sprintf(cRow79, "		update v3state_%s_%s set tnum =tmp_tnum, tmd5 = tmp_tmd5, state=tmp_state where batchno = rs0.batchno;\n", pOwn,pTab);
	
	int nRows = vctTgtFile.size();
	for (int i = 0; i <nRows; i++)
	{
		switch(i+1)	//需要修改的行数,对应到文件	
		{
			case 22:
				vctTgtFile[i] = cRow22;
				break;
			case 24:
				vctTgtFile[i] = cRow24;
				break;
			case 44:
				vctTgtFile[i] = cRow44;
				break;
			case 79:
				vctTgtFile[i] = cRow79;
				break;
			default:
				//~ printf("No change:%d!\n", i);
				break;
				
		}
	}
	printf("Upd tgt buf ok!\n");
}

// TGT： 生成源库的存储过程，pro_v3sample_t.sql
void GenTgtPro(char *pOwn, char *pTab, ST_TabMeta aTabMeta)
{
	char cTgtFile[300]="";
	vector<string> vctTgtFile;
	
	//1.0 导入sample
	ReadSampleFile(pTgtSample, vctTgtFile);
	
	//2.0 修改tgt信息
	UpdTgtPro(pOwn,pTab, vctTgtFile,aTabMeta);
		
	//3.0 生成tgt新文件
	sprintf(cTgtFile, pTgtPro, pOwn,pTab);
	WriteFile(cTgtFile, vctTgtFile);
}

bool genProTab(){
	char cTabPath[100]="./genProTab.txt";
	
	//0.0 获取工作路径 读取表名
	genOwnerAndTbName(cTabPath);
	
	int vsize = vTabInfo.size();
	for(int i = 0; i < vsize; i++)
	{
		char cMetaFile[300]="";
		ST_TabMeta aTabMeta;
		
		//1.0 获取表meta,获取META存入aTabMeta, M_JP.TESTJP.lua
		sprintf(cMetaFile, pMetaDir, vTabInfo[i].cOwn, vTabInfo[i].cTab);
		printf("cMetaFile=%s\n", cMetaFile);
		
		if(!(GetTabMeta(cMetaFile, aTabMeta))){
			printf("GetTabMeta error!\n");
			fprintf(Logfp, "GetTabMeta error!\n");
			
			return false;
		}   
		
		//2.0 生成源库的存储过程，pro_v3sample.sql
		GenSrcPro(vTabInfo[i].cOwn, vTabInfo[i].cTab, aTabMeta);
		
		//3.0 生成目标库的存储过程，pro_v3sample_t.sql
		GenTgtPro(vTabInfo[i].cOwn, vTabInfo[i].cTab, aTabMeta);
	}
	
	return true;
}

bool ScrSql()
{
	int  fd;
	int  r;
	char pSrcDbName[20] = "";			   //源库别名
	char tabname[100] = "";			   	   //状态表名
	char tmp[20] = "normol";				
	char DXusername[20]	= "";  			   //将源表用户名大写
	char buf[100] = "";
	char createtabsql[200] = "";   		   //存储过程命令
	
	strncpy(pSrcDbName, aOraEnv.dbname, strlen(aOraEnv.dbname));
	strncpy(DXusername, aOraEnv.username, strlen(aOraEnv.username));
	strupr(DXusername);	
	sprintf(tabname, "%s_%s", DXusername, aOraEnv.tabname);
	sprintf(buf, "%s.%s\n", aOraEnv.username, aOraEnv.tabname);
	//~ sqlplus %s/%s@%s @./v3Pro/pro_v3%s_%s.sql >/dev/null
	sprintf(createtabsql, scsql, aRedisEnv.sysname, aRedisEnv.syspasswd, pSrcDbName, aOraEnv.username, aOraEnv.tabname);
	
	printf("aOraEnv.tabname:%s\n", aOraEnv.tabname);
	printf("pSrcDbName:%s , aRedisEnv.sysname:%s, aRedisEnv.syspasswd:%s , tabname:%s, tmp:%s\n", 
		pSrcDbName, aRedisEnv.sysname, aRedisEnv.syspasswd, tabname, tmp);
		
		
	if(!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))  {  //初始化客户端环境
		return false;
	}
	aOraEnv.pSrcOraConn = OCI_ConnectionCreate(pSrcDbName, aRedisEnv.sysname, aRedisEnv.syspasswd, strcasecmp(tmp, "SYSDBA")?OCI_SESSION_DEFAULT:OCI_SESSION_SYSDBA); //创建连接
	if(!aOraEnv.pSrcOraConn) 
	{
		printf("ConnSrcDb error !");
		fprintf(Logfp, "ConnSrcDb error !");
		return false;
	}
	printf("ConnSrcDb OK\n");
	fprintf(Logfp,"ConnSrcDb OK\n");
	
	if(!(Detercreatetable(tabname))){		//判断woxin用户下是否有tabname这张表，如果没有则创建一张
		OCI_ConnectionFree (aOraEnv.pSrcOraConn);

		return false;
	}	
	OCI_ConnectionFree(aOraEnv.pSrcOraConn);
	
	
	fd = open("genProTab.txt", O_RDWR | O_CREAT | O_EXCL, 0666);
	if(fd==-1){
		fd = open("genProTab.txt", O_WRONLY | O_TRUNC);
		if(fd==-1){
			printf("error genProTab.txt:%m\n");
			fprintf(Logfp, "error genProTab.txt:%m\n");
			return false;
		}
	}
	do{
		r = write(fd, buf, strlen(buf));
		bzero(buf, sizeof(buf));
	}while(r>0);
	
	if(!(genProTab())){					//生成存储过程
		printf("createsql error!\n");
		fprintf(Logfp, "createsql error!\n");
		
		return false;
	}

	int rc = system(createtabsql);			//执行存储过程命令
	if( rc == -1 ||rc == 127 ) {
		printf("create error:%s\n", createtabsql);
		fprintf(Logfp, "create error:%s\n", createtabsql);
		return false;
	}
	rc = system("exit");              //退出sql环境
	if( rc == -1 ||rc == 127 ) {
		printf("create error:exit\n");
		fprintf(Logfp, "create error:exit\n");
		return false;
	}
	printf("make %s successful\n", createtabsql);
	fprintf(Logfp, "make %s successful\n", createtabsql);
	
	return true;
}

int ExecExit( )
{
	printf("StateM end!\n");
	fprintf(Logfp,"StateM end!\n");
	fclose(Logfp);
	exit(0);
}

void  BMG_S_LongRun()
{
	int nCurState = E_ConnBus;
	while(1) 
	{
		switch(nCurState) 
		{
			case E_ConnBus:    //连接总线
			{
				aRedisEnv.ctx = redisConnect(aRedisEnv.aIP, aRedisEnv.nPort);
				if ( !aRedisEnv.ctx->err)
				{
					printf("Connect OK!\n");
					if(GetMsg())				//获取用户名，密码，表名
						nCurState = E_ScrMeta;
				} else {
					printf("Connect Failed !Sleep(200)!\n");
					aRedisEnv.ctx = 0x00;
					sleep(200);
				}
				break;
			}
			case E_ScrMeta:
			{
				 if(ScrMeta())     //生成lua文件
				{
					printf("make meta ok!!!\n");			
					nCurState = E_ScrSql;
				} 
				else{
					printf("make meta failed,next 3s continue...\r");
					sleep(3);
				} 
				break;
			}	
			case E_ScrSql:	
			{
				getchar();
				if(ScrSql())   	   //生成存储过程
				{
					printf("make ...sql ok!!!\n");
					nCurState = E_Exit;
				}
				else{
					printf("make ...sql failed,next 3s continue...\r");
					sleep(3);
				}
				break;
			}
			case E_Exit:
				ExecExit();
					break;
			default:
				printf("Why in Schedule Default Routine. ErrorAndQuit!");
				exit(-200);
				break;
		}	
	}
}

void ClearGblMem()
{
		//清除连接 
		if (aOraEnv.pSrcOraConn)
			OCI_ConnectionFree(aOraEnv.pSrcOraConn); 
		//清除OCI库
		OCI_Cleanup();
		printf("Menfree\n");
		fprintf(Logfp,"Menfree!\n");
		fclose(Logfp);
}

int main(int argc, char* argv[])
{
	aRedisEnv.nPid = getpid();
	
	//0.0 parameters check
	CheckArgs(argc, argv);
	
	//1.0 初始化BMG_S环境
	BMG_S_Setup(aRedisEnv.pJobName, aRedisEnv.pMotaDir);
	
	//2.0 生成状态表
	BMG_S_LongRun();
	
	//3.0 free 
	ClearGblMem();	
	
	return 0;
}
