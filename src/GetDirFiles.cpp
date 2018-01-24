#include "GetDirFiles.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>

//判断是否是".."目录和"."目录
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
		//可能是空字符
		return strDir;
	}

	return strDirBak;
}

//显示删除失败原因
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

//递归搜索目录中文件并删除
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
				//如果是目录，开始递归删除目录中的内容
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
		_findclose(hFile);//关闭打开的文件句柄，并释放关联资源，否则无法删除空目录
	}

	//删除空目录，必须在递归返回前调用_findclose,否则无法删除目录
	if(_rmdir(pathBak.c_str()) == -1)
	{
		showError();//目录非空则会显示出错原因

		return false;
	}

	return true;
}

size_t getDirs(const std::string& path, std::vector<std::string>& dirs, const bool& bIncludeSub/* = true*/)
{
	std::string pathBak = addSlashIfNeeded(path);
	//文件句柄
	intptr_t  hFile = 0;
	//文件信息  
	_finddata_t fileinfo;
	std::string p;

	//查找目录
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
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
	//文件句柄
	intptr_t  hFile = 0;
	//文件信息  
	_finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(pathBak).append(filespec).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果不是目录,加入列表  
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

	//上面只是查找了文件，这里查找目录
	if ((hFile = _findfirst(p.assign(pathBak).append("*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
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

	//读取所有文件
	if (bGetAll)
	{
		vSpecs.push_back("*.*");
	}
	else
	{
		//读取指定格式的文件
		for (size_t i = 0; i < nFSNum; ++i)
		{
			std::string strCurFS = vFileSpecs[i];
			//如果在任意位置包含'*'
			if (strCurFS.find('*') != std::string::npos)
			{
				vSpecs.push_back(strCurFS);
			}
			//如果不包含'*'并且包含字符
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

	//对每一个后缀按照顺序进行读取
	size_t nSpecNum = vSpecs.size();
	for (size_t i = 0; i < nSpecNum; ++i)
	{
		std::string strCurSpec = vSpecs[i];
		getFiles(path, files, strCurSpec.c_str(), bIncludeSub);
	}

	return files.size();
}
