//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <iostream>

#include "Math.h"
#include "Matrix.h"
#include "Texture.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render()
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	//RENDER LOGIC
	//for (int px{}; px < m_Width; ++px)
	//{
	//	for (int py{}; py < m_Height; ++py)
	//	{
	//		float gradient = px / static_cast<float>(m_Width);
	//		gradient += py / static_cast<float>(m_Width);
	//		gradient /= 2.0f;

	//		ColorRGB finalColor{ gradient, gradient, gradient };

	//		//Update Color in Buffer
	//		finalColor.MaxToOne();

	//		m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
	//			static_cast<uint8_t>(finalColor.r * 255),
	//			static_cast<uint8_t>(finalColor.g * 255),
	//			static_cast<uint8_t>(finalColor.b * 255));
	//	}
	//}
	Render_W1_Part1();

	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	//Todo > W1 Projection Stage
}

void dae::Renderer::Render_W1_Part1()
{
	//CreateTriangle
	std::vector<Vector3> vertices_ndc
	{
		{0.f,.5f,1.f},
		{.5f,-.5f,1.f},
		{-.5f,-.5f,1.f}
	};

	//Convert Poins To screenSpace (raster space)

	std::vector<Vector3> screenCoor{};
	for(int i{}; i < vertices_ndc.size();i++)
	{
		Vector2 screenCor{};
		screenCor.x = ((vertices_ndc[i].x + 1) / 2) * m_Width;
		screenCor.y = ((1 - vertices_ndc[i].y) / 2) * m_Height;
		screenCoor.push_back(Vector3{ screenCor.x,screenCor.y,1 });


	}

	//1 Triangle
	for(int i{}; i < 1; i++)
	{
		//create the triangle side vectors


		Vector2 a = Vector2{screenCoor[1].x,screenCoor[1].y} - Vector2{ screenCoor[0].x,screenCoor[0].y };
		Vector2 b = Vector2{ screenCoor[2].x,screenCoor[2].y } - Vector2{ screenCoor[0].x,screenCoor[0].y };


		
		//for every vertice
		for(int j{}; j < screenCoor.size();j++)
		{

		
			for (int px{}; px < m_Width; ++px)
			{
				for (int py{}; py < m_Height; ++py)
				{
					float gradient = px / static_cast<float>(m_Width);
					gradient += py / static_cast<float>(m_Width);
					gradient /= 2.0f;

					//make vector from pixel to vertice
					Vector2 pixelPoint{ px - screenCoor[j].x,py - screenCoor[j].y };

					//cross product this vector to both the vectors that the triangle makes
					//if both true color white.



					float signedArea1 = Vector2::Cross(a, pixelPoint);
					float signedArea2 = Vector2::Cross(pixelPoint, b);

					ColorRGB finalColor{};
					if(signedArea1 > 0 && signedArea2 > 0)
					{
						finalColor = ColorRGB{ 1,1,1 };
					}else
					{
						finalColor = ColorRGB{ 0,0,0 };

					}

					///*	ColorRGB finalColor {
					//	gradient, gradient, gradient
					//};*/

						//ColorRGB finalColor{ 0,0,0 };

					//Update Color in Buffer
					finalColor.MaxToOne();

					m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255),
						static_cast<uint8_t>(finalColor.g * 255),
						static_cast<uint8_t>(finalColor.b * 255));
				}
			}
		}

	}

	
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
