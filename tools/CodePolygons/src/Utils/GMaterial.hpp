#ifndef ABETARE__GMATERIAL_HPP_
#define ABETARE__GMATERIAL_HPP_

namespace Abetare
{
    class GMaterial
    {
    public:
	GMaterial(void)
	{
	    SetPearl();
	    
	}
	
	virtual ~GMaterial(void)
	{
	}
	
	enum Property
	    {
		AMBIENT     = 0,
		DIFFUSE     = 1,
		SPECULAR    = 2,
		EMMISSIVE   = 3,
		SHININESS   = 4
	    };
	
	virtual const float* GetProperty(const Property type) const
	{
	    return m_prop[type];
	}
	
	virtual void SetProperty(const Property type, const float * const prop)
	{
	    SetProperty(type, prop[0], prop[1], prop[2], prop[3]);
	}
	
	virtual void SetProperty(const Property type,
				 const float a = 0.0f,
				 const float b = 0.0f,
				 const float c = 0.0f,
				 const float d = 1.0f)
	{
	    m_prop[type][0] = a;
	    m_prop[type][1] = b;
	    m_prop[type][2] = c;
	    m_prop[type][3] = d;
	}
	
	void SetAmbient(const float a, const float b, const float c, const float d = 1.0f)
	{
	    SetProperty(AMBIENT, a, b, c, d);
	}
	void SetDiffuse(const float a, const float b, const float c, const float d = 1.0f)
	{
	    SetProperty(DIFFUSE, a, b, c, d);
	}
	void SetSpecular(const float a, const float b, const float c, const float d = 1.0f)
	{
	    SetProperty(SPECULAR, a, b, c, d);
	}
	void SetEmmissive(const float a, const float b, const float c, const float d = 1.0f)
	{
	    SetProperty(EMMISSIVE, a, b, c, d);
	}
	void SetShininess(const float a)
	{
	    SetProperty(SHININESS, a);
	}
	
	void Copy(const GMaterial * const gMat)
	{
	    SetProperty(AMBIENT,   gMat->GetProperty(AMBIENT));
	    SetProperty(DIFFUSE,   gMat->GetProperty(DIFFUSE));
	    SetProperty(SPECULAR,  gMat->GetProperty(SPECULAR));
	    SetProperty(EMMISSIVE, gMat->GetProperty(EMMISSIVE));
	    SetProperty(SHININESS, gMat->GetProperty(SHININESS));
	}
	
	
	void SetGold(void)
	{
	    SetAmbient(0.24725, 	0.1995, 	0.0745);
	    SetDiffuse(0.75164, 	0.60648, 	0.22648);
	    SetShininess(20.4);
	    SetEmmissive(0, 0, 0, 1);
	    SetSpecular(1, 1, 1, 1);
	    

	}
	
	void SetPearl(void)
	{
	    SetAmbient(0.25, 	0.20725, 	0.20725);
	    SetDiffuse(1, 	0.829, 	0.829);
	    SetShininess(18.088);
	    SetEmmissive(0, 0, 0, 1);
	    SetSpecular(1, 1, 1, 1);
	    

	}
	

    protected:
	float m_prop[5][4];

	
    };
}


#endif
    
    






