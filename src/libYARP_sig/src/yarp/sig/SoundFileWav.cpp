/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundFileWav.h>

#include <yarp/conf/system.h>

#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/Sound.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace
{
    YARP_LOG_COMPONENT(SOUNDFILE_WAV, "yarp.sig.SoundFileWav")
}

YARP_BEGIN_PACK
class PcmWavHeader {
public:
    NetInt32 wavHeader;
    NetInt32 wavLength;
    NetInt32 formatHeader1;
    NetInt32 formatHeader2;
    NetInt32 formatLength;

    struct {
        NetInt16 pcmFormatTag;
        NetInt16 pcmChannels;
        NetInt32 pcmSamplesPerSecond;
        NetInt32 pcmBytesPerSecond;
        NetInt16 pcmBlockAlign;
        NetInt16 pcmBitsPerSample;
    } pcm;
    ManagedBytes pcmExtraData;

    NetInt32     dummyHeader;
    NetInt32     dummyLength;
    ManagedBytes dummyData;

    NetInt32 dataHeader;
    NetInt32 dataLength;

    void setup_to_write(const Sound& sound, FILE *fp);
    void setup_to_write_with_markers(const Sound& sound, FILE *fp);
    bool parse_from_file(FILE *fp);
};
YARP_END_PACK

bool PcmWavHeader::parse_from_file(FILE *fp)
{
    yCTrace(SOUNDFILE_WAV, "bool PcmWavHeader::parse_from_file(FILE *fp)\n");

    size_t ret;

    ret = fread(&wavHeader, sizeof(wavHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&wavLength, sizeof(wavLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatHeader1, sizeof(formatHeader1), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatHeader2, sizeof(formatHeader2), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&formatLength, sizeof(formatLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmFormatTag, sizeof(pcm.pcmFormatTag), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmChannels, sizeof(pcm.pcmChannels), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }
    if (pcm.pcmChannels <= 0)
    {
        yCError(SOUNDFILE_WAV, "pcmChannels <=0, invalid wav file\n");
        return false;
    }

    ret = fread(&pcm.pcmSamplesPerSecond, sizeof(pcm.pcmSamplesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }
    if (pcm.pcmSamplesPerSecond <= 0)
    {
        yCError(SOUNDFILE_WAV, "pcmSamplesPerSecond <=0, invalid wav file\n");
        return false;
    }

    ret = fread(&pcm.pcmBytesPerSecond, sizeof(pcm.pcmBytesPerSecond), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }
    if (pcm.pcmBytesPerSecond <= 0)
    {
        yCError(SOUNDFILE_WAV, "pcmBytesPerSecond <=0, invalid wav file\n");
        return false;
    }

    ret = fread(&pcm.pcmBlockAlign, sizeof(pcm.pcmBlockAlign), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    ret = fread(&pcm.pcmBitsPerSample, sizeof(pcm.pcmBitsPerSample), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }
    if (pcm.pcmBitsPerSample != 16)
    {
        yCError(SOUNDFILE_WAV, "sorry, lousy .wav read code only does 16-bit ints\n");
        return false;
    }

    //extra bytes in pcm chuck
    int extra_size = formatLength - sizeof(pcm);
    if (extra_size != 0)
    {
        yCError(SOUNDFILE_WAV, "extra_size = %d\n", extra_size);
        pcmExtraData.allocate(extra_size);
        ret = fread(pcmExtraData.get(), extra_size, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
    }

    //extra chunks
    ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    while (dummyHeader != yarp::os::createVocab32('d', 'a', 't', 'a'))
    {
        ret = fread(&dummyLength, sizeof(dummyLength), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
        dummyData.clear();
        dummyData.allocate(dummyLength);
        ret = fread(dummyData.get(), dummyLength, 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
        ret = fread(&dummyHeader, sizeof(dummyHeader), 1, fp);
        if (ret != 1) {
            yCError(SOUNDFILE_WAV, "failed to read .wav file");
            return false;
        }
    }

    dataHeader = dummyHeader;
    ret = fread(&dataLength, sizeof(dataLength), 1, fp);
    if (ret != 1) {
        yCError(SOUNDFILE_WAV, "failed to read .wav file");
        return false;
    }

    return true;
}

void PcmWavHeader::setup_to_write(const Sound& src, FILE *fp)
{
    int bitsPerSample = 16;
    size_t channels = src.getChannels();
    size_t bytes = channels*src.getSamples()*2;
    int align = channels*((bitsPerSample+7)/8);

    wavHeader = yarp::os::createVocab32('R','I','F','F');
    wavLength = bytes + sizeof(PcmWavHeader) - 2*sizeof(NetInt32);
    formatHeader1 = yarp::os::createVocab32('W','A','V','E');
    formatHeader2 = yarp::os::createVocab32('f','m','t',' ');
    formatLength = sizeof(pcm);

    pcm.pcmFormatTag = 1; /* PCM! */
    pcm.pcmChannels = channels;
    pcm.pcmSamplesPerSecond = (int)src.getFrequency();
    pcm.pcmBytesPerSecond = align*pcm.pcmSamplesPerSecond;
    pcm.pcmBlockAlign = align;
    pcm.pcmBitsPerSample = bitsPerSample;

    dataHeader = yarp::os::createVocab32('d','a','t','a');
    dataLength = bytes;


    fwrite(&wavHeader,sizeof(wavHeader),1,fp);
    fwrite(&wavLength,sizeof(wavLength),1,fp);
    fwrite(&formatHeader1,sizeof(formatHeader1),1,fp);

    fwrite(&formatHeader2,sizeof(formatHeader2),1,fp);
    fwrite(&formatLength,sizeof(formatLength),1,fp);

    fwrite(&pcm.pcmFormatTag,sizeof(pcm.pcmFormatTag),1,fp);
    fwrite(&pcm.pcmChannels,sizeof(pcm.pcmChannels),1,fp);
    fwrite(&pcm.pcmSamplesPerSecond,sizeof(pcm.pcmSamplesPerSecond),1,fp);
    fwrite(&pcm.pcmBytesPerSecond,sizeof(pcm.pcmBytesPerSecond),1,fp);
    fwrite(&pcm.pcmBlockAlign,sizeof(pcm.pcmBlockAlign),1,fp);
    fwrite(&pcm.pcmBitsPerSample,sizeof(pcm.pcmBitsPerSample),1,fp);

    fwrite(&dataHeader,sizeof(dataHeader),1,fp);
    fwrite(&dataLength,sizeof(dataLength),1,fp);

}

void PcmWavHeader::setup_to_write_with_markers(const Sound& src, FILE *fp)
{
    int bitsPerSample = 16;
    size_t channels = src.getChannels();
    size_t bytes = channels*src.getSamples()*2;
    int align = channels*((bitsPerSample+7)/8);

    // Calculate sizes for marker chunks
    size_t num_markers = src.getMarkersCount();

    // Size of cue chunk: 4 bytes (chunk ID) + 4 bytes (chunk size) + 4 bytes (num cue points) + 24 bytes per cue point
    size_t cue_chunk_size = 0;
    if (num_markers > 0) {
        cue_chunk_size = 4 + 4 + (4 + num_markers * 24);
    }

    // Size of LIST/adtl chunk for labels: 4 bytes (LIST) + 4 bytes (size) + 4 bytes (adtl) + labels
    size_t list_chunk_size = 0;
    if (num_markers > 0)
    {
        list_chunk_size = 4 + 4 + 4; // LIST + size + adtl
        for (size_t i = 0; i < num_markers; i++)
        {
            SoundMarker marker = src.getMarker(i);
            // Each label chunk: 'labl' (4) + size (4) + cue point ID (4) + null-terminated string
            size_t label_len = marker.label.length() + 1; // +1 for null terminator
            if (label_len % 2) label_len++; // Pad to even number
            list_chunk_size += 4 + 4 + 4 + label_len;
        }
    }

    wavHeader = yarp::os::createVocab32('R','I','F','F');
    wavLength = bytes + sizeof(PcmWavHeader) - 2*sizeof(NetInt32) + cue_chunk_size + list_chunk_size;
    formatHeader1 = yarp::os::createVocab32('W','A','V','E');
    formatHeader2 = yarp::os::createVocab32('f','m','t',' ');
    formatLength = sizeof(pcm);

    pcm.pcmFormatTag = 1; /* PCM! */
    pcm.pcmChannels = channels;
    pcm.pcmSamplesPerSecond = (int)src.getFrequency();
    pcm.pcmBytesPerSecond = align*pcm.pcmSamplesPerSecond;
    pcm.pcmBlockAlign = align;
    pcm.pcmBitsPerSample = bitsPerSample;

    dataHeader = yarp::os::createVocab32('d','a','t','a');
    dataLength = bytes;

    // Write RIFF header
    fwrite(&wavHeader,sizeof(wavHeader),1,fp);
    fwrite(&wavLength,sizeof(wavLength),1,fp);
    fwrite(&formatHeader1,sizeof(formatHeader1),1,fp);

    // Write fmt chunk
    fwrite(&formatHeader2,sizeof(formatHeader2),1,fp);
    fwrite(&formatLength,sizeof(formatLength),1,fp);

    fwrite(&pcm.pcmFormatTag,sizeof(pcm.pcmFormatTag),1,fp);
    fwrite(&pcm.pcmChannels,sizeof(pcm.pcmChannels),1,fp);
    fwrite(&pcm.pcmSamplesPerSecond,sizeof(pcm.pcmSamplesPerSecond),1,fp);
    fwrite(&pcm.pcmBytesPerSecond,sizeof(pcm.pcmBytesPerSecond),1,fp);
    fwrite(&pcm.pcmBlockAlign,sizeof(pcm.pcmBlockAlign),1,fp);
    fwrite(&pcm.pcmBitsPerSample,sizeof(pcm.pcmBitsPerSample),1,fp);

    // Write cue chunk if markers exist
    if (num_markers > 0) {
        NetInt32 cueHeader = yarp::os::createVocab32('c','u','e',' ');
        NetInt32 cueChunkSize = 4 + num_markers * 24; // 4 bytes for num cue points + 24 bytes per cue point
        NetInt32 numCuePoints = num_markers;

        fwrite(&cueHeader, sizeof(cueHeader), 1, fp);
        fwrite(&cueChunkSize, sizeof(cueChunkSize), 1, fp);
        fwrite(&numCuePoints, sizeof(numCuePoints), 1, fp);

        // Write each cue point
        for (size_t i = 0; i < num_markers; i++) {
            SoundMarker marker = src.getMarker(i);

            NetInt32 cuePointId = i + 1; // Cue point IDs start from 1
            NetInt32 position = marker.sample_id;
            NetInt32 dataChunkId = yarp::os::createVocab32('d','a','t','a');
            NetInt32 chunkStart = 0;
            NetInt32 blockStart = 0;
            NetInt32 sampleOffset = position;

            fwrite(&cuePointId, sizeof(cuePointId), 1, fp);
            fwrite(&position, sizeof(position), 1, fp);
            fwrite(&dataChunkId, sizeof(dataChunkId), 1, fp);
            fwrite(&chunkStart, sizeof(chunkStart), 1, fp);
            fwrite(&blockStart, sizeof(blockStart), 1, fp);
            fwrite(&sampleOffset, sizeof(sampleOffset), 1, fp);
        }
    }

    // Write LIST/adtl chunk with labels if markers exist
    if (num_markers > 0) {
        NetInt32 listHeader = yarp::os::createVocab32('L','I','S','T');
        NetInt32 listChunkSize = list_chunk_size - 8; // Total size minus 'LIST' and size fields
        NetInt32 adtlHeader = yarp::os::createVocab32('a','d','t','l');

        fwrite(&listHeader, sizeof(listHeader), 1, fp);
        fwrite(&listChunkSize, sizeof(listChunkSize), 1, fp);
        fwrite(&adtlHeader, sizeof(adtlHeader), 1, fp);

        // Write each label
        for (size_t i = 0; i < num_markers; i++) {
            SoundMarker marker = src.getMarker(i);

            NetInt32 lablHeader = yarp::os::createVocab32('l','a','b','l');
            NetInt32 cuePointId = i + 1;

            std::string label = marker.label;
            size_t label_len = label.length() + 1; // +1 for null terminator
            bool need_pad = (label_len % 2) == 1;
            if (need_pad) label_len++;

            NetInt32 lablChunkSize = 4 + label_len; // 4 bytes for cue point ID + label string

            fwrite(&lablHeader, sizeof(lablHeader), 1, fp);
            fwrite(&lablChunkSize, sizeof(lablChunkSize), 1, fp);
            fwrite(&cuePointId, sizeof(cuePointId), 1, fp);
            fwrite(label.c_str(), label.length() + 1, 1, fp); // Write null-terminated string

            if (need_pad) {
                char pad = 0;
                fwrite(&pad, 1, 1, fp);
            }
        }
    }

    // Write data chunk header
    fwrite(&dataHeader,sizeof(dataHeader),1,fp);
    fwrite(&dataLength,sizeof(dataLength),1,fp);
}

//#######################################################################################################

bool yarp::sig::file::read_wav_file(Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        yCError(SOUNDFILE_WAV, "cannot open file %s for reading\n", filename);
        return false;
    }

    PcmWavHeader header;
    if (!header.parse_from_file(fp))
    {
        yCError(SOUNDFILE_WAV, "error parsing header of file %s\n", filename);
        fclose(fp);
        return false;
    };


    int freq = header.pcm.pcmSamplesPerSecond;
    int channels = header.pcm.pcmChannels;
    int bits = header.pcm.pcmBitsPerSample;
    int samples = header.dataLength/(bits/8)/channels;
    sound_data.resize(samples,channels);
    sound_data.setFrequency(freq);
    ManagedBytes bytes(header.dataLength);
    yCDebug(SOUNDFILE_WAV, "%d channels %d samples %d frequency\n", channels, samples, freq);

    size_t result;
    result = fread(bytes.get(),bytes.length(),1,fp);
    YARP_UNUSED(result);

    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    for (int i=0; i<samples; i++) {
        for (int j=0; j<channels; j++) {
            sound_data.set(data[ct],i,j);
            ct++;
        }
    }

    fclose(fp);
    return true;
}

bool yarp::sig::file::read_wav_bytestream(Sound& data, const char* bytestream)
{
    yCError(SOUNDFILE_WAV, "read_wav_bytestream() Not yet implemented");
    return true;
}

bool yarp::sig::file::write_wav_file(const Sound& sound_data, const char * filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        yCError(SOUNDFILE_WAV, "cannot open file %s for writing\n", filename);
        return false;
    }

    PcmWavHeader header;

    // Use the new method if markers are present, otherwise use the old method
    if (sound_data.getMarkersCount() > 0) {
        header.setup_to_write_with_markers(sound_data, fp);
    } else {
        header.setup_to_write(sound_data, fp);
    }

    ManagedBytes bytes(header.dataLength);
    auto* data = reinterpret_cast<NetInt16*>(bytes.get());
    int ct = 0;
    size_t samples = sound_data.getSamples();
    size_t channels = sound_data.getChannels();
    for (size_t i=0; i<samples; i++) {
        for (size_t j=0; j<channels; j++) {
            int v = sound_data.get(i,j);
            data[ct] = v;
            ct++;
        }
    }
    fwrite(bytes.get(),bytes.length(),1,fp);

    fclose(fp);

    // Write Audacity label file if markers are present
    if (sound_data.getMarkersCount() > 0)
    {
        std::string label_filename(filename);
        size_t dot_pos = label_filename.find_last_of('.');
        if (dot_pos != std::string::npos)
        {
            label_filename = label_filename.substr(0, dot_pos);
        }
        label_filename += ".txt";

        FILE *label_fp = fopen(label_filename.c_str(), "w");
        if (!label_fp)
        {
            yCWarning(SOUNDFILE_WAV, "cannot open file %s for writing Audacity labels\n", label_filename.c_str());
        }
        else
        {
            double frequency = sound_data.getFrequency();
            for (size_t i = 0; i < sound_data.getMarkersCount(); i++)
            {
                SoundMarker marker = sound_data.getMarker(i);
                double time_seconds = static_cast<double>(marker.sample_id) / frequency;
                // Audacity label format: start_time\tend_time\tlabel
                // For point labels, start and end time are the same
                fprintf(label_fp, "%.6f\t%.6f\t%s\n", time_seconds, time_seconds, marker.label.c_str());
            }
            fclose(label_fp);
            yCInfo(SOUNDFILE_WAV, "Audacity label file written: %s\n", label_filename.c_str());
        }
    }

    return true;
}
