#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <iostream>
#include <stdexcept>
#include <vector>

#include <ErrorManager.h>

using namespace std;

vector<string> CErrorManager::m_errorStack = vector<string>();
CErrorManager* CErrorManager::m_instance = 0;
bool CErrorManager::m_isDestroyed = false;

/**
*/
void CErrorManager::PushError(char *file, int line)
{
	if (m_errorStack.size() < 100)
		m_errorStack.push_back(FormatError(file, line, "An assertion failure has occurred."));
}

/**
*/
void CErrorManager::PushError(char *file, int line, const char *message)
{
	if (m_errorStack.size() < 100 && message)
	{
		string entireMessage(message);
		vector<string> partialMessageArray;
		string::size_type last,
			current;

		// The last element of the string.
		last = entireMessage.size();
		
		// The beginning of the current message.
		current = entireMessage.rfind('\n');
		
		// Walk backward through the string.
		while (current != string::npos)
		{
			// Push each message into the vector. Current is incremented 
			// before copying to avoid copying the delimiter.
			partialMessageArray.push_back(entireMessage.substr(++current, last - current));
			
			// Back over the delimiter we just found, and set last to the end 
			// of the next message.
			current -= 2;
			last = current;
			
			// Find the next delimiter
			current = entireMessage.rfind('\n', current);
		}
		
		// Pick up the first message - it's not preceded by a delimiter.
		partialMessageArray.push_back(entireMessage.substr(0, last - current));

		while (!partialMessageArray.empty())
		{
			if (!partialMessageArray.back().empty())
				m_errorStack.push_back(FormatError(file, line, partialMessageArray.back().c_str()));

			partialMessageArray.pop_back();
		}
	}
}

/**
*/
void CErrorManager::PushError(const char *message)
{
	if (m_errorStack.size() < 100 && message)
	{
		string entireMessage(message);
		vector<string> partialMessageArray;
		string::size_type last,
			current;

		// The last element of the string.
		last = entireMessage.size();
		
		// The beginning of the current message.
		current = entireMessage.rfind('\n');
		
		// Walk backward through the string.
		while (current != string::npos)
		{
			// Push each message into the vector. Current is incremented 
			// before copying to avoid copying the delimiter.
			partialMessageArray.push_back(entireMessage.substr(++current, last - current));
			
			// Back over the delimiter we just found, and set last to the end 
			// of the next message.
			current -= 2;
			last = current;
			
			// Find the next delimiter
			current = entireMessage.rfind('\n', current);
		}
		
		// Pick up the first message - it's not preceded by a delimiter.
		partialMessageArray.push_back(entireMessage.substr(0, last - current));

		while (!partialMessageArray.empty())
		{
			if (!partialMessageArray.back().empty())
				m_errorStack.push_back(FormatError(partialMessageArray.back().c_str()));

			partialMessageArray.pop_back();
		}
	}
}

/**
*/
void CErrorManager::PopError(void) const
{
//#ifdef _WIN32
	//MessageBox(0, (LPCWSTR)m_errorStack.back().c_str(), (LPCWSTR)"Error", MB_OK);
//#else // !_WIN32
	cout << m_errorStack.back().c_str() << endl;
//#endif // _WIN32

	m_errorStack.pop_back();
}

/**
*/
void CErrorManager::FlushErrors(void) const
{
	int errorCount;

	errorCount = 0;

	while (!m_errorStack.empty())
	{
		PopError();

		errorCount++;
	}

	if (errorCount)
		cout << errorCount << " error(s)" << endl;
}

/**
*/
bool CErrorManager::IsOk(void) const
{
	return m_errorStack.empty();
}

/**
*/
CErrorManager::CErrorManager(void)
{
	m_errorStack.clear();
}

/**
*/
CErrorManager::~CErrorManager(void)
{
	m_instance = 0;
	m_isDestroyed = true;
}

/**
*/
void CErrorManager::Create(void)
{
	static CErrorManager theInstance;

	m_errorStack.clear();

	m_instance = &theInstance;
}

/**
*/
void CErrorManager::OnDeadReference(void)
{
	throw runtime_error("Dead Reference Detected.");
}

/**
*/
std::string CErrorManager::FormatError(char *file, int line, const char *message) const
{
	char lineString[64];
	string errorString;

#ifdef _WIN32
	sprintf_s(lineString, 64, "%i", line);
#else // !_WIN32
	sprintf(lineString, "%i", line);
#endif // _WIN32

	errorString = file;
	errorString += "(";
	errorString += lineString;
	errorString += ")";
	errorString += " : error: ";

	if (message)
		errorString += message;
	else 
		errorString += "(invalid error message)";

	return errorString;
}

/**
*/
std::string CErrorManager::FormatError(const char *message) const
{
	string errorString;

	errorString = "error: ";

	if (message)
		errorString += message;
	else 
		errorString += "(invalid error message)";

	return errorString;
}

