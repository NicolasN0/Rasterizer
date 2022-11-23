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

	m_pDepthBufferPixels = new float[m_Width * m_Height];
	m_pTexture = Texture::LoadFromFile("Resources/uv_grid_2.png");
	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });

	int size{ m_Width * m_Height };
	m_ColorBuffer = new ColorRGB[size];
	for (int i{ 0 }; i < size; i++)
	{
		m_ColorBuffer[i] = colors::Gray;
	}
	m_pDepthBufferPixels = new float[size];
	for (int i{ 0 }; i < size; i++)
	{
		m_pDepthBufferPixels[i] = FLT_MAX;
	}
}

Renderer::~Renderer()
{
	delete[] m_pDepthBufferPixels;
	delete m_pTexture;
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
	//ResetBuffers
	int size{ m_Width * m_Height };
	m_ColorBuffer = new ColorRGB[size];
	for (int i{ 0 }; i < size; i++)
	{
		m_ColorBuffer[i] = colors::Gray;
	}
	m_pDepthBufferPixels = new float[size];
	for (int i{ 0 }; i < size; i++)
	{
		m_pDepthBufferPixels[i] = FLT_MAX;
	}
	//Render_W1_Part1();
	//Render_W1_Part2();
	//Render_W1_Part3();
	//Render_W1_Part4();
	//Render_W2_Part1();
	Render_W2_Part2();
	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	//Todo > W1 Projection Stage


	//std::vector<Vertex> vertices_in;

	//WorldSpace to vector of NDC Space (or directlcy to screen?)
	vertices_out.clear();
	float aspectRatio = { m_Width / static_cast<float>(m_Height) };
	m_Camera.viewMatrix;

	for(int i{};i < vertices_in.size();i++)
	{
		Vertex pWorld{};
		Vertex p{};
		//To view Space
		//Multiply all the vertices with the m_camera.viewMatrix(which is actualy the onb inverse)
		pWorld = vertices_in[i];
		Vertex pView{};
		pView.position = m_Camera.viewMatrix.TransformPoint(pWorld.position);
		//Perspective Divide
		p.position.x = pView.position.x / pView.position.z;
		p.position.y = pView.position.y / pView.position.z;
		p.position.z = pView.position.z;
		//Camera settings
		p.position.x = p.position.x / (aspectRatio * m_Camera.fov);
		p.position.y = p.position.y / m_Camera.fov;
		//toScreenSpace(NDC)
		Vertex pScreen{};
		pScreen.position.x = ((p.position.x + 1) / 2) * m_Width;
		pScreen.position.y = ((1 - p.position.y) / 2) * m_Height;
		pScreen.position.z = p.position.z;
		pScreen.color = vertices_in[i].color;
		pScreen.uv = vertices_in[i].uv;
		vertices_out.push_back(pScreen);
	}

}

void dae::Renderer::VertexTransformationFunction(const std::vector<Mesh>& mesh, std::vector<Vertex>& vertices_out) const
{
	std::vector<Vertex> vertices;
	for(int i{};  i < mesh.size();i++)
	{
		for(int j{}; j < mesh[i].vertices.size(); j++)
		{
			vertices.push_back(mesh[i].vertices[j]);
		}
	}
	//vertices = mesh.vertices;
	VertexTransformationFunction(vertices, vertices_out);
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
		Vector2 b = Vector2{ screenCoor[2].x,screenCoor[2].y } - Vector2{ screenCoor[1].x,screenCoor[1].y };
		Vector2 c = Vector2{ screenCoor[0].x,screenCoor[0].y } - Vector2{ screenCoor[2].x,screenCoor[2].y };

		Vector2 triangleV1 = { screenCoor[0].x,screenCoor[0].y };
		Vector2 triangleV2 = { screenCoor[1].x,screenCoor[1].y };
		Vector2 triangleV3 = { screenCoor[2].x,screenCoor[2].y };

		
		
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
					//Vector2 pixelPoint{ px - screenCoor[j].x,py - screenCoor[j].y };
					//test with middle
					//Vector2 pixelPoint{ px - center.x,py - center.y };


					//cross product this vector to both the vectors that the triangle makes
					//if both true color white.

					Vector2 p{ float(px),float(py) };

					

					Vector2 pointToSide = p - triangleV1;

					float signedArea1 = Vector2::Cross(a, pointToSide);

					pointToSide = p - triangleV2;
					float signedArea2 = Vector2::Cross(b, pointToSide);

					pointToSide = p - triangleV3;
					float signedArea3 = Vector2::Cross(c, pointToSide);


					//Raytraycer

					/*Vector3 edgeA = triangle.v1 - triangle.v0;
					Vector3 pointToSide = p - triangle.v0;
					if (Vector3::Dot(normal, Vector3::Cross(edgeA, pointToSide)) < 0)
					{
						return false;
					}

					Vector3 edgeB = triangle.v2 - triangle.v1;
					pointToSide = p - triangle.v1;
					if (Vector3::Dot(normal, Vector3::Cross(edgeB, pointToSide)) < 0)
					{
						return false;
					}

					Vector3 edgeC = triangle.v0 - triangle.v2;
					pointToSide = p - triangle.v2;
					if (Vector3::Dot(normal, Vector3::Cross(edgeC, pointToSide)) < 0)
					{
						return false;
					}*/

					ColorRGB finalColor{};
					if(signedArea1 > 0 && signedArea2 > 0 && signedArea3 > 0)
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

void dae::Renderer::Render_W1_Part2()
{
	
	//CreateTriangle
	std::vector<Vertex> vertices_ndc
	{
		{{0.f,2.f,0.f},ColorRGB{1,1,1}},
		{{1.f,0.f,0.f},ColorRGB{1,1,1}},
		{{-1.f,0.f,0.f},ColorRGB{1,1,1}}
	};

	//Convert Poins To screenSpace (raster space)
	std::vector<Vertex> newTriangle;
	VertexTransformationFunction(vertices_ndc, newTriangle);


	for (int i{}; i < 1; i++)
	{
		//create the triangle side vectors


		Vector2 a = Vector2{ newTriangle[1].position.x,newTriangle[1].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 b = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 c = Vector2{ newTriangle[0].position.x,newTriangle[0].position.y } - Vector2{ newTriangle[2].position.x,newTriangle[2].position.y };

		Vector2 triangleV1 = { newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 triangleV2 = { newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 triangleV3 = { newTriangle[2].position.x,newTriangle[2].position.y };



	
			for (int px{}; px < m_Width; ++px)
			{
				for (int py{}; py < m_Height; ++py)
				{
					float gradient = px / static_cast<float>(m_Width);
					gradient += py / static_cast<float>(m_Width);
					gradient /= 2.0f;


					Vector2 p{ float(px),float(py) };



					Vector2 pointToSide = p - triangleV1;

					float signedArea1 = Vector2::Cross(a, pointToSide);

					pointToSide = p - triangleV2;
					float signedArea2 = Vector2::Cross(b, pointToSide);

					pointToSide = p - triangleV3;
					float signedArea3 = Vector2::Cross(c, pointToSide);


					ColorRGB finalColor{};
					if (signedArea1 > 0 && signedArea2 > 0 && signedArea3 > 0)
					{
						finalColor = ColorRGB{ 1,1,1 };
					}
					else
					{
						finalColor = ColorRGB{ 0,0,0 };

					}


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

void dae::Renderer::Render_W1_Part3()
{

	//CreateTriangle
	std::vector<Vertex> vertices_ndc
	{
		{{0.f,4.f,2.f},ColorRGB{1,0,0}},
		{{3.f,-2.f,2.f},ColorRGB{0,1,0}},
		{{-3.f,-2.f,2.f},ColorRGB{0,0,1}}
	};

	//Convert Poins To screenSpace (raster space)
	std::vector<Vertex> newTriangle;
	VertexTransformationFunction(vertices_ndc, newTriangle);

	for (int i{}; i < 1; i++)
	{
		//create the triangle side vectors


		Vector2 a = Vector2{ newTriangle[1].position.x,newTriangle[1].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 b = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 c = Vector2{ newTriangle[0].position.x,newTriangle[0].position.y } - Vector2{ newTriangle[2].position.x,newTriangle[2].position.y };

		Vector2 triangleV1 = { newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 triangleV2 = { newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 triangleV3 = { newTriangle[2].position.x,newTriangle[2].position.y };

		float W0;
		float W1;
		float W2;

		Vector2 edge = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		float totalArea = Vector2::Cross(a, edge);

		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{
				float gradient = px / static_cast<float>(m_Width);
				gradient += py / static_cast<float>(m_Width);
				gradient /= 2.0f;


				Vector2 p{ float(px),float(py) };



				Vector2 pointToSide{ p - triangleV2 };
				float signedArea1{ Vector2::Cross(b, pointToSide) };

				pointToSide = p - triangleV3;
				float signedArea2{ Vector2::Cross(c, pointToSide) };

				pointToSide = p - triangleV1;
				float signedArea3{ Vector2::Cross(a, pointToSide) };


				float W1 = signedArea1 / totalArea;
				float W2 = signedArea2 / totalArea;
				float W3 = signedArea3 / totalArea;
			

				ColorRGB finalColor{};
				if (signedArea1 > 0 && signedArea2 > 0 && signedArea3 > 0)
				{
					//float total = signedArea1 + signedArea2 + signedArea3;

					//std::cout << total << std::endl;
					ColorRGB interpolated{ newTriangle[0].color * W1 + newTriangle[1].color * W2 + newTriangle[2].color * W3 };
					finalColor = interpolated;
					//finalColor = ColorRGB{ 1,1,1 };
				}
				else
				{
					finalColor = ColorRGB{ 0,0,0 };

				}


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

void dae::Renderer::Render_W1_Part4()
{
	const int size{ m_Width * m_Height };
	std::vector<std::vector<Vertex>> triangles;
	//CreateTriangle
	std::vector<Vertex> vertices_ndcSmall
	{
		{{0.f,2.f,0.f},ColorRGB{1,1,1}},
		{{1.f,0.f,0.f},ColorRGB{1,1,1}},
		{{-1.f,0.f,0.f},ColorRGB{1,1,1}}
	};
	std::vector<Vertex> vertices_ndc
	{
		{{0.f,4.f,2.f},ColorRGB{1,0,0}},
		{{3.f,-2.f,2.f},ColorRGB{0,1,0}},
		{{-3.f,-2.f,2.f},ColorRGB{0,0,1}}
	};

	triangles.push_back(vertices_ndcSmall);
	triangles.push_back(vertices_ndc);
	//Make depthBuffer and fill with max value floates
	std::vector<float> depthBuffer;
	for(int i{};i < m_Width* m_Height;i++)
	{
		depthBuffer.push_back(FLT_MAX);
	}
	//ColorBuffer? No idea how else;
	std::vector<ColorRGB> ColorBuffer;
	for (int i{}; i < m_Width * m_Height; i++)
	{
		ColorBuffer.push_back(colors::Black);
	}

	//loop over all triangles
	for (int i{}; i < triangles.size(); i++)
	{
		//Convert Poins To screenSpace (raster space)
		std::vector<Vertex> newTriangle;
		VertexTransformationFunction(triangles[i], newTriangle);
		//create the triangle side vectors


		Vector2 a = Vector2{ newTriangle[1].position.x,newTriangle[1].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 b = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 c = Vector2{ newTriangle[0].position.x,newTriangle[0].position.y } - Vector2{ newTriangle[2].position.x,newTriangle[2].position.y };

		Vector2 triangleV1 = { newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 triangleV2 = { newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 triangleV3 = { newTriangle[2].position.x,newTriangle[2].position.y };

		float W0;
		float W1;
		float W2;

		Vector2 edge = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		float totalArea = Vector2::Cross(a, edge);

		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{
				float gradient = px / static_cast<float>(m_Width);
				gradient += py / static_cast<float>(m_Width);
				gradient /= 2.0f;


				Vector2 p{ float(px),float(py) };




				Vector2 pointToSide{ p - triangleV2 };
				float signedArea1{ Vector2::Cross(b, pointToSide) };

				pointToSide = p - triangleV3;
				float signedArea2{ Vector2::Cross(c, pointToSide) };

				pointToSide = p - triangleV1;
				float signedArea3{ Vector2::Cross(a, pointToSide) };

				float W1 = signedArea1 / totalArea;
				float W2 = signedArea2 / totalArea;
				float W3 = signedArea3 / totalArea;
				

				int curPixel = px + (py*m_Width);
				ColorRGB finalColor{};
				if (signedArea1 > 0 && signedArea2 > 0 && signedArea3 > 0)
				{
					//depthTest
					//float interpolatedDepth{ newTriangle[0].position.z * W1 + newTriangle[1].position.z * W2 + newTriangle[2].position.z * W3 };
					float interpolatedDepth{ 1/newTriangle[0].position.z * W1 + 1/newTriangle[1].position.z * W2 + 1/newTriangle[2].position.z * W3 };
					float depth = 1 / interpolatedDepth;

					
					if(depth < depthBuffer[curPixel])
					{
						depthBuffer[curPixel] = interpolatedDepth;

						ColorRGB interpolated{ newTriangle[0].color * W1 + newTriangle[1].color * W2 + newTriangle[2].color * W3 };
						ColorBuffer[curPixel] = interpolated;


					}
					
					
				}
			
				finalColor = ColorBuffer[curPixel];

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

void dae::Renderer::Render_W2_Part1()
{

//	DefineMesh Strip
	std::vector<Mesh> meshes_world
	{
		Mesh{
			{
					Vertex{{-3.f,3.f,-2.f},{1,1,1},{0,0}},
					Vertex{{0.f,3.f,-2.f},{1,1,1},{0.5f,0}},
					Vertex{{3.f,3.f,-2.f},{1,1,1},{1,0}},
					Vertex{{-3.f,0.f,-2.f},{1,1,1},{0,0.5f}},
					Vertex{{0.f,0.f,-2.f},{1,1,1},{0.5f,0.5f}},
					Vertex{{3.f,0.f,-2.f},{1,1,1},{1,0.5f}},
					Vertex{{-3.f,-3.f,-2.f},{1,1,1},{0,1}},
					Vertex{{0.f,-3.f,-2.f},{1,1,1},{0.5f,1}},
					Vertex{{3.f,-3.f,-2.f},{1,1,1},{1,1}},
						},
{
			3,0,4,1,5,2,
			2,6,
			6,3,7,4,8,5
			},
		PrimitiveTopology::TriangleStrip
			}
	};

//	//DefineMesh List
	/*std::vector<Mesh> meshes_world
	{
		Mesh{
			{
					Vertex{{-3.f,3.f,-2.f}},
					Vertex{{0.f,3.f,-2.f}},
					Vertex{{3.f,3.f,-2.f}},
					Vertex{{-3.f,0.f,-2.f}},
					Vertex{{0.f,0.f,-2.f}},
					Vertex{{3.f,0.f,-2.f}},
					Vertex{{-3.f,-3.f,-2.f}},
					Vertex{{0.f,-3.f,-2.f}},
					Vertex{{3.f,-3.f,-2.f}},
						},
{
			3,0,1, 1,4,3, 4,1,2,
			2,5,4, 6,3,4, 4,7,6,
			7,4,5, 5,8,7
			},
		PrimitiveTopology::TriangeList
			}
	};*/


	for(int meshIter{}; meshIter < meshes_world.size();meshIter++)
	{
		Mesh& mesh{ meshes_world[meshIter] };
		switch(mesh.primitiveTopology)
		{
		case PrimitiveTopology::TriangeList:
			for(int indiceI{}; indiceI < mesh.indices.size(); indiceI+=3)
			{
				int ind1{ int(mesh.indices[indiceI]) };
				int ind2{ int(mesh.indices[indiceI+1]) };
				int ind3{ int(mesh.indices[indiceI+2]) };
				std::vector<Vertex> triangle{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };

				std::vector<Vertex> totalVertices;
				VertexTransformationFunction(triangle, totalVertices);
				
				RenderTriangle(totalVertices);
				
			}
			break;
		case PrimitiveTopology::TriangleStrip:
			for(int indiceI{}; indiceI < mesh.indices.size();indiceI++)
			{
				int ind1{};
				int ind2{};
				int ind3{};
				if(indiceI == mesh.indices.size()/2)
				{
					ind1 = int(mesh.indices[indiceI + 1]) ;
					ind2 = int(mesh.indices[indiceI + 2]) ;
					ind3 = int(mesh.indices[indiceI + 3]) ;
				} else if(indiceI < mesh.indices.size() -2)
				{
					ind1 = int(mesh.indices[indiceI ]);
					ind2 = int(mesh.indices[indiceI + 1]);
					ind3 = int(mesh.indices[indiceI + 2]);
				}
				std::vector<Vertex> triangle{};
				if(indiceI % 2 == 0)
				{
					triangle = std::vector<Vertex>{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };
				} else
				{
					triangle = std::vector<Vertex>{ mesh.vertices[ind1],mesh.vertices[ind3],mesh.vertices[ind2] };

				}

				//std::vector<Vertex> triangle{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };

				std::vector<Vertex> totalVertices;
				VertexTransformationFunction(triangle, totalVertices);

				RenderTriangle(totalVertices);
			}
			break;
		}
	}
	//Convert Poins To screenSpace (raster space)
	


	
	
}

void dae::Renderer::Render_W2_Part2()
{
	//DefineMesh Strip
		std::vector<Mesh> meshes_world
	{
		Mesh{
			{
					Vertex{{-3.f,3.f,-2.f},{1,1,1},{0,0}},
					Vertex{{0.f,3.f,-2.f},{1,1,1},{0.5f,0}},
					Vertex{{3.f,3.f,-2.f},{1,1,1},{1,0}},
					Vertex{{-3.f,0.f,-2.f},{1,1,1},{0,0.5f}},
					Vertex{{0.f,0.f,-2.f},{1,1,1},{0.5f,0.5f}},
					Vertex{{3.f,0.f,-2.f},{1,1,1},{1,0.5f}},
					Vertex{{-3.f,-3.f,-2.f},{1,1,1},{0,1}},
					Vertex{{0.f,-3.f,-2.f},{1,1,1},{0.5f,1}},
					Vertex{{3.f,-3.f,-2.f},{1,1,1},{1,1}},
						},
{
			3,0,4,1,5,2,
			2,6,
			6,3,7,4,8,5
			},
		PrimitiveTopology::TriangleStrip
			}
	};

	//	//DefineMesh List
		/*std::vector<Mesh> meshes_world
		{
			Mesh{
				{
						Vertex{{-3.f,3.f,-2.f}},
						Vertex{{0.f,3.f,-2.f}},
						Vertex{{3.f,3.f,-2.f}},
						Vertex{{-3.f,0.f,-2.f}},
						Vertex{{0.f,0.f,-2.f}},
						Vertex{{3.f,0.f,-2.f}},
						Vertex{{-3.f,-3.f,-2.f}},
						Vertex{{0.f,-3.f,-2.f}},
						Vertex{{3.f,-3.f,-2.f}},
							},
	{
				3,0,1, 1,4,3, 4,1,2,
				2,5,4, 6,3,4, 4,7,6,
				7,4,5, 5,8,7
				},
			PrimitiveTopology::TriangeList
				}
		};*/


	for (int meshIter{}; meshIter < meshes_world.size(); meshIter++)
	{
		Mesh& mesh{ meshes_world[meshIter] };
		switch (mesh.primitiveTopology)
		{
		case PrimitiveTopology::TriangeList:
			for (int indiceI{}; indiceI < mesh.indices.size(); indiceI += 3)
			{
				int ind1{ int(mesh.indices[indiceI]) };
				int ind2{ int(mesh.indices[indiceI + 1]) };
				int ind3{ int(mesh.indices[indiceI + 2]) };
				std::vector<Vertex> triangle{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };

				std::vector<Vertex> totalVertices;
				VertexTransformationFunction(triangle, totalVertices);

				RenderTriangle(totalVertices);

			}
			break;
		case PrimitiveTopology::TriangleStrip:
			for (int indiceI{}; indiceI < mesh.indices.size(); indiceI++)
			{
				int ind1{};
				int ind2{};
				int ind3{};
				if (indiceI == mesh.indices.size() / 2)
				{
					ind1 = int(mesh.indices[indiceI + 1]);
					ind2 = int(mesh.indices[indiceI + 2]);
					ind3 = int(mesh.indices[indiceI + 3]);
				}
				else if (indiceI < mesh.indices.size() - 2)
				{
					ind1 = int(mesh.indices[indiceI]);
					ind2 = int(mesh.indices[indiceI + 1]);
					ind3 = int(mesh.indices[indiceI + 2]);
				}
				std::vector<Vertex> triangle{};
				if (indiceI % 2 == 0)
				{
					triangle = std::vector<Vertex>{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };
				}
				else
				{
					triangle = std::vector<Vertex>{ mesh.vertices[ind1],mesh.vertices[ind3],mesh.vertices[ind2] };

				}

				//std::vector<Vertex> triangle{ mesh.vertices[ind1],mesh.vertices[ind2],mesh.vertices[ind3] };

				std::vector<Vertex> totalVertices;
				VertexTransformationFunction(triangle, totalVertices);

				RenderTriangle(totalVertices);
			}
			break;
		}
	}
	//Convert Poins To screenSpace (raster space)




}

float dae::Renderer::ComputeDepth(Vertex v0, Vertex v1,int curPixelX)
{
	float weight = (curPixelX - v0.position.x) / (v1.position.x - v0.position.x);
	//float zInverse = 1/
	return 0.0f;
}

void dae::Renderer::RenderTriangle(std::vector<Vertex> newTriangle)
{


		Vector2 a = Vector2{ newTriangle[1].position.x,newTriangle[1].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 b = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 c = Vector2{ newTriangle[0].position.x,newTriangle[0].position.y } - Vector2{ newTriangle[2].position.x,newTriangle[2].position.y };

		Vector2 triangleV1 = { newTriangle[0].position.x,newTriangle[0].position.y };
		Vector2 triangleV2 = { newTriangle[1].position.x,newTriangle[1].position.y };
		Vector2 triangleV3 = { newTriangle[2].position.x,newTriangle[2].position.y };

		float W0;
		float W1;
		float W2;

		Vector2 edge = Vector2{ newTriangle[2].position.x,newTriangle[2].position.y } - Vector2{ newTriangle[0].position.x,newTriangle[0].position.y };
		float totalArea = Vector2::Cross(a, edge);

		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{
				float gradient = px / static_cast<float>(m_Width);
				gradient += py / static_cast<float>(m_Width);
				gradient /= 2.0f;


				Vector2 p{ float(px),float(py) };




				Vector2 pointToSide{ p - triangleV2 };
				float signedArea1{ Vector2::Cross(b, pointToSide) };

				pointToSide = p - triangleV3;
				float signedArea2{ Vector2::Cross(c, pointToSide) };

				pointToSide = p - triangleV1;
				float signedArea3{ Vector2::Cross(a, pointToSide) };

				float W1 = signedArea1 / totalArea;
				float W2 = signedArea2 / totalArea;
				float W3 = signedArea3 / totalArea;


				int curPixel = px + (py * m_Width);
				ColorRGB finalColor{};
				if (signedArea1 > 0 && signedArea2 > 0 && signedArea3 > 0)
				{
					//depthTest
					//float interpolatedDepth{ newTriangle[0].position.z * W1 + newTriangle[1].position.z * W2 + newTriangle[2].position.z * W3 };
					float interpolatedDepth{ 1 / ((1 / newTriangle[0].position.z) * W1 + (1 / newTriangle[1].position.z) * W2 + (1 / newTriangle[2].position.z) * W3) };
					float depth = 1 / interpolatedDepth;


					//ColorRGB interpolated{ newTriangle[0].color * W1 + newTriangle[1].color * W2 + newTriangle[2].color * W3 };

					/*Vector2 interpolatedUV{
						(((newTriangle[0].uv/newTriangle[0].position.z) * W1) 
						+ ((newTriangle[1].uv / newTriangle[1].position.z) * W2)
							+ ((newTriangle[2].uv / newTriangle[2].position.z) * W3))* interpolatedDepth};
		
					ColorRGB textureColor = m_pTexture->Sample(interpolatedUV);*/

					Vector2 interpolatedUV{
					(((newTriangle[0].uv / newTriangle[0].position.z) * W1) +
					((newTriangle[1].uv / newTriangle[1].position.z) * W2) +
					((newTriangle[2].uv / newTriangle[2].position.z) * W3)) * interpolatedDepth };

					ColorRGB textureColor{ m_pTexture->Sample(interpolatedUV) };
					ColorRGB interpolatedColor{ (textureColor * W1) + (textureColor * W2) + (textureColor * W3) };
					//ColorRGB interpolatedColor = textureColor * W1 + textureColor * W2 + textureColor * W3;

					if (depth < m_pDepthBufferPixels[curPixel])
					{
						m_pDepthBufferPixels[curPixel] = interpolatedDepth;
						//m_ColorBuffer[curPixel] = interpolated;
						m_ColorBuffer[curPixel] = interpolatedColor;
						//m_ColorBuffer[curPixel] = interpolatedColor;
					}



				}

				finalColor = m_ColorBuffer[curPixel];
				//Update Color in Buffer
				finalColor.MaxToOne();

				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.r * 255),
					static_cast<uint8_t>(finalColor.g * 255),
					static_cast<uint8_t>(finalColor.b * 255));
			}
		}


	
}

void dae::Renderer::MakeWorldViewProjectionMatrix(std::vector<Vertex> verts)
{
	//WORLD

	totalTranslationTest += translationTransform.GetTranslation();
	Matrix totalTransMatrix = Matrix::CreateTranslation(totalTranslationTest);
	Matrix totalTransMatrixNeg = Matrix::CreateTranslation(-totalTranslationTest);

	const Matrix finalTransform{ totalTransMatrixNeg * scaleTransform * rotationTransform * translationTransform * totalTransMatrix };

	for (int i{}; i < verts.size(); i++)
	{
		verts[i].position = finalTransform.TransformPoint(verts[i].position);
	}


	for (int i{}; i < verts.size(); i++)
	{
		verts[i].normal = finalTransform.TransformVector(verts[i].normal);
	}


	translationTransform = Matrix{};
	scaleTransform = Matrix{};
	rotationTransform = Matrix{};
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
