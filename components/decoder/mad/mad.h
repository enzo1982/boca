 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(DecoderMAD)

namespace BoCA
{
	class DecoderMAD : public CS::DecoderComponent
	{
		friend mad_flow	 MADInputCallback(void *, mad_stream *);
		friend mad_flow	 MADOutputCallback(void *, const mad_header *, mad_pcm *);
		friend mad_flow	 MADHeaderCallback(void *, const mad_header *, mad_pcm *);
		friend mad_flow	 MADErrorCallback(void *, mad_stream *, mad_frame *);

		private:
			ConfigLayer		*configLayer;

			Bool			 stop;
			Bool			 finished;

			Buffer<unsigned char>	 inputBuffer;
			Buffer<signed int>	 samplesBuffer;

			Track			*infoTrack;

			Int			 numFrames;

			Int			 delaySamples;
			Int			 padSamples;

			Int			 delaySamplesLeft;

			Int			 offset;

			Mutex			*readDataMutex;
			Mutex			*samplesBufferMutex;
			Thread			*decoderThread;

			Bool			 SkipID3v2Tag(IO::InStream *);
			Bool			 ParseVBRHeaders(IO::InStream *);

			Int			 GetMPEGFrameSize(const Buffer<UnsignedByte> &);

			Int			 ReadMAD(Bool);
		public:
			static const String	&GetComponentSpecs();

						 DecoderMAD();
						~DecoderMAD();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderMAD)

BoCA_END_COMPONENT(DecoderMAD)
