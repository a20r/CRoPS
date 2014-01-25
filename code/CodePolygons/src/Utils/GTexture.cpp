#include "Utils/GTexture.hpp"
#include <cstdlib>
#include <cstring>
#include <OpenGL/glu.h>
#include <png.h>

namespace Abetare
{
    GTexture::GTexture(void)
    {
	m_id    = 0;
	m_image = NULL;
	m_fname = NULL;
	m_sizeX = 0;
	m_sizeY = 0;
    }

    GTexture::~GTexture(void)
    {
	if(m_fname)
	    free(m_fname);
	if(m_image)
	{
	    free(m_image);
	    glDeleteTextures(1, &m_id);
	}
    }

    void GTexture::ReadPPM(FILE *in)
    {
	int i,d;
	char head[70];

	fgets(head, 70, in);
	if(strncmp(head, "P6", 2))
	{
	    fprintf(stderr, "not a raw PPM file\n");
	    return;
	}

	i = 0;
	while(i < 3)
	{
	    fgets(head, 70, in);
	    if (head[0] == '#')     /* skip comments. */
		continue;
	    if (i == 0)
		i += sscanf(head, "%d %d %d", &m_sizeX, &m_sizeY, &d);
	    else if (i == 1)
		i += sscanf(head, "%d %d", &m_sizeY, &d);
	    else if (i == 2)
		i += sscanf(head, "%d", &d);
	}

	m_image = (unsigned char*)malloc(sizeof(unsigned char) * m_sizeX * m_sizeY * 3);
	fread(m_image, sizeof(unsigned char), m_sizeX * m_sizeY * 3, in);
    }

    void GTexture::ReadBMP(FILE * in)
    {
	unsigned long sizeX;
	unsigned long sizeY;
	unsigned long size;                 // size of the image in bytes.
	unsigned long i;                    // standard counter.
	unsigned short int planes;          // number of planes in image (must be 1)
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // used to convert bgr to rgb color.

	// Skip to bmp header
	fseek(in, 18, SEEK_CUR);

	if ((i = fread(&sizeX, 4, 1, in)) != 1)
	    return;
	if((i = fread(&sizeY, 4, 1, in)) != 1)
	    return;
	size = sizeX * sizeY * 3;

	if ((fread(&planes, 2, 1, in)) != 1)
	    return;
	if (planes != 1)
	    return;

	if((i = fread(&bpp, 2, 1, in)) != 1)
	    return;
	if (bpp != 24)
	    return;

	// seek past the rest of the bitmap header
	fseek(in, 24, SEEK_CUR);

	// Read the data
	m_image = (unsigned char *) malloc(size);

	if((i = fread(m_image, size, 1, in)) != 1)
	    return;

	// reverse all of the colours bgr => rgb)
	for(i=0; i < size; i +=3)
	{
	    temp           = m_image[i];
	    m_image[i]     = m_image[i + 2];
	    m_image[i + 2] = temp;
	}

	m_sizeX = sizeX;
	m_sizeY = sizeY;
    }

    void GTexture::SetFileName(const char fname[])
    {
	if(m_fname != fname)
	{
	    if(m_fname)
		free(m_fname);
	    m_fname = strdup(fname);
	}
    }

    void GTexture::ManualCoords(void)
    {
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
    }

    void GTexture::AutomaticCoords(void)
    {
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	GLfloat s_params[4] = {1.0f, 0.0f, 0.0f, 0.0f};
	GLfloat t_params[4] = {0.0f, -1.0f, 1.0f, 0.0f};
	glTexGenfv(GL_S, GL_OBJECT_PLANE, s_params);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, t_params);

  }


    void GTexture::Use(void)
    {
	if(m_fname == NULL || m_image != NULL)
	{
	    glBindTexture(GL_TEXTURE_2D, m_id);
	    return;
	}

	m_id    = 0;

	const int length = strlen(m_fname);
	if(length >= 4)
	{
	    if(strcmp(&m_fname[length - 4], ".ppm") == 0)
	    {
		FILE *in = fopen(m_fname, "rb");
		if(in)
		{
		    m_rgba = false;
		    ReadPPM(in);
		    fclose(in);
		}
	    }
	    else if(strcmp(&m_fname[length - 4], ".bmp") == 0)
	    {
		FILE *in = fopen(m_fname, "r");
		if(in)
		{
		    m_rgba = false;
		    ReadBMP(in);
		    fclose(in);
		}
	    }
	}

	if(m_image)
	{
	    glGenTextures(1, &m_id);
	    glBindTexture(GL_TEXTURE_2D, m_id);

	    if(m_rgba)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_sizeX, m_sizeY, GL_RGBA, GL_UNSIGNED_BYTE, m_image);
	    else
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_sizeX, m_sizeY, GL_RGB, GL_UNSIGNED_BYTE, m_image);

	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

    }
}

