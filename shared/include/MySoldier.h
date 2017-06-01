#ifndef MYSOLDIER_INCLUDED
#define MYSOLDIER_INCLUDED

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <Texture.h>
#include <VertexPool.h>

class CMySoldier
	: boost::noncopyable
{
public:
	CMySoldier(void);
	virtual ~CMySoldier(void);
	bool Initialize(void);
	void LoadDrawState(void);
	void UnloadDrawState(void);
	void Draw(void);
	boost::shared_ptr<CVertexPool> GetMesh(void);
	boost::shared_ptr<CTexture> GetTexture(void);
	bool IsValid(void);
protected:
	bool InitializeMesh(void);
	bool InitializeTexture(void);
	void Create(void);
public:
	boost::shared_ptr<CVertexPool> m_mesh;
	boost::shared_ptr<CTexture> m_texture;
};

#endif // MYSOLDIER_INCLUDED

