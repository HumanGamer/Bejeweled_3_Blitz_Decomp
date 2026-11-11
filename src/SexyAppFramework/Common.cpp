#include "Common.h"

#include "Debug.h"

HINSTANCE Sexy::gHInstance;

std::string Sexy::vformat(const char* fmt, va_list argPtr)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

    // We draw the line at a 1MB string.
    const int maxSize = 1000000;

    // If the string is less than 161 characters,
    // allocate it on the stack because this saves
    // the malloc/free time.
    const int bufSize = 161;
	char stackBuffer[bufSize];

    int attemptedSize = bufSize - 1;

	int numChars = 0;
#ifdef _WIN32
	numChars = _vsnprintf(stackBuffer, attemptedSize, fmt, argPtr);
#else
	numChars = vsnprintf(stackBuffer, attemptedSize, fmt, argPtr);
#endif

	//cout << "NumChars: " << numChars << endl;

    if ((numChars >= 0) && (numChars <= attemptedSize))
	{
		// Needed for case of 160-character printf thing
		stackBuffer[numChars] = '\0';

        // Got it on the first try.
		return std::string(stackBuffer);
    }

    // Now use the heap.
    char* heapBuffer = NULL;

    while (((numChars == -1) || (numChars > attemptedSize)) &&
		(attemptedSize < maxSize))
	{
        // Try a bigger size
        attemptedSize *= 2;
		heapBuffer = (char*)realloc(heapBuffer, (attemptedSize + 1));
#ifdef _WIN32
		numChars = _vsnprintf(heapBuffer, attemptedSize, fmt, argPtr);
#else
		numChars = vsnprintf(heapBuffer, attemptedSize, fmt, argPtr);
#endif
    }

	heapBuffer[numChars] = 0;

	std::string result = std::string(heapBuffer);

    free(heapBuffer);
    return result;
}

//overloaded StrFormat: should only be used by the xml strings
std::string Sexy::StrFormat(const char* fmt ...)
{
    va_list argList;
    va_start(argList, fmt);
	std::string result = vformat(fmt, argList);
    va_end(argList);

    return result;
}

std::wstring Sexy::vformat(const wchar_t* fmt, va_list argPtr)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

    // We draw the line at a 1MB string.
    const int maxSize = 1000000;

    // If the string is less than 161 characters,
    // allocate it on the stack because this saves
    // the malloc/free time.
    const int bufSize = 161;
	wchar_t stackBuffer[bufSize];

    int attemptedSize = bufSize - 1;

	int numChars = 0;
#ifdef _WIN32
	numChars = _vsnwprintf(stackBuffer, attemptedSize, fmt, argPtr);
#else
	numChars = vsnwprintf(stackBuffer, attemptedSize, fmt, argPtr);
#endif

	//cout << "NumChars: " << numChars << endl;

    if ((numChars >= 0) && (numChars <= attemptedSize))
	{
		// Needed for case of 160-character printf thing
		stackBuffer[numChars] = '\0';

        // Got it on the first try.
		return std::wstring(stackBuffer);
    }

    // Now use the heap.
	wchar_t* heapBuffer = NULL;

    while (((numChars == -1) || (numChars > attemptedSize)) &&
		(attemptedSize < maxSize))
	{
        // Try a bigger size
        attemptedSize *= 2;
		heapBuffer = (wchar_t*)realloc(heapBuffer, (attemptedSize + 1));
#ifdef _WIN32
		numChars = _vsnwprintf(heapBuffer, attemptedSize, fmt, argPtr);
#else
		numChars = vsnwprintf(heapBuffer, attemptedSize, fmt, argPtr);
#endif
    }

	heapBuffer[numChars] = 0;

	std::wstring result = std::wstring(heapBuffer);

    free(heapBuffer);

    return result;
}

//overloaded StrFormat: should only be used by the xml strings
std::wstring Sexy::StrFormat(const wchar_t* fmt ...)
{
    va_list argList;
    va_start(argList, fmt);
	std::wstring result = vformat(fmt, argList);
    va_end(argList);

    return result;
}

std::wstring Sexy::StringToWString(const std::string &theString)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	std::wstring aString;
	aString.reserve(theString.length());
	for(size_t i = 0; i < theString.length(); ++i)
		aString += (unsigned char)theString[i];
	return aString;
}

std::string Sexy::WStringToString(const std::wstring &theString)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	size_t aRequiredLength = wcstombs( NULL, theString.c_str(), 0 );
	if (aRequiredLength < 16384)
	{
		char aBuffer[16384];
		wcstombs( aBuffer, theString.c_str(), 16384 );
		return std::string(aBuffer);
	}
	else
	{
		DBG_ASSERTE(aRequiredLength != (size_t)-1);
		if (aRequiredLength == (size_t)-1) return "";

		char* aBuffer = new char[aRequiredLength+1];
		wcstombs( aBuffer, theString.c_str(), aRequiredLength+1 );
		std::string aStr = aBuffer;
		delete[] aBuffer;
		return aStr;
	}
}

SexyString Sexy::StringToSexyString(const std::string& theString)
{
#ifdef _USE_WIDE_STRING
	return StringToWString(theString);
#else
	return SexyString(theString);
#endif
}

SexyString Sexy::WStringToSexyString(const std::wstring &theString)
{
#ifdef _USE_WIDE_STRING
	return SexyString(theString);
#else
	return WStringToString(theString);
#endif
}

std::string Sexy::SexyStringToString(const SexyString& theString)
{
#ifdef _USE_WIDE_STRING
	return WStringToString(theString);
#else
	return std::string(theString);
#endif
}

std::wstring Sexy::SexyStringToWString(const SexyString& theString)
{
#ifdef _USE_WIDE_STRING
	return std::wstring(theString);
#else
	return StringToWString(theString);
#endif
}


SexyString Sexy::ToSexyString(const std::string& theString)
{
	return StringToSexyString(theString);
}

SexyString Sexy::ToSexyString(const std::wstring& theString)
{
	return WStringToSexyString(theString);
}

bool Sexy::StringToInt(const std::string theString, int* theIntVal)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	*theIntVal = 0;

	if (theString.length() == 0)
		return false;

	int theRadix = 10;
	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (; i < theString.length(); i++)
	{
		char aChar = theString[i];

		if ((theRadix == 10) && (aChar >= '0') && (aChar <= '9'))
			*theIntVal = (*theIntVal * 10) + (aChar - '0');
		else if ((theRadix == 0x10) &&
			(((aChar >= '0') && (aChar <= '9')) ||
			 ((aChar >= 'A') && (aChar <= 'F')) ||
			 ((aChar >= 'a') && (aChar <= 'f'))))
		{
			if (aChar <= '9')
				*theIntVal = (*theIntVal * 0x10) + (aChar - '0');
			else if (aChar <= 'F')
				*theIntVal = (*theIntVal * 0x10) + (aChar - 'A') + 0x0A;
			else
				*theIntVal = (*theIntVal * 0x10) + (aChar - 'a') + 0x0A;
		}
		else if (((aChar == 'x') || (aChar == 'X')) && (i == 1) && (*theIntVal == 0))
		{
			theRadix = 0x10;
		}
		else
		{
			*theIntVal = 0;
			return false;
		}
	}

	if (isNeg)
		*theIntVal = -*theIntVal;

	return true;
}

bool Sexy::StringToInt(const std::wstring theString, int* theIntVal)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	*theIntVal = 0;

	if (theString.length() == 0)
		return false;

	int theRadix = 10;
	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (; i < theString.length(); i++)
	{
		wchar_t aChar = theString[i];

		if ((theRadix == 10) && (aChar >= L'0') && (aChar <= L'9'))
			*theIntVal = (*theIntVal * 10) + (aChar - L'0');
		else if ((theRadix == 0x10) &&
			(((aChar >= L'0') && (aChar <= L'9')) ||
			 ((aChar >= L'A') && (aChar <= L'F')) ||
			 ((aChar >= L'a') && (aChar <= L'f'))))
		{
			if (aChar <= L'9')
				*theIntVal = (*theIntVal * 0x10) + (aChar - L'0');
			else if (aChar <= L'F')
				*theIntVal = (*theIntVal * 0x10) + (aChar - L'A') + 0x0A;
			else
				*theIntVal = (*theIntVal * 0x10) + (aChar - L'a') + 0x0A;
		}
		else if (((aChar == L'x') || (aChar == L'X')) && (i == 1) && (*theIntVal == 0))
		{
			theRadix = 0x10;
		}
		else
		{
			*theIntVal = 0;
			return false;
		}
	}

	if (isNeg)
		*theIntVal = -*theIntVal;

	return true;
}

bool Sexy::StringToDouble(const std::string theString, double* theDoubleVal)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	*theDoubleVal = 0.0;

	if (theString.length() == 0)
		return false;

	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (; i < theString.length(); i++)
	{
		char aChar = theString[i];

		if ((aChar >= '0') && (aChar <= '9'))
			*theDoubleVal = (*theDoubleVal * 10) + (aChar - '0');
		else if (aChar == '.')
		{
			i++;
			break;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}
	}

	double aMult = 0.1;
	for (; i < theString.length(); i++)
	{
		char aChar = theString[i];

		if ((aChar >= '0') && (aChar <= '9'))
		{
			*theDoubleVal += (aChar - '0') * aMult;
			aMult /= 10.0;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}
	}

	if (isNeg)
		*theDoubleVal = -*theDoubleVal;

	return true;
}

bool Sexy::StringToDouble(const std::wstring theString, double* theDoubleVal)
{
	// TODO: Copied from PopcapFramework 1.3, needs to be verified against decomp

	*theDoubleVal = 0.0;

	if (theString.length() == 0)
		return false;

	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (; i < theString.length(); i++)
	{
		wchar_t aChar = theString[i];

		if ((aChar >= L'0') && (aChar <= L'9'))
			*theDoubleVal = (*theDoubleVal * 10) + (aChar - L'0');
		else if (aChar == L'.')
		{
			i++;
			break;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}
	}

	double aMult = 0.1;
	for (; i < theString.length(); i++)
	{
		wchar_t aChar = theString[i];

		if ((aChar >= L'0') && (aChar <= L'9'))
		{
			*theDoubleVal += (aChar - L'0') * aMult;
			aMult /= 10.0;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}
	}

	if (isNeg)
		*theDoubleVal = -*theDoubleVal;

	return true;
}

std::string Sexy::GetFileDir(const std::string& thePath, bool withSlash)
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