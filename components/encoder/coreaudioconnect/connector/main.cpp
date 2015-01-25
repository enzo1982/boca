 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"
#include "communication.h"

#include <algorithm>

static CoreAudioCommSetup	 setup		= { 0 };

static unsigned char		*buffer		= NULL;
static unsigned int		 bufferSize	= 0;
static unsigned int		 bytesConsumed	= 0;

static CA::UInt32		 totalSamples	= 0;

void QueryCoreAudioCodecs(CoreAudioCommCodecs *comm)
{
	ZeroMemory(comm, sizeof(CoreAudioCommCodecs));

	/* Get supported codecs.
	 */
	CA::UInt32	 size = 0;

	CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_EncodeFormatIDs, 0, NULL, &size);

	CA::UInt32	*formats = (CA::UInt32 *) malloc(size);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_EncodeFormatIDs, 0, NULL, &size, formats);

	for (CA::UInt32 i = 0; i < size / sizeof(CA::UInt32) && i < 32; i++)
	{
		comm->codecs[i] = formats[i];

		/* Get bitrate ranges for each codec.
		 */
		CA::UInt32	 brSize = 0;

		CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(CA::UInt32), &formats[i], &brSize);

		CA::AudioValueRange	*bitrateValues = (CA::AudioValueRange *) malloc(brSize);

		CA::AudioFormatGetProperty(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(CA::UInt32), &formats[i], &brSize, bitrateValues);

		for (CA::UInt32 j = 0; j < brSize / sizeof(CA::AudioValueRange) && j < 64; j++)
		{
			comm->bitrates[i][j * 2    ] = bitrateValues[j].mMinimum;
			comm->bitrates[i][j * 2 + 1] = bitrateValues[j].mMaximum;
		}

		free(bitrateValues);
	}

	free(formats);
}

int GetOutputSampleRate(const CoreAudioCommSetup &setup)
{
	/* Get supported sample rate ranges for selected codec.
	 */
	CA::UInt32	 format	= setup.codec;
	CA::UInt32	 size	= 0;

	CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size);

	if (size == 0) return setup.rate;

	CA::AudioValueRange	*sampleRates = (CA::AudioValueRange *) malloc(size);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size, sampleRates);

	/* Find best fit output sample rate.
	 */
	int	 outputRate = 0;

	for (CA::UInt32 i = 0; i < size / sizeof(CA::AudioValueRange); i++)
	{
		/* Check if encoder supports arbitrary sample rate.
		 */
		if (sampleRates[i].mMinimum == 0 &&
		    sampleRates[i].mMaximum == 0) { outputRate = setup.rate; break; }

		/* Check if input rate falls into current sample rate range.
		 */
		if (setup.rate >= sampleRates[i].mMinimum &&
		    setup.rate <= sampleRates[i].mMaximum) { outputRate = setup.rate; break; }

		/* Check if current sample rate range fits better than previous best.
		 */
		if (abs(setup.rate - sampleRates[i].mMinimum) < abs(setup.rate - outputRate)) outputRate = sampleRates[i].mMinimum;
		if (abs(setup.rate - sampleRates[i].mMaximum) < abs(setup.rate - outputRate)) outputRate = sampleRates[i].mMaximum;
	}

	free(sampleRates);

	return outputRate;
}

CA::OSStatus AudioConverterComplexInputDataProc(CA::AudioConverterRef inAudioConverter, CA::UInt32 *ioNumberDataPackets, CA::AudioBufferList *ioData, CA::AudioStreamPacketDescription **outDataPacketDescription, void *inUserData)
{
	static unsigned char	*suppliedData	  = NULL;
	static unsigned int	 suppliedDataSize = 0;

	suppliedDataSize = std::min(bufferSize - bytesConsumed, (unsigned int) *ioNumberDataPackets * setup.channels * (setup.bits / 8));
	suppliedData	 = (unsigned char *) realloc(suppliedData, suppliedDataSize);

	memcpy(suppliedData, buffer + bytesConsumed, suppliedDataSize);

	*ioNumberDataPackets = suppliedDataSize / setup.channels / (setup.bits / 8);

	ioData->mBuffers[0].mData           = suppliedData;
	ioData->mBuffers[0].mDataByteSize   = suppliedDataSize;
	ioData->mBuffers[0].mNumberChannels = setup.channels;

	totalSamples  += *ioNumberDataPackets;
	bytesConsumed += ioData->mBuffers[0].mDataByteSize;

	return 0;
}

int main(int argc, char *argv[])
{
	/* Open file mapping.
	 */
	HANDLE	 mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, argv[1]);

	if (mapping == NULL) return 1;

	/* Map view to communication buffer.
	 */
	CoreAudioCommBuffer	*comm = (CoreAudioCommBuffer *) MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	/* Load Core Audio DLLs.
	 */
	LoadCoreFoundationDLL();
	LoadCoreAudioDLL();

	/* Main processing loop.
	 */
	CA::AudioFileID		 audioFile	= NULL;
	CA::AudioConverterRef	 converter	= NULL;
	CA::AudioBufferList	*buffers	= NULL;

	CA::UInt32		 dataSize	= 0;
	CA::UInt32		 packetsWritten	= 0;

	CA::UInt32		 sleepTime	= 100;

	while (comm->command != CommCommandQuit)
	{
		while (comm->status != CommStatusIssued) Sleep(sleepTime);

		switch (comm->command)
		{
			case CommCommandHello:
				if (((CoreAudioCommHello *) &comm->data)->version == 1 &&
				    corefoundationdll != NULL && coreaudiodll != NULL) comm->status = CommStatusReady;
				else						       comm->status = CommStatusError;

				break;
			case CommCommandCodecs:
				QueryCoreAudioCodecs((CoreAudioCommCodecs *) &comm->data);

				comm->length = sizeof(CoreAudioCommCodecs);
				comm->status = CommStatusReady;

				break;
			case CommCommandSetup:
				memcpy(&setup, comm->data, sizeof(CoreAudioCommSetup));

				/* Setup encoder for first packet.
				 */
				{
					/* Fill out source format description.
					 */
					CA::AudioStreamBasicDescription	 sourceFormat = { 0 };

					sourceFormat.mFormatID		    = CA::kAudioFormatLinearPCM;
					sourceFormat.mFormatFlags	    = CA::kLinearPCMFormatFlagIsPacked | (setup.bits > 8 ? CA::kLinearPCMFormatFlagIsSignedInteger : 0);
					sourceFormat.mSampleRate	    = setup.rate;
					sourceFormat.mChannelsPerFrame	    = setup.channels;
					sourceFormat.mBitsPerChannel	    = setup.bits;
					sourceFormat.mFramesPerPacket	    = 1;
					sourceFormat.mBytesPerFrame	    = sourceFormat.mChannelsPerFrame * sourceFormat.mBitsPerChannel / 8;
					sourceFormat.mBytesPerPacket	    = sourceFormat.mFramesPerPacket * sourceFormat.mBytesPerFrame;

					/* Fill out destination format description.
					 */
					CA::AudioStreamBasicDescription	 destinationFormat = { 0 };

					destinationFormat.mFormatID	    = setup.codec;
					destinationFormat.mSampleRate	    = GetOutputSampleRate(setup);
					destinationFormat.mChannelsPerFrame = setup.channels;

					CA::UInt32	 formatSize = sizeof(destinationFormat);

					CA::AudioFormatGetProperty(CA::kAudioFormatProperty_FormatInfo, 0, NULL, &formatSize, &destinationFormat);

					/* Create audio converter object.
					 */
					CA::OSStatus	 status = CA::AudioConverterNew(&sourceFormat, &destinationFormat, &converter);

					if (status != 0) { comm->status = CommStatusError; break; }

					/* Set bitrate if format does support bitrates.
					 */
					CA::UInt32	 bitratesSize = 0;

					if (CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(destinationFormat.mFormatID), &destinationFormat.mFormatID, &bitratesSize) == 0)
					{
						CA::UInt32	 bitrate = setup.bitrate;

						status = CA::AudioConverterSetProperty(converter, CA::kAudioConverterEncodeBitRate, sizeof(CA::UInt32), &bitrate);

						if (status != 0)
						{
							CA::AudioConverterDispose(converter);

							comm->status = CommStatusError;

							break;
						}
					}

					/* Create audio file object for output file.
					 */
					CA::CFStringRef	 fileNameString	= CA::CFStringCreateWithCString(NULL, setup.file, CA::kCFStringEncodingUTF8);
					CA::CFURLRef	 fileNameURL	= CA::CFURLCreateWithFileSystemPath(NULL, fileNameString, CA::kCFURLWindowsPathStyle, false);
					CA::UInt32	 fileType	= setup.format ? CA::kAudioFileM4AType : CA::kAudioFileAAC_ADTSType;

					CA::AudioFileCreateWithURL(fileNameURL, fileType, &destinationFormat, CA::kAudioFileFlags_EraseFile, &audioFile);

					CA::CFRelease(fileNameURL);
					CA::CFRelease(fileNameString);

					/* Get magic cookie and supply it to audio file.
					 */
					CA::UInt32	 cookieSize = 0;

					if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NULL) == 0)
					{
						unsigned char	*cookie = (unsigned char *) malloc(cookieSize);

						CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

						free(cookie);
					}

					/* Get maximum output packet size.
					 */
					CA::UInt32	 valueSize = 4;

					CA::AudioConverterGetProperty(converter, CA::kAudioConverterPropertyMaximumOutputPacketSize, &valueSize, &dataSize);

					/* Set up buffer for Core Audio.
					 */
					buffers = (CA::AudioBufferList	*) malloc(sizeof(CA::AudioBufferList) + sizeof(CA::AudioBuffer));

					buffers->mNumberBuffers = 1;

					buffers->mBuffers[0].mData	     = (unsigned char *) malloc(dataSize);
					buffers->mBuffers[0].mDataByteSize   = dataSize;
					buffers->mBuffers[0].mNumberChannels = setup.channels;
				}

				packetsWritten = 0;
				totalSamples   = 0;

				comm->status = CommStatusReady;

				sleepTime = 0;

				break;
			case CommCommandEncode:
				if (converter == NULL) { comm->status = CommStatusError; break; }

				/* Encode supplied packet.
				 */
				{
					/* Configure buffer.
					 */
					bufferSize += comm->length;
					buffer	    = (unsigned char *) realloc(buffer, bufferSize);

					memmove(buffer, buffer + bytesConsumed, bufferSize - bytesConsumed - comm->length);
					memcpy(buffer + bufferSize - bytesConsumed - comm->length, comm->data, comm->length);

					bufferSize -= bytesConsumed;
					buffer	    = (unsigned char *) realloc(buffer, bufferSize);

					bytesConsumed = 0;

					/* Convert frames.
					 */
					CA::UInt32				 packets = 1;
					CA::AudioStreamPacketDescription	 packet;

					buffers->mBuffers[0].mDataByteSize = dataSize;

					while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, NULL, &packets, buffers, &packet) == 0)
					{
						if (buffers->mBuffers[0].mDataByteSize == 0) break;

						CA::AudioFileWritePackets(audioFile, false, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

						packetsWritten += packets;

						if (bufferSize - bytesConsumed < 65536) break;

						buffers->mBuffers[0].mDataByteSize = dataSize;
					}
				}

				comm->status = CommStatusReady;

				break;
			case CommCommandFinish:
				if (converter == NULL) { comm->status = CommStatusError; break; }

				/* Finish encoding after last packet.
				 */
				{
					/* Convert final frames.
					 */
					CA::UInt32				 packets = 1;
					CA::AudioStreamPacketDescription	 packet;

					buffers->mBuffers[0].mDataByteSize = dataSize;

					while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, NULL, &packets, buffers, &packet) == 0)
					{
						if (buffers->mBuffers[0].mDataByteSize == 0) break;

						CA::AudioFileWritePackets(audioFile, false, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

						packetsWritten += packets;

						buffers->mBuffers[0].mDataByteSize = dataSize;
					}

					free(buffers->mBuffers[0].mData);
					free(buffers);

					/* Write priming and remainder info.
					 */
					CA::AudioConverterPrimeInfo	 primeInfo;
					CA::UInt32			 size = sizeof(primeInfo);

					if (CA::AudioConverterGetProperty(converter, CA::kAudioConverterPrimeInfo, &size, &primeInfo) == 0)
					{
						int	 divider = 1;
						int	 extra	 = 0;

						if (setup.codec == 'aach' ||
						    setup.codec == 'aacp') { divider = 2; extra = 480; }

						CA::AudioFilePacketTableInfo	 pti;

						pti.mPrimingFrames     = primeInfo.leadingFrames + extra;
						pti.mRemainderFrames   = primeInfo.trailingFrames;
						pti.mNumberValidFrames = totalSamples / divider;

						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyPacketTableInfo, sizeof(pti), &pti);
					}

					/* Get and set magic cookie again as some
					 * encoders may change it during encoding.
					 */
					CA::UInt32	 cookieSize = 4;

					if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NULL) == 0)
					{
						unsigned char	*cookie = (unsigned char *) malloc(cookieSize);

						CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

						free(cookie);
					}

					/* Close converter and audio file.
					 */
					CA::AudioConverterDispose(converter);
					CA::AudioFileClose(audioFile);
				}

				comm->status = CommStatusReady;

				sleepTime = 100;

				break;
			case CommCommandQuit:
				comm->status = CommStatusReady;

				break;
		}
	}

	/* Free Core Audio DLLs.
	 */
	FreeCoreFoundationDLL();
	FreeCoreAudioDLL();

	/* Close file mapping handle.
	 */
	UnmapViewOfFile(comm);
	CloseHandle(mapping);

	return 0;
}