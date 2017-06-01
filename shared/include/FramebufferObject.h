#ifndef FRAMEBUFFEROBJECT_INCLUDED
#define FRAMEBUFFEROBJECT_INCLUDED

#include <vector>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <Texture.h>

class CFramebufferObject
	: boost::noncopyable
{
public:
	enum ATTACHMENT_TYPE {
		COLOR_ATTACHMENT0 = 0x8CE0,
		COLOR_ATTACHMENT1 = 0x8CE1,
		COLOR_ATTACHMENT2 = 0x8CE2,
		COLOR_ATTACHMENT3 = 0x8CE3,
		COLOR_ATTACHMENT4 = 0x8CE4,
		COLOR_ATTACHMENT5 = 0x8CE5,
		COLOR_ATTACHMENT6 = 0x8CE6,
		COLOR_ATTACHMENT7 = 0x8CE7,
		COLOR_ATTACHMENT8 = 0x8CE8,
		DEPTH_ATTACHMENT = 0x8D00
	};
	CFramebufferObject(void);
	virtual ~CFramebufferObject(void);
	bool Initialize(void);
	bool AttachTexture(ATTACHMENT_TYPE attachment, boost::shared_ptr<CTexture> texture, int level = 0, int slice = 0);
	bool AttachRenderbuffer(ATTACHMENT_TYPE attachment, unsigned int name);
	void Bind(void);
	static void Disable(void);
	unsigned int GetWidth(void) const;
	unsigned int GetHeight(void) const;
	bool IsValid(void);
	// (test purposes only).
	bool GetPixel(unsigned int x, unsigned int y, float *pixel);
private:
	void GuardedBind(void);
	void GuardedUnbind(void) const;
	bool AttachTextureND(unsigned int attachment, unsigned int target, unsigned int id, int level, int slice);
	void Create(void);
	void Destroy(void);
protected:
	unsigned int m_name;
	unsigned int m_width;
	unsigned int m_height;
	std::vector<unsigned int> m_drawBufferArray;
	int m_lastFramebuffer;
	// Current render context parameters, which are restored in response to 
	// 'Disable' calls.
	static int m_defaultDrawBuffer;
	static int m_defaultFramebufferViewport[4];
};

#endif // FRAMEBUFFEROBJECT_INCLUDED

