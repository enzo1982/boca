 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "layer.h"

BoCA_BEGIN_COMPONENT(TagEdit)

namespace BoCA
{
	class TagEdit : public CS::ExtensionComponent
	{
		private:
			LayerTags		*mainTabLayer;
		public:
			static const String	&GetComponentSpecs();

						 TagEdit();
						~TagEdit();
	};
};

BoCA_DEFINE_EXTENSION_COMPONENT(TagEdit)

BoCA_END_COMPONENT(TagEdit)