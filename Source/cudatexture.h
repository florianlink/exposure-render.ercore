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

#include "buffer.h"

namespace ExposureRender
{

template<class T>
class EXPOSURE_RENDER_DLL Texture3D : public Buffer<T>
{
public:
	HOST Texture3D(const char* pName = "Texture3D", Enums::FilterMode& FilterMode = Enums::Linear) :
		Buffer<T>(pName, Enums::Device),
		Resolution(0),
		CudaArray(NULL)
	{cudaTextureFilterMode 
	}

	HOST Texture3D(const Texture3D& Other) :
		Buffer<T>(),
		Resolution(0)
	{
		*this = Other;
	}

	HOST virtual ~Buffer3D(void)
	{
		this->Free();
	}

	HOST Buffer3D& operator = (const Buffer3D& Other)
	{
		DebugLog("%s: this = %s, Other = %s", __FUNCTION__, this->GetFullName(), Other.GetFullName());
		
		if (Other.Dirty)
		{
			this->Set(Other.MemoryType, Other.Resolution, Other.Data);
			Other.Dirty = false;
		}
		
		sprintf_s(this->Name, MAX_CHAR_SIZE, "Copy of %s", Other.Name);

		return *this;
	}

	HOST void Free(void)
	{
#ifdef __CUDA_ARCH__
		Cuda::FreeArray(this->CudaArray);
#endif
				
		this->Resolution	= Vec3i(0);
		this->NoElements	= 0;
		this->Dirty			= true;
	}

	HOST void Reset(void)
	{
		DebugLog("%s: %s", __FUNCTION__, this->GetFullName());
		
		if (this->GetNoElements() <= 0)
			return;
		
		if (this->MemoryType == Enums::Host)
			memset(this->Data, 0, this->GetNoBytes());
/*
#ifdef __CUDA_ARCH__
		if (this->MemoryType == Enums::Device)
			Cuda::MemSet(this->Data, 0, this->GetNoElements());
#endif
*/		
		this->Dirty = true;
	}

	HOST void Resize(const Vec3i& Resolution)
	{
		DebugLog("%s", __FUNCTION__);
		
		if (this->Resolution == Resolution)
			return;
		else
			this->Free();
		
		this->Resolution = Resolution;
		
		DebugLog("Resolution = [%d x %d x %d]", this->Resolution[0], this->Resolution[1], this->Resolution[2]);

		this->NoElements = this->Resolution[0] * this->Resolution[1] * this->Resolution[2];
		
		if (this->NoElements <= 0)
			return;
		
		DebugLog("No. Elements = %d", this->NoElements);

		char MemoryString[MAX_CHAR_SIZE];
		
		this->GetMemoryString(MemoryString, Enums::MegaByte);

		if (this->MemoryType == Enums::Host)
		{
			this->Data = (T*)malloc(this->GetNoBytes());
			DebugLog("Allocated %s on host", MemoryString);
		}

#ifdef __CUDA_ARCH__
		if (this->MemoryType == Enums::Device)
		{
			Cuda::Allocate(this->Data, this->GetNoElements());
			DebugLog("Allocated %s on device", MemoryString);
		}
#endif

		this->Reset();
	}

	HOST void Set(const Enums::MemoryType& MemoryType, const Vec3i& Resolution, T* Data)
	{
		DebugLog("%s: %s, %d x %d x %d", __FUNCTION__, this->GetFullName(), Resolution[0], Resolution[1], Resolution[2]);

		this->Resize(Resolution);

		if (this->NoElements <= 0)
			return;

		if (this->MemoryType == Enums::Host)
		{
			if (MemoryType == Enums::Host)
				memcpy(this->Data, Data, this->GetNoBytes());
			
#ifdef __CUDA_ARCH__
			if (MemoryType == Enums::Device)
				Cuda::MemCopyDeviceToHost(Data, this->Data, this->GetNoElements());
#endif
		}

#ifdef __CUDA_ARCH__
		if (this->MemoryType == Enums::Device)
		{
			if (MemoryType == Enums::Host)
				Cuda::MemCopyHostToDevice(Data, this->Data, this->GetNoElements());

			if (MemoryType == Enums::Device)
				Cuda::MemCopyDeviceToDevice(Data, this->Data, this->GetNoElements());
		}
#endif

		this->Dirty = true;
	}

private:
	Vec3i	Resolution;
#ifdef __CUDA_ARCH__
	cudaArray*	CudaArray
#endif
};

}