 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/component/convertercomponent.h>

BoCA::CS::ConverterComponent::ConverterComponent()
{
}

BoCA::CS::ConverterComponent::~ConverterComponent()
{
}

Bool BoCA::CS::ConverterComponent::SetAudioTrackInfo(const Track &track)
{
	this->track = track;

	return True;
}

Bool BoCA::CS::ConverterComponent::Activate()
{
	return True;
}

Bool BoCA::CS::ConverterComponent::Deactivate()
{
	return True;
}
