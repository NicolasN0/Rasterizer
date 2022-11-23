#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)
		SDL_Surface* surface = IMG_Load(path.c_str());
		return new Texture{surface};
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv
	
		int width = uv.x * m_pSurface->w;
		int height = uv.y * m_pSurface->h;
	
		SDL_Color finalColor;

	

				SDL_GetRGB(m_pSurfacePixels[width + height * m_pSurface->w],m_pSurface->format,
				&finalColor.r,
				&finalColor.g,
				&finalColor.b);

			ColorRGB color{ (float)finalColor.r/255,(float)finalColor.g/255,(float)finalColor.b/255 };
		return (color);

	
		
	}
}