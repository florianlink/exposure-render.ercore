/*
	Copyright (c) 2011, T. Kroes <t.kroes@tudelft.nl>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	- Neither the name of the TU Delft nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "macros.cuh"
#include "utilities.h"
#include "transport.h"
#include "camera.h"

namespace ExposureRender
{

DEVICE void SampleCamera(const Camera& Camera, Ray& R, const int& U, const int& V, CameraSample& CS)
{
	Vec2f ScreenPoint;

	ScreenPoint[0] = Camera.Screen[0][0] + (Camera.InvScreen[0] * (float)(U + CS.FilmUV[0] * 1.0f / Camera.FilmSize[0]));
	ScreenPoint[1] = Camera.Screen[1][0] + (Camera.InvScreen[1] * (float)(V + CS.FilmUV[1] * 1.0f / Camera.FilmSize[1]));

	R.O		= Camera.Pos;
	R.D		= Normalize(Camera.N + (ScreenPoint[0] * Camera.U) - (ScreenPoint[1] * Camera.V));
	R.MinT	= Camera.ClipNear;
	R.MaxT	= Camera.ClipFar;

	if (Camera.ApertureSize != 0.0f)
	{
		Vec2f LensUV;

		switch (Camera.ApertureShape)
		{
			case Enums::Circular:
			{
				LensUV = Camera.ApertureSize * ConcentricSampleDisk(CS.LensUV);
				break;
			}

			case Enums::Polygon:
			{
				float LensY = CS.LensUV[0] * Camera.NoApertureBlades;
				float Side = (int)LensY;
				float Offset = (float) LensY - Side;
				float Distance = (float) sqrtf(CS.LensUV[1]);
				float A0 = (float) (Side * PI_F * 2.0f / Camera.NoApertureBlades + Camera.ApertureAngle);
				float A1 = (float) ((Side + 1.0f) * PI_F * 2.0f / Camera.NoApertureBlades + Camera.ApertureAngle);
				const float EyeX = (float) ((cos(A0) * (1.0f - Offset) + cos(A1) * Offset) * Distance);
				const float EyeY = (float) ((sin(A0) * (1.0f - Offset) + sin(A1) * Offset) * Distance);
				
				LensUV[0] = EyeX * gpTracer->Camera.ApertureSize;
				LensUV[1] = EyeY * gpTracer->Camera.ApertureSize;
				break;
			}
		}
		

		const Vec3f LI = Camera.U * LensUV[0] + Camera.V * LensUV[1];

		R.O += LI;
		R.D = Normalize(R.D * Camera.FocalDistance - LI);
	}
}

DEVICE ScatterEvent SampleRay(Ray R, CRNG& RNG)
{
	ScatterEvent SE[3] = { ScatterEvent(Enums::Volume), ScatterEvent(Enums::Light), ScatterEvent(Enums::Object) };

	RayMarcher RM;

	RM.SampleVolume(R, RNG, SE[0]);

	IntersectLights(R, SE[1], true);
	IntersectObjects(R, SE[2]);

	float T = FLT_MAX;

	ScatterEvent NearestRS(Enums::Volume);

	for (int i = 0; i < 3; i++)
	{
		if (SE[i].Valid && SE[i].T < T)
		{
			NearestRS = SE[i];
			T = SE[i].T;
		}
	}

	return NearestRS;
}

DEVICE ColorXYZAf SingleScattering(Tracer* pTracer, const Vec2i& PixelCoord)
{
	CRNG RNG(&gpTracer->FrameBuffer.RandomSeeds1(PixelCoord[0], PixelCoord[1]), &gpTracer->FrameBuffer.RandomSeeds2(PixelCoord[0], PixelCoord[1]));

	ColorXYZf Lv = ColorXYZf::Black();

	MetroSample Sample(RNG);

	Ray R;

	SampleCamera(gpTracer->Camera, R, PixelCoord[0], PixelCoord[1], Sample.CameraSample);

	ScatterEvent SE = SampleRay(R, RNG);

	ColorRGBf RGB;

	if (SE.Valid)
	{
		switch (SE.Type)
		{
			case Enums::Volume:
			{
				Lv += UniformSampleOneLight(SE, RNG, Sample.LightingSample);
				break;
			}

			case Enums::Light:
			{
				Lv += SE.Le;
				break;
			}

			case Enums::Object:
			{
				Lv += UniformSampleOneLight(SE, RNG, Sample.LightingSample);
				break;
			}
		}

		ColorXYZf XYZ;

		XYZ[0] = Clamp(1.0f - expf(-gpTracer->Camera.InvExposure * Lv[0]), 0.0f, 1.0f);
		XYZ[1] = Clamp(1.0f - expf(-gpTracer->Camera.InvExposure * Lv[1]), 0.0f, 1.0f);
		XYZ[2] = Clamp(1.0f - expf(-gpTracer->Camera.InvExposure * Lv[2]), 0.0f, 1.0f);

		RGB = ColorRGBf::FromXYZf(XYZ);

		RGB.Clamp(0.0f, 1.0f);
	}

	return ColorXYZAf(RGB[0], RGB[1], RGB[2], SE.Valid ? 1.0f : 0.0f);
}

}
