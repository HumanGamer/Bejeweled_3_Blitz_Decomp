#include <direct.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <list>
#include <errno.h>

#include <PakInterface.h>

#define PAK_ENCRYPTION_KEY 0xF7

namespace Sexy
{
	static std::string StringToUpper(const std::string& theString)
	{
		std::string aString;

		for (unsigned i = 0; i < theString.length(); i++)
			aString += toupper(theString[i]);

		return aString;
	}

	std::string GetFileName(const std::string& thePath, bool noExtension = false)
	{
		int aLastSlash = max((int) thePath.rfind('\\'), (int) thePath.rfind('/'));

		if (noExtension)
		{
			int aLastDot = (int)thePath.rfind('.');
			if (aLastDot > aLastSlash)
				return thePath.substr(aLastSlash + 1, aLastDot - aLastSlash - 1);
		}

		if (aLastSlash == -1)
			return thePath;
		else
			return thePath.substr(aLastSlash + 1);
	}

	std::string GetFileDir(const std::string& thePath, bool withSlash = false)
	{
		// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

		int aLastSlash = max((int) thePath.rfind('\\'), (int) thePath.rfind('/'));

		if (aLastSlash == -1)
			return "";
		else
		{
			if (withSlash)
				return thePath.substr(0, aLastSlash+1);
			else
				return thePath.substr(0, aLastSlash);
		}
	}

	void MkDir(const std::string& theDir)
	{
		std::string aPath = theDir;

		int aCurPos = 0;
		for (;;)
		{
			int aSlashPos = aPath.find_first_of("\\/", aCurPos);
			if (aSlashPos == -1)
			{
				_mkdir(aPath.c_str());
				break;
			}

			aCurPos = aSlashPos+1;

			std::string aCurPath = aPath.substr(0, aSlashPos);
			_mkdir(aCurPath.c_str());
		}
	}
}

using namespace std;
using namespace Sexy;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

bool package = false;
bool unpackage = false;

FILE* gDestFP = nullptr;

// 0xBAC04AC0

class DeferredFile
{
public:
    std::string mFileName;
    int mFileSize;
};

enum
{
    FILEFLAGS_END = 0x80
};

typedef std::list<DeferredFile> DeferredFileList;

DeferredFileList gDeferredFilesList;

int pak_fwrite(const void* theData, size_t theElemSize, size_t theCount, FILE* theFP)
{
	int aSize = theElemSize*theCount;
	char* aBuf = new char[aSize];

	for (int i = 0; i < aSize; i++)
		aBuf[i] = ((char*) theData)[i] ^ PAK_ENCRYPTION_KEY;

	int aCount = fwrite(aBuf, theElemSize, theCount, theFP);
	delete aBuf;
	return aCount;
}

void AddFiles(const std::string& theDir, const std::string& theRelDir)
{
	WIN32_FIND_DATA aFindFileData;

	HANDLE aFindHandle = FindFirstFile((theDir + "*.*").c_str(), &aFindFileData);

	if (aFindHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string aFileStr = aFindFileData.cFileName;
			if ((aFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				std::string aFileName = theDir + aFindFileData.cFileName;
				std::string aRelName = theRelDir + aFindFileData.cFileName;

				FILE* aSrcFP = fopen(aFileName.c_str(), "rb");
				if (aSrcFP == nullptr)
				{
					cerr << "Unable to open source file '" << (theDir + aFindFileData.cFileName).c_str() << "'" << endl;
					exit(4);
				}

				fseek(aSrcFP, 0, SEEK_END);
				int aSrcSize = ftell(aSrcFP);
				fseek(aSrcFP, 0, SEEK_SET);

				uchar aFlags = 0;
				pak_fwrite(&aFlags, 1, 1, gDestFP);

				uchar aNameWidth = (uchar) aRelName.length();
				pak_fwrite(&aNameWidth, 1, 1, gDestFP);
				pak_fwrite(aRelName.c_str(), 1, aNameWidth, gDestFP);

				pak_fwrite(&aSrcSize, sizeof(int), 1, gDestFP);
				pak_fwrite(&aFindFileData.ftLastWriteTime, sizeof(FILETIME), 1, gDestFP);

				DeferredFile aDeferredFile;
				aDeferredFile.mFileName = aFileName;
				aDeferredFile.mFileSize = aSrcSize;

				gDeferredFilesList.push_back(aDeferredFile);

				fclose(aSrcFP);
			}
			else if (aFileStr != "." && aFileStr != "..")
			{
				AddFiles(theDir + aFindFileData.cFileName + "\\", theRelDir + aFindFileData.cFileName + "\\");
			}

		} while (FindNextFile(aFindHandle, &aFindFileData));

		FindClose(aFindHandle);
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		cerr << "Usage: PopPak [/U] [/P] <FileName> <DirPath>" << endl;
		cerr << "  /U    Unpacks pak file to DirPath" << endl;
		cerr << "  /P    Creates pak file from files in DirPath" << endl;
		return 1;
	}

	int anArgPos = 1;

	while (argc >= anArgPos + 1)
	{
		if (argv[anArgPos][0] == '/')
		{
			if (stricmp(argv[anArgPos], "/U") == 0)
				unpackage = true;
			else if (stricmp(argv[anArgPos], "/P") == 0)
				package = true;
			else
			{
				cerr << "Invalid option" << endl;
				return 101;
			}

			anArgPos++;
		}
		else
			break;
	}

	if (argc < anArgPos + 2)
	{
		cerr << "Usage: PopPak [/U] [/P] <FileName> <DirPath>" << endl;
		cerr << "  /U    Unpacks pak file to DirPath" << endl;
		cerr << "  /P    Creates pak file from files in DirPath" << endl;
		return 102;
	}

	if (package && unpackage)
	{
		cerr << "/U and /P may not be specified together." << endl;
		cerr << "Usage: PopPak [/U] [/P] <FileName> <DirPath>" << endl;
		cerr << "  /U    Unpacks pak file to DirPath" << endl;
		cerr << "  /P    Creates pak file from files in DirPath" << endl;
		return 103;
	}

	std::string aPackName = argv[anArgPos++];
	std::string aDirName = argv[anArgPos++];

	if ((aDirName[aDirName.length()-1] != '\\') && (aDirName[aDirName.length()-1] != '/'))
		aDirName += "\\";

	if (package)
	{
		gDestFP = fopen(aPackName.c_str(), "wb");
		if (gDestFP == nullptr)
		{
			cerr << "Unable to create '" << aPackName.c_str() << "'" << endl;
			return 2;
		}

		ulong anId = 0xBAC04AC0;
		pak_fwrite(&anId, sizeof(ulong), 1, gDestFP);

		ulong aVersion = 0;
		pak_fwrite(&aVersion, sizeof(ulong), 1, gDestFP);

		AddFiles(aDirName, "");

		uchar aFileFlags = FILEFLAGS_END;
		pak_fwrite(&aFileFlags, 1, 1, gDestFP);

		if (gDeferredFilesList.size() == 0)
			cout << "Warning: no files!" << endl;

		int aCount = 0;

		DeferredFileList::iterator anItr = gDeferredFilesList.begin();
		while (anItr != gDeferredFilesList.end())
		{
			cout << "Writing: " << ((aCount * 100) / gDeferredFilesList.size()) << "%\r";
			cout.flush();

			DeferredFile* aDeferredFile = &(*anItr);

			FILE* aSrcFP = fopen(aDeferredFile->mFileName.c_str(), "rb");
			if (aSrcFP == nullptr)
			{
				cerr << "Unable to open source file '" << aDeferredFile->mFileName.c_str() << "'" << endl;
				exit(4);
			}

			int aTotalSize = 0;
			while (!feof(aSrcFP))
			{
				uchar aBuffer[4096];
				int aSize = (int) fread(aBuffer, 1, 4096, aSrcFP);

				pak_fwrite(aBuffer, 1, aSize, gDestFP);
				aTotalSize += aSize;
			}

			fclose(aSrcFP);

			if (aTotalSize != aDeferredFile->mFileSize)
			{
				cerr << "Error: File size changed!" << endl;
				return 3;
			}

			++anItr;
			aCount++;
		}

		fclose(gDestFP);
	}

	if (unpackage)
	{
		PakInterface anInterface;
		if (!anInterface.AddPakFile(aPackName))
		{
			cerr << "Pak File: " << aPackName << " was unable to be loaded. Was this created with PopPak?" << endl;
			return 104;
		}

		WIN32_FIND_DATAA wfd;
		HANDLE aHandle = FindFirstFileA((aDirName + "*.*").c_str(), &wfd);
		if (aHandle != INVALID_HANDLE_VALUE)
		{
			cout << "Warning: Directory " << aDirName << " already exists.  Data will be overwritten." << endl;
			FindClose(aHandle);
		}

		MkDir(aDirName);


		int aNumFiles = (int)anInterface.mPakRecordMap.size();
		int aDoneFiles = 0;
		for (PakRecordMap::iterator anItr = anInterface.mPakRecordMap.begin(); anItr != anInterface.mPakRecordMap.end(); ++anItr)
		{
			PakRecord& aRecord = anItr->second;

			if (anItr->first == StringToUpper(GetFileName(aPackName)))
				continue;

			std::string aFileName = aDirName + aRecord.mFileName;
			MkDir(GetFileDir(aFileName));
			FILE* fp = fopen(aFileName.c_str(), "wb");
			if (!fp)
			{
				cerr << "Error: Unable to open " << aFileName << " for writing: " << errno << endl;
				return errno;
			}

			PFILE aFile;
			aFile.mFP = nullptr;
			aFile.mPos = 0;
			aFile.mRecord = &aRecord;

			BYTE aByte;
			while (aFile.mPos < aRecord.mSize && anInterface.FRead(&aByte, 1, 1, &aFile) > 0)
			{
				if (fwrite(&aByte, 1, 1, fp) != 1)
				{
					int err_save = errno; // in case fclose destroys errno
					cerr << "Error: Writing data to " << aFileName << ": " << err_save << endl;
					fclose(fp);
					return err_save;
				}
			}
			if (aFile.mPos < aRecord.mSize)
			{
				cerr << "Error: Reading data from PAK file for " << aFileName << endl;
				fclose(fp);
				return 105;
			}

			fclose(fp);

			aDoneFiles++;
			cout << "Writing: " << ((aDoneFiles * 100) / aNumFiles) << "%\r";
			cout.flush();
		}

		cout << "Writing: 100%\r";
		cout.flush();
	}

	return 0;
}