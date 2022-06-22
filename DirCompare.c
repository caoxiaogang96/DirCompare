#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "dirent.h"  
#include "nsp_log.h"
#include "md5.h"


#define TYPE_DIR  0x4
#define TYPE_FILE 0x8
#define PATH_SIZE 1024

#define ERROR_FILE1_NOT_EXIST -10
#define ERROR_FILE2_NOT_EXIST -11
#define ERROR_TYPE_NOT_SUPPORT -12
#define ERROR_FILE_NOT_SAME -13

#define FILE_SPLIT "/"
#define FILE_LOG_PATH "./different_files.log"

int write_to_logs(const char *paPath1, const char *paPath2, const char *md51, const char*md52, int iRet)
{
	static unsigned int suiCnt = 0;
	FILE *pstFp;
	char acResult[1024];
	
	if((paPath1 == NULL) || (paPath2 == NULL))
	{
		nsp_err(__FUNCTION__, __LINE__, "参数错误", -1);
		return -1;		
	}

	
	if((pstFp = fopen(FILE_LOG_PATH, "a+")) == NULL)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s 文件创建失败", FILE_LOG_PATH);
		return ERROR_FILE1_NOT_EXIST;
	}
	

	switch(iRet)
	{
		case ERROR_FILE1_NOT_EXIST:
			fprintf(pstFp, "[%d] %s  %s %s\n", ++suiCnt, paPath1, paPath2, "第一个文件不存在");
		break;                                                    
		case ERROR_FILE2_NOT_EXIST:                               
			fprintf(pstFp, "[%d] %s  %s %s\n", ++suiCnt, paPath1, paPath2, "第二个文件不存在");
		break;                                                    
		case ERROR_TYPE_NOT_SUPPORT:                              
			fprintf(pstFp, "[%d] %s  %s %s\n", ++suiCnt, paPath1, paPath2, "文件类型不支持");
		break;                                                    
		case ERROR_FILE_NOT_SAME:                                 
			fprintf(pstFp, "[%d] %s  %s %s\n", ++suiCnt, paPath1, paPath2, "两个文件不相同");
		break;
		default:
			return -2;
	}
	fclose(pstFp);
	
	return 0;
}

int compare_file(char *pstFile1, char *pstFile2)
{
	int iRet;
	FILE *pstFp1;
	FILE *pstFp2;
	unsigned char ucChar1;
	unsigned char ucChar2;
	std::string file1Md5;
	std::string file2Md5;
	
	iRet = 0;
	
	//参数检查
	if((pstFile1 ==NULL) || (pstFile2 == NULL))
	{
		nsp_err(__FUNCTION__, __LINE__, "参数错误", -1);
		iRet = 0;
		goto END;
	}
	
	if(access(pstFile1, F_OK) != 0)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s 文件不存在", pstFile1);
		iRet = ERROR_FILE1_NOT_EXIST;
		goto END;
	}
	if(access(pstFile2, F_OK) != 0)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s 文件不存在", pstFile2);
		iRet =  ERROR_FILE2_NOT_EXIST;
		goto END;
	}
	
	file1Md5 = md5file(pstFile1);
	file2Md5 = md5file(pstFile2);
	if(file1Md5 != file2Md5)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s(%s) 和%s(%s) 文件不相同", pstFile1, file1Md5.c_str(), pstFile2, file2Md5.c_str());
		iRet =  ERROR_FILE_NOT_SAME;		
	}
	else
	{
		nsp_log(__FUNCTION__, __LINE__, "%s(%s) 和%s(%s) 文件相同", pstFile1, file1Md5.c_str(), pstFile2, file2Md5.c_str());
		iRet = 0;
	}
	
END:
	if(iRet != 0)
	{
		write_to_logs(pstFile1, pstFile2, file1Md5.c_str(), file2Md5.c_str(), iRet);
	}
	return iRet;

#if	0
	//变量初始化
	iRet   = 0;
	pstFp1 = NULL;
	pstFp2 = NULL;
	memset(&ucChar1,0x0,sizeof(char));
	memset(&ucChar2,0x0,sizeof(char));
	
	//判断第一个文件是否存在
	if((pstFp1 = fopen(pstFile1, "rb")) == NULL)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s 文件不存在", pstFile1);
		iRet = ERROR_FILE1_NOT_EXIST;
	}
	
	//判断第二个文件是否存在
	if((pstFp2 = fopen(pstFile2, "rb")) == NULL)
	{
		nsp_log(__FUNCTION__, __LINE__, "%s 文件不存在", pstFile2);
		fclose(pstFp1);
		iRet = ERROR_FILE2_NOT_EXIST;
	}
	
	while((ucChar1 = (unsigned char)fgetc(pstFp1) != EOF) && (ucChar2 = (unsigned char)fgetc(pstFp2) != EOF))
	{
		printf("%x | %x\n", ucChar1, ucChar2);
		if(ucChar1 != ucChar2)
		{
			nsp_log(__FUNCTION__, __LINE__, "%s 和 %s 内容不相同", pstFile1, pstFile2);
			iRet = ERROR_FILE_NOT_SAME;
		}
	}
	nsp_log(__FUNCTION__, __LINE__, "%s 和 %s 内容相同", pstFile1, pstFile2);
	
END:
	switch(iRet)
	{
		case ERROR_FILE1_NOT_EXIST:
			fclose(pstFp1);
		case ERROR_FILE2_NOT_EXIST:
			fclose(pstFp2);
			break;
		default:
			fclose(pstFp1);
			fclose(pstFp2);		
	}
	return iRet;
#endif

	
}

int compare_Dir(char* pcFileDir1, char* pcFileDir2)
{
	int i;
	DIR *pstDir1;
	struct dirent *pstEntry;
	char *pcFilePath1;
	char *pcFilePath2;
	
	//参数检查
	if((pcFileDir1 ==NULL) || (pcFileDir2 == NULL))
	{
		nsp_err(__FUNCTION__, __LINE__, "参数错误", -1);
		return -1;
	}
	
	//打开第一个路径
	if((pstDir1 = opendir(pcFileDir1))==NULL)  
	{  
		nsp_err(__FUNCTION__, __LINE__, "opendir 失败", ERROR_FILE1_NOT_EXIST);
		return ERROR_FILE1_NOT_EXIST;  
	}
	
	//参数初始化
	i 			= 0;
	pstEntry 	= NULL;
	
	//申请文件名内存
	if(((pcFilePath1 = (char *)malloc(PATH_SIZE)) == NULL) ||
		(pcFilePath2 = (char *)malloc(PATH_SIZE)) == NULL)
	{
		nsp_err(__FUNCTION__, __LINE__, "malloc 失败", -1);
		return -2; 
	}
	
	
	//遍历文件树
	while(pstEntry=readdir(pstDir1))  
	{
		if(strcmp(pstEntry->d_name, ".") == 0 || strcmp(pstEntry->d_name, "..") == 0)
			continue;
		
		//初始化内容为空
		memset(pcFilePath1, 0x00, PATH_SIZE);
		memset(pcFilePath2, 0x00, PATH_SIZE);
		
		//补全完整路径
		strcat(pcFilePath1, pcFileDir1);
		strcat(pcFilePath1, FILE_SPLIT);
		strcat(pcFilePath1, pstEntry->d_name);
		strcat(pcFilePath2, pcFileDir2);
		strcat(pcFilePath2, FILE_SPLIT);
		strcat(pcFilePath2, pstEntry->d_name);
		
		i++;
		//如果是文件，则调用compare_File比较文件内容
		if(pstEntry->d_type == TYPE_FILE)
		{
			nsp_log(__FUNCTION__, __LINE__, "待比较文件%d: %s\n", i, pcFilePath1);
			compare_file(pcFilePath1, pcFilePath2);
		}
		//如果是文件夹，则调用compare_Dir遍历该文件夹
		else if(pstEntry->d_type == TYPE_DIR)
		{
			nsp_log(__FUNCTION__, __LINE__, "待比较文件夹%d: %s VS %s\n", i, pcFilePath1, pcFilePath2);
			compare_Dir(pcFilePath1, pcFilePath2);
		}
		else
		{
			nsp_log(__FUNCTION__, __LINE__, "比较文件%d: %s\n", i, pcFilePath1);
			nsp_err(__FUNCTION__, __LINE__, "该类型不支持", pstEntry->d_type);
			write_to_logs(pcFilePath1, pcFilePath2, NULL, NULL, ERROR_TYPE_NOT_SUPPORT);
		}
	}
	
	//释放文件名内存
	free(pcFilePath1);
	free(pcFilePath2);

	return 0;
}

int main(int argc, char *argv[])
{     
	int iRet;
	int iFileSize = 0;

	if(argc != 4)
	{
		nsp_err(__FUNCTION__, __LINE__, "参数错误", -1);
		nsp_err(__FUNCTION__, __LINE__, "格式为：./HexCompare filePath1 filePath2 0/1[0比较文件，1比较文件夹]", -1);
		return -1;
	}

	nsp_log(__FUNCTION__, __LINE__, "待比较文件夹1: %s\n", argv[1]);
	nsp_log(__FUNCTION__, __LINE__, "待比较文件夹2: %s\n", argv[2]);

	switch(atoi(argv[3]))
	{
		case 0:
			iRet = compare_file(argv[1], argv[2]);
		break;
		case 1:
			iRet = compare_Dir(argv[1], argv[2]);
		break;
	}
	
	return 0;    
}