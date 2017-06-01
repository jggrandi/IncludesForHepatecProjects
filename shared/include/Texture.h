#ifndef TEXTURE_INCLUDED
#define TEXTURE_INCLUDED

#include <vector>

#include <boost/utility.hpp>

class CTexture
	: boost::noncopyable
{
public:
	enum TARGET {
		TEXTURE_2D = 0x0DE1,
		TEXTURE_RECTANGLE = 0X84F5,
		TEXTURE_3D = 0x806f,
	};
	enum INTERNAL_FORMAT {
		RED = 0x1903,
		ALPHA = 0x1906,
		DEPTH_COMPONENT = 0x1902,
		LUMINANCE = 0x1909,
		RGB = 0x1907,
		RGBA = 0x1908,
		RGB16F = 0x881B,
		RGBA16F = 0x881A,
		RGBA32F = 0x8814
	};
	enum TYPE {
		FLOAT = 0x1406,
		INT = 0x1404,
		SHORT = 0x1402,
		UNSIGNED_BYTE = 0x1401,
		UNSIGNED_INT = 0x1405,
		UNSIGNED_SHORT = 0x1403
	};
	enum PARAMETER {
		CLAMP = 0x2900,
		CLAMP_TO_EDGE = 0x812f,
		LINEAR = 0x2601,
		LINEAR_MIPMAP_LINEAR = 0x2703,
		MAG_FILTER = 0x2800,
		MIN_FILTER = 0x2801,
		NEAREST = 0x2600,
		REPEAT = 0x2901,
		MIRRORED_REPEAT = 0x8370,
		WRAP_S = 0x2802,
		WRAP_T = 0x2803
	};
	CTexture(void);
	virtual ~CTexture(void);
	bool Initialize(unsigned int target, int internalFormat);
	bool SetImage(int width, int height, int border, unsigned int format, unsigned int type, const void *pixels);
	bool SetImage(int width, int height, int depth, int border, unsigned int format, unsigned int type, const void *pixels);
	bool SetSubImage(int xOffset, int yOffset, int width, int height, unsigned int format, unsigned int type, const void *pixels);
	bool SetSubImage(int xOffset, int yOffset, int zOffset, int width, int height, int depth, unsigned int format, unsigned int type, const void *pixels);
	bool BuildMipmaps(void);
	void SetParameter(unsigned int name, int value);
	void SetParameter(unsigned int name, float value);
	void SetParameter(unsigned int name, const int *value);
	void SetParameter(unsigned int name, const float *value);
	static void SetActiveTextureUnit(unsigned int unit);
	static void DisableTextureUnit(unsigned int unit);
	void Bind(unsigned int unit = 0);
	int GetName(void) const;
	int GetWidth(void) const;
	int GetHeight(void) const;
	int GetDepth(void) const;
	unsigned int GetTarget(void) const;
	unsigned int GetInternalFormat(void) const;
	bool IsValid(void);
private:
	void GuardedBind(void);
	void GuardedUnbind(void);
	void InitializeDefaultContext(void);
	void Create(void);
	void Destroy(void);
protected:
	unsigned int m_name;
	unsigned int m_lastTexture;
	unsigned int m_target;
	unsigned int m_internalFormat;
	unsigned int m_binding;
	int m_width;
	int m_height;
	int m_depth;
	static std::vector<unsigned int> m_textureUnitArray;
};

#endif // TEXTURE_INCLUDED

