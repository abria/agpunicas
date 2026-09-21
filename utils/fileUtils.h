// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <string>
#include <sys/stat.h>
#include <algorithm>

#ifdef _WIN32
#include <ctime>
#include <direct.h>
#include <windows.h>
#else
#include <fnmatch.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#endif

#include "stringUtils.h"

namespace agp
{
	inline std::vector<std::string> getFilesInDirectory(const std::string& directory, const std::string& filter = "*.*")
	{
		std::vector<std::string> files;

#ifdef _WIN32
		std::string searchPath = directory + "/" + filter;
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(searchPath.c_str(), &fd);

		if (hFind != INVALID_HANDLE_VALUE) {
			do
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					files.push_back(fd.cFileName);
		
			} while (FindNextFile(hFind, &fd));

			FindClose(hFind);
		}
#else
		DIR* dir = opendir(directory.c_str());

		if (dir) 
		{
			struct dirent* entry;

			while ((entry = readdir(dir)) != nullptr) 
			{
				if (entry->d_type == DT_REG) 
				{
					// Use fnmatch to apply the filter
					if (fnmatch(filter.c_str(), entry->d_name, 0) == 0)
					{
						files.push_back(entry->d_name);
					}
				}
			}
			closedir(dir);
		}
#endif
		std::sort(files.begin(), files.end());
		return files;
	}

	
	// returns file extension, if any (otherwise returns "")
	inline std::string getFileExtension(const std::string& FileName)
	{
		if(FileName.find_last_of(".") != std::string::npos)
			return FileName.substr(FileName.find_last_of(".")+1);
		return "";
	}

	// extracts the filename from the given path and stores it into <filename>
	inline std::string getFileName(std::string const & path, bool save_ext = true)
	{
		std::string filename = path;

		// Remove directory if present.
		// Do this before extension removal in case directory has a period character.
		const size_t last_slash_idx = filename.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
			filename.erase(0, last_slash_idx + 1);

		// Remove extension if present.
		if(!save_ext)
		{
			const size_t period_idx = filename.rfind('.');
			if (std::string::npos != period_idx)
				filename.erase(period_idx);
		}

		return filename;
	}

	// changes directory by moving one directory up from the current directory
	inline std::string cdUp(std::string const & path)
	{
		return path.substr(0, path.find_last_of("/\\"));
	}

	// change extension to file
	inline std::string changeExtension(const std::string & filepath, const std::string & new_ext)
	{
		return cdUp(filepath) + "/" + getFileName(filepath, false) + "." + new_ext;
	}

	// returns true if the given path is a directory
	inline bool isDirectory(std::string path)
	{
		struct stat s;
		if( stat(path.c_str(),&s) == 0 )
		{
			if( s.st_mode & S_IFDIR )
				return true;
			else if( s.st_mode & S_IFREG )
				return false;
			else 
				return false;
		}
		else 
			return false;
	}

	// returns true if the given path is a file
	inline bool isFile(std::string path)
	{
		struct stat s;
		if( stat(path.c_str(),&s) == 0 )
		{
			if( s.st_mode & S_IFDIR )
				return false;
			else if( s.st_mode & S_IFREG )
				return true;
			else 
				return false;
		}
		else 
			return false;
	}

	// make dir
#ifdef _WIN32
#include <errno.h>
	inline bool makeDir(const char* arg){
		//        printf("Creating directory \"%s\" ...", arg);
		bool done = _mkdir(arg) == 0;
		bool result = done || errno != ENOENT;
		//        printf("%s\n", result? "DONE!" : "ERROR!");
		return result;
	}
#else
	inline bool makeDir(const char* arg){
		//        printf("Creating directory \"%s\" ...", arg);
		bool done = mkdir(arg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
		bool result = done || errno == EEXIST;
		//        printf("%s\n", result? "DONE!" : "ERROR!");
		return result;
	}
#endif

	// check-and-makedir
	inline bool check_and_make_dir(const std::string & dirname){
		if(isDirectory(dirname))
			return true;
		else
			return makeDir(dirname.c_str());
	}

	inline void remove_folder(const std::string & path, bool verbose = false) 
	{
		if(verbose)
			printf("Removing folder \"%s\"...", path.c_str());
		if(!isDirectory(path))
		{
			if(verbose)
				printf("DOES NOT EXIST -> skipped\n");
			return;
		}
#ifdef __linux__
		system((std::string("rm -rf ") + path).c_str());
#else
		throw ("Unimplemented remove_folder() function for the current platform");
#endif
		if(isDirectory(path))
			throw (strprintf("Failed to remove folder \"%s\"", path.c_str()));
		else if(verbose)
			printf("DONE!\n");
	}

	inline void rename_file(const std::string & old_path, const std::string & new_path, bool verbose = false) 
	{
		if(verbose)
			printf("Renaming file \"%s\" to \"%s\"...", old_path.c_str(), new_path.c_str());
		if(!isFile(old_path))
		{
			if(verbose)
				printf("PATH DOES NOT EXIST -> skipped\n");
			return;
		}
		else if(old_path.compare(new_path) == 0)
		{
			if(verbose)
				printf("PATH MATCH -> skipped\n");
			return;
		}
#ifdef __linux__
		system((std::string("mv -f ") + old_path + " " + new_path).c_str());
#else
		throw ("Unimplemented rename() function for the current platform");
#endif
		if(!isFile(new_path))
			throw (strprintf("Failed to rename file \"%s\"", old_path.c_str()));
		else if(verbose)
			printf("DONE!\n");
	}

#ifdef _WIN32
	inline FILE* pipe_open(const char *command, const char *type){return _popen(command, type);}
	inline int pipe_close(FILE *stream){return _pclose(stream);}
#else
	inline FILE* pipe_open(const char *command, const char *type){return popen(command, type);}
	inline int pipe_close(FILE *stream){return pclose(stream);}
#endif
}