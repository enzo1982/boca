 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/dspcomponent.h>

BoCA::AS::DSPComponent::DSPComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::DSPComponent::~DSPComponent()
{
}

Int BoCA::AS::DSPComponent::GetPackageSize()
{
	return specs->func_GetPackageSize(component);
}

Bool BoCA::AS::DSPComponent::SetAudioTrackInfo(const Track &track)
{
	return specs->func_SetAudioTrackInfo(component, &track);
}

Void BoCA::AS::DSPComponent::GetFormatInfo(Format &format)
{
	specs->func_GetFormatInfo(component, &format);
}

Bool BoCA::AS::DSPComponent::Activate()
{
	return specs->func_Activate(component);
}

Bool BoCA::AS::DSPComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Int BoCA::AS::DSPComponent::TransformData(Buffer<UnsignedByte> &buffer, Int size)
{
	return specs->func_TransformData(component, &buffer, size);
}

Int BoCA::AS::DSPComponent::Flush(Buffer<UnsignedByte> &buffer)
{
	return specs->func_Flush(component, &buffer);
}
