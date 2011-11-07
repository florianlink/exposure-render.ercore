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

#include "Geometry.h"

struct EXPOSURE_RENDER_DLL VolumeInfo
{
	Vec3f	m_Extent;
	Vec3f	m_InvExtent;
	Vec3f	m_MinAABB;
	Vec3f	m_MaxAABB;
	Vec3f	m_InvMinAABB;
	Vec3f	m_InvMaxAABB;
	float	m_IntensityMin;
	float	m_IntensityMax;
	float	m_IntensityRange;
	float	m_IntensityInvRange;
	float	m_StepSize;
	float	m_StepSizeShadow;
	float	m_DensityScale;
	float	m_GradientDelta;
	float	m_InvGradientDelta;
	Vec3f	m_GradientDeltaX;
	Vec3f	m_GradientDeltaY;
	Vec3f	m_GradientDeltaZ;
	Vec3f	m_Spacing;
	Vec3f	m_InvSpacing;
};