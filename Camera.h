#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		Vector3 rightLocal{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		float rotspeed{ 0.005f };

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//first obn
			Vector3 right = Vector3::Cross(up, forward).Normalized();
			Vector4 rightMatrix{ right,0 };
			Vector3 up = Vector3::Cross(forward, right).Normalized();
			Vector4 upMatrix{ up,0 };
			Vector4 forwardMatrix{ forward,0 };
			Vector4 positionMatrix{ origin,1 };

			Matrix obn{ rightMatrix,upMatrix,forwardMatrix,positionMatrix };

			Matrix invView = Matrix::Inverse(obn);
			
			//ONB => invViewMatrix
			invViewMatrix = obn;
			//Inverse(ONB) => ViewMatrix
			viewMatrix = invView;
			//Matrix::CreateLookAtLH()
			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			//TODO W2

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//...

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				
				origin += forward * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
			
				origin -= forward * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
			
				origin -= rightLocal * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
			
				origin += rightLocal * deltaTime;
			}
			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);



			//Middle mouse is 2
			//Right mouse is 8


			//MovementMouse
			if (SDL_BUTTON(mouseState) == 1)
			{
		
				origin -= forward * mouseY * deltaTime;

			}

			if (SDL_BUTTON(mouseState) == 16)
			{

				origin.y -= mouseY * deltaTime;
			}

			//RotationMouse
			if (SDL_BUTTON(mouseState) == 8)
			{
		

				totalPitch -= mouseX * rotspeed;
				totalYaw -= mouseY * rotspeed;
			}


			Matrix finalRot = Matrix::CreateRotation(totalYaw, totalPitch, 0);
			forward = finalRot.TransformVector(Vector3::UnitZ);
			forward.Normalize();
	
			rightLocal = finalRot.TransformVector(Vector3::UnitX);
			rightLocal.Normalize();


			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
