
#include <fstream>

#include <math.h>

#include <ErrorManager.h>
#include <OpenGLFont.h>

#include <GL/glu.h>

/**
*/
COpenGLFont::COpenGLFont(void)
{
	Create();
}

/**
*/
COpenGLFont::~COpenGLFont()
{
	Destroy();
}

bool COpenGLFont::Initialize(const char* font, unsigned int size, bool bold, bool italic)
{
	HDC hDC;
	HFONT hFont,
		hFontOld;
	PAINT_RESULT paintResult;
	DWORD * pBitmapBits;
	BITMAPINFO bmi;
	HBITMAP hbmBitmap;
	HGDIOBJ hbmOld;
	BITMAP bitmap;

	if (glIsTexture(m_textureName) == GL_TRUE)
	{
		MarkError();

		return false;
	}

	m_spacing = size/3;

	m_size = size;

	m_name = font;

	m_isBold = bold;
	m_isItalic = italic;

	hDC = CreateCompatibleDC(NULL);

	if (hDC == NULL)
	{
		MarkError();
	
		return false;
	}

	if (!SetMapMode(hDC, MM_TEXT))
	{
		MarkError();
	
		return false;
	}

	if (!InitializeFont(hDC, &hFont))
	{
		MarkError();
	
		return false;
	}

	hFontOld = (HFONT)SelectObject(hDC, hFont);

	while ((paintResult = SaveAlphabet(hDC)) == MORE_DATA)
	{
		m_width *= 2;
		m_height *= 2;
	}

	if (paintResult != SUCCESS)
	{
		MarkError();
	
		return false;
	}

	// Prepare to create a bitmap.
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));

	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = (int)m_width;
	bmi.bmiHeader.biHeight      = -(int)m_height;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	// Create a bitmap for the font.
	hbmBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, NULL, 0);

	if (hbmBitmap == NULL)
	{
		MarkError();

		return false;
	}
	
	hbmOld = SelectObject(hDC, hbmBitmap);

	// Set text properties.
	SetTextColor(hDC, RGB(255,255,255));
	SetBkColor(hDC, RGB(0,0,0));
	SetTextAlign(hDC, TA_TOP);

	// Paint the alphabet onto the selected bitmap.
	if (SaveAlphabet(hDC) != SUCCESS)
	{
		MarkError();

		return false;
	}
	
	glGenTextures(1, &m_textureName);
	
	GetObject(hbmBitmap, sizeof(BITMAP), &bitmap);
	
	glBindTexture(GL_TEXTURE_2D, m_textureName);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bmBits);

	SelectObject(hDC, hbmOld);
	SelectObject(hDC, hFontOld);
	
	DeleteObject(hbmBitmap);
	DeleteObject(hFont);
	DeleteDC(hDC);

	return true;
}

/**
*/
void COpenGLFont::Draw(const char* text, float xTextOffset, float yTextOffset) const 
{
	float xCharacterOffset,
		yCharacterOffset;
	const char *character;

	xCharacterOffset = xTextOffset;
	yCharacterOffset = yTextOffset - m_size;

	glBlendFunc(GL_SRC_COLOR, GL_ONE);

	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, m_textureName);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	
	for (character=text; *character; ++character)
	{
		if (*character == ' ')
			xCharacterOffset += m_spacing;
		else if (*character == '\n')
		{
			yCharacterOffset -= m_size;
			xCharacterOffset = xTextOffset;
		}	
		else if ((*character - 32) < 0 || (*character - 32) >= 128 - 32)
			MarkError();
		else
			xCharacterOffset += DrawCharacter(*character, xCharacterOffset, yCharacterOffset);		
	}	

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
}

/**
*/
bool COpenGLFont::IsValid(void) const
{
	return (m_textureName != 0);
}

/**
*/
bool COpenGLFont::InitializeFont(HDC hDC, HFONT *pFont)
{
	int characterHeight;
	DWORD fontStyle;
	DWORD isItalic;

	characterHeight = m_size;

	fontStyle = m_isBold ? FW_BOLD : FW_NORMAL;

	isItalic = m_isItalic ? TRUE : FALSE;

	*pFont = CreateFont(characterHeight, 0, 0, 0, fontStyle, isItalic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, (LPCWSTR)m_name.c_str());

	if (*pFont == NULL)
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
COpenGLFont::PAINT_RESULT COpenGLFont::SaveAlphabet(HDC hDC)
{
	SIZE size;
	char str[2] = "x",
		c;
	DWORD xTextOffset,
		yTextOffset;

	// Calculate the spacing between characters based on line height.
	if (!GetTextExtentPoint32(hDC, (LPCWSTR)str, 1, &size))
		return FAIL;

	m_spacing = (int)ceil(size.cy*0.3f);

	// Set the starting point for the drawing.
	xTextOffset = m_spacing;
	yTextOffset = 0;

	// For each character, draw text on the DC and advance the current 
	// position.
	for (c=32; c<127; c++)
	{
		str[0] = c;

		if (!GetTextExtentPoint32(hDC, (LPCWSTR)str, 1, &size))
			return FAIL;

		if ((DWORD)(xTextOffset + size.cx + m_spacing) > m_width)
		{
			xTextOffset = m_spacing;
			yTextOffset += size.cy + 1;
		}

		// Check to see if there's room to write the character here.
		if (yTextOffset + size.cy > m_height)
			return MORE_DATA;

		// Perform the actual drawing.
		if (!ExtTextOut(hDC, xTextOffset + 0, yTextOffset + 0, ETO_OPAQUE, NULL, (LPCWSTR)str, 1, NULL))
			return FAIL;

		m_texCoordArray[c - 32][0] = ((FLOAT)(xTextOffset))/m_width;
		m_texCoordArray[c - 32][1] = ((FLOAT)(yTextOffset))/m_height;
		m_texCoordArray[c - 32][2] = ((FLOAT)(xTextOffset + size.cx))/m_width;
		m_texCoordArray[c - 32][3] = ((FLOAT)(yTextOffset + size.cy))/m_height;

		xTextOffset += size.cx + (2 * m_spacing);
	}

	return SUCCESS;
}

/**
*/
float COpenGLFont::DrawCharacter(int characterName, float xTextOffset, float yTextOffset) const
{
	float sOrg,
		tOrg,
		sDest,
		tDest,
		characterWidth,
		characterHeight;

	sOrg = m_texCoordArray[characterName - 32][0];
	tOrg = m_texCoordArray[characterName - 32][1];
	sDest = m_texCoordArray[characterName - 32][2];
	tDest = m_texCoordArray[characterName - 32][3];

	characterWidth = (sDest - sOrg)*m_width ;
	characterHeight = (tDest - tOrg)*m_height;

	glTexCoord2f(sOrg, tDest);
	glVertex2f(xTextOffset, yTextOffset);

	glTexCoord2f(sDest, tDest);
	glVertex2f(xTextOffset + characterWidth, yTextOffset);

	glTexCoord2f(sDest, tOrg);
	glVertex2f(xTextOffset + characterWidth, yTextOffset + characterHeight);

	glTexCoord2f(sOrg, tOrg);
	glVertex2f(xTextOffset, yTextOffset + characterHeight);

	return characterWidth;
}

/**
*/
void COpenGLFont::Create(void)
{
	// Size of the texture which holds the font characters.
	m_width = 128; 
	m_height = 128; 
	
	m_spacing = 0; 
	
	m_textureName = 0; 

	m_size = 0; 
	
	m_name.clear(); 

	m_isBold = false; 
	m_isItalic = false; 
}

/**
*/
void COpenGLFont::Destroy(void)
{
	if (m_textureName)
		glDeleteTextures(1, &m_textureName);
}

