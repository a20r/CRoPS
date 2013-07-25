#ifndef ABETARE__GTEXTURE_HPP_
#define ABETARE__GTEXTURE_HPP_

#include <cstdio>

namespace Abetare
{
    class GTexture
    {
    public:
	GTexture(void);	

	virtual ~GTexture(void);
	
	unsigned int GetId(void) const
	{
	    return m_id;
	}
	
	unsigned char* GetImage(void) const
	{
	    return m_image;
	}
	void SetFileName(const char fname[]);
	void ReadBMP(FILE *in);
	void ReadPPM(FILE *in);


	static void ManualCoords(void);	
	static void AutomaticCoords(void);
	
	void Use(void);
	
    protected:    
	unsigned int   m_id;
	unsigned char *m_image;	
	int            m_sizeX;
	int            m_sizeY;
	char          *m_fname;
	bool           m_rgba;	
    };
}

#endif









