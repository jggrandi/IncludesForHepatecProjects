#ifndef OPENGLFONT_INCLUDED
#define OPENGLFONT_INCLUDED

#ifndef _WIN32
#error Current COpenGLFont only works under windows.
#endif // _WIN32

#include <string> 

#include <windows.h> 

#include <boost/utility.hpp>

class COpenGLFont 
	: boost::noncopyable
{ 
	enum PAINT_RESULT { 
		FAIL, 
		MORE_DATA, 
		SUCCESS, 
	}; 
public: 
	COpenGLFont(void); 
	~COpenGLFont(void); 
	bool Initialize(const char *font, unsigned int size, bool bold, bool italic); 
	void Draw(const char *text, float xTextOffset, float yTextOffset) const; 
	bool IsValid(void) const;
private: 
	bool InitializeFont(HDC hDC, HFONT *pFont); 
	PAINT_RESULT SaveAlphabet(HDC hDC); 
	float DrawCharacter(int characterName, float xTextOffset, float yTextOffset) const; 
	void Create(void);
	void Destroy(void);
protected:
	// Size of the texture which holds the font characters.
	unsigned int m_width; 
	unsigned int m_height; 
	unsigned int m_spacing; 
	unsigned int m_textureName; 
	unsigned int m_size; 
	std::string m_name; 
	float m_texCoordArray[128-32][4]; 
	bool m_isBold; 
	bool m_isItalic; 
}; 

#endif // OPENGLFONT_INCLUDED

