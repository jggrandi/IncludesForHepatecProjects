#ifndef ERRORMANAGER_INCLUDED
#define ERRORMANAGER_INCLUDED

#include <vector>
#include <string>

#define MarkError() CErrorManager::Instance().PushError(__FILE__, __LINE__)
#define ReportError(message) CErrorManager::Instance().PushError(__FILE__, __LINE__, message)

class CErrorManager
{
public:
	static CErrorManager& Instance(void)
	{
		if (!m_instance)
		{
			// Check for dead reference.
			if (m_isDestroyed)
				OnDeadReference();
			else
				// First call—initialize.
				Create();
		}

		return *m_instance;
	}
	void PushError(char *file, int line);
	void PushError(char *file, int line, const char *message);
	void PushError(const char *message);
	void PopError(void) const;
	void FlushErrors(void) const;
	bool IsOk(void) const;
private:
	CErrorManager(void);
	CErrorManager(const CErrorManager&);
	CErrorManager& operator=(const CErrorManager&);
	virtual ~CErrorManager(void);
	// Create a new CErrorManager and store a pointer to it in m_instance.
	static void Create(void);
	// Gets called if dead reference detected
	static void OnDeadReference(void);
	std::string FormatError(char *file, int line, const char *message) const;
	std::string FormatError(const char *message) const;
protected:
	static std::vector<std::string> m_errorStack;
	static CErrorManager* m_instance;
	static bool m_isDestroyed;
};

#endif // ERRORMANAGER_INCLUDED

