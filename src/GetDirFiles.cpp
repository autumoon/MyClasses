#include "GetDirFiles.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>

//�ж��Ƿ���".."Ŀ¼��"."Ŀ¼
inline bool isSpecialDir(const char *path)
{
	return strcmp(path, "..") == 0 || strcmp(path, ".") == 0;
}

static inline std::string addSlashIfNeeded(const std::string strDir, const char& cDir = '\\')
{
	std::string strDirBak(strDir);
	if (strDir.length() > 0 && strDir[strDir.length() - 1] != cDir)
	{
		strDirBak += cDir;
	}
	else
	{
		//�����ǿ��ַ�
		return strDir;
	}

	return strDirBak;
}

//��ʾɾ��ʧ��ԭ��
inline void showError(const char *file_name = NULL)
{
	errno_t err;
	_get_errno(&err);
	switch(err)
	{
	case ENOTEMPTY:
		printf("Given path is not a directory, the directory is not empty, or the directory is either the current working directory or the root directory.\n");
		break;
	case ENOENT:
		printf("Path is invalid.\n");
		break;
	case EACCES:
		printf("%s had been opend by some application, can't delete.\n", file_name);
		break;
	}
}

//�ݹ�����Ŀ¼���ļ���ɾ��
bool deleteDir(const std::string& path)
{
	std::string pathBak = addSlashIfNeeded(path);
	_finddata_t fileinfo;
	intptr_t hFile;
	std::string p;
	if((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		while(_findnext(hFile, &fileinfo) == 0)
		{
			if(isSpecialDir(fileinfo.name))
				continue;
			std::string sCurDir = p.assign(pathBak).append(fileinfo.name);
			if( fileinfo.attrib & _A_SUBDIR )
			{
				//�����Ŀ¼����ʼ�ݹ�ɾ��Ŀ¼�е�����
				deleteDir(sCurDir);
			}
			else
			{
				if(remove(sCurDir.c_str()) == -1)
				{
					showError(fileinfo.name);

					return false;
				}

			}
		}
		_findclose(hFile);//�رմ򿪵��ļ���������ͷŹ�����Դ�������޷�ɾ����Ŀ¼
	}

	//ɾ����Ŀ¼�������ڵݹ鷵��ǰ����_findclose,�����޷�ɾ��Ŀ¼
	if(_rmdir(pathBak.c_str()) == -1)
	{
		showError();//Ŀ¼�ǿ������ʾ����ԭ��

		return false;
	}

	return true;
}

size_t getDirs(const std::string& path, std::vector<std::string>& dirs, const bool& bIncludeSub/* = true*/)
{
	std::string pathBak = addSlashIfNeeded(path);
	//�ļ����
	intptr_t  hFile = 0;
	//�ļ���Ϣ  
	_finddata_t fileinfo;
	std::string p;

	//����Ŀ¼
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			if (fileinfo.attrib &  _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 &&
					strcmp(fileinfo.name, "..") != 0 && !(fileinfo.attrib & _A_SYSTEM))
				{
					if (bIncludeSub)
					{
						getDirs(p.assign(pathBak).append(fileinfo.name), dirs, bIncludeSub);
					}
					dirs.push_back(p.assign(pathBak).append(fileinfo.name));
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	return dirs.size();
}

size_t getFiles(const std::string& path, std::vector<std::string>& files, const char *filespec/* = "*.*"*/, const bool& bIncludeSub/* = true*/)
{
	std::string pathBak = addSlashIfNeeded(path);
	//�ļ����
	intptr_t  hFile = 0;
	//�ļ���Ϣ  
	_finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(pathBak).append(filespec).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�������Ŀ¼,�����б�  
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				files.push_back(p.assign(pathBak).append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	if (!bIncludeSub)
	{
		return files.size();
	}

	//����ֻ�ǲ������ļ����������Ŀ¼
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			if (fileinfo.attrib &  _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 &&
					strcmp(fileinfo.name, "..") != 0 && !(fileinfo.attrib & _A_SYSTEM))
					getFiles(p.assign(pathBak).append(fileinfo.name), files, filespec, true);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	return files.size();
}

size_t getFiles(const std::string& path, std::vector<std::string>& files, const std::vector<std::string>& vFileSpecs, const bool& bIncludeSub /*= true*/)
{
	std::vector<std::string> vSpecs;
	bool bGetAll = false;
	size_t nFSNum = vFileSpecs.size();

	for (size_t i = 0; i < nFSNum; ++i)
	{
		std::string strCurSpe = vFileSpecs[i];
		if (strCurSpe == "*.*" || strCurSpe == "*")
		{
			bGetAll = true;
			break;
		}
	}

	//��ȡ�����ļ�
	if (bGetAll)
	{
		vSpecs.push_back("*.*");
	}
	else
	{
		//��ȡָ����ʽ���ļ�
		for (size_t i = 0; i < nFSNum; ++i)
		{
			std::string strCurFS = vFileSpecs[i];
			//���������λ�ð���'*'
			if (strCurFS.find('*') != std::string::npos)
			{
				vSpecs.push_back(strCurFS);
			}
			//���������'*'���Ұ����ַ�
			else if (strCurFS.length() > 0)
			{
				if (strCurFS[0] == '.')
				{
					vSpecs.push_back('*' + strCurFS);
				}
				else
				{
					vSpecs.push_back("*." + strCurFS);
				}
			}
		}
	}

	//��ÿһ����׺����˳����ж�ȡ
	size_t nSpecNum = vSpecs.size();
	for (size_t i = 0; i < nSpecNum; ++i)
	{
		std::string strCurSpec = vSpecs[i];
		getFiles(path, files, strCurSpec.c_str(), bIncludeSub);
	}

	return files.size();
}
