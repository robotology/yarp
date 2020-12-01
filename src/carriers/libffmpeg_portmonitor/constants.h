#ifndef YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_CL_PARAMS_H
#define YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_CL_PARAMS_H

#include <string>
#include <vector>
#include <map>

#include "libavcodec/codec_id.h"
#include <yarp/sig/Image.h>

static const std::vector<std::string> FFMPEGPORTMONITOR_IGNORE_PARAMS {
    "tcp",
    "fast_tcp",
    "udp",
    "send.portmonitor",
    "recv.portmonitor",
    "file.libffmpeg",
    "type.dll"
};

static const std::string FFMPEGPORTMONITOR_CL_CODEC_KEY = "codec";

static const std::vector<std::string> FFMPEGPORTMONITOR_CL_CODECS {
    "mpeg2video",
    "h264",
    "h265"
};

static const std::vector<int> FFMPEGPORTMONITOR_CODE_CODECS {
    AV_CODEC_ID_MPEG2VIDEO,
    AV_CODEC_ID_H264,
    AV_CODEC_ID_H265
};

static const std::vector<std::string> FFMPEGPORTMONITOR_PRIV_PARAMS {
    "preset",
    "tune",
    "crf",
    "cqp",
    "crf_max",
    "x264opts",
    "aq_mode",
    "aq_strength",
    "psy",
    "rc_lookahead",
    "weightp",
    "weightb",
    "cplxblur",
    "ssim",
    "intra_refresh",
    "b_bias",
    "b_pyramid",
    "mixed_refs",
    "dct8x8",
    "fast_pskip",
    "aud",
    "mbtree",
    "direct_pred",
    "slice_max_size",
    "fastfirstpass",
    "profile",
    "nal_hrd",
    "x265opts",
    "profile"
};

static std::map<int, int> FFMPEGPORTMONITOR_PIXELMAP = {
  { VOCAB_PIXEL_RGB, AV_PIX_FMT_RGB24 },
  { VOCAB_PIXEL_RGBA, AV_PIX_FMT_RGBA },
  { VOCAB_PIXEL_BGR, AV_PIX_FMT_BGR24 },
  { VOCAB_PIXEL_BGRA, AV_PIX_FMT_BGRA },
  { VOCAB_PIXEL_YUV_420, AV_PIX_FMT_YUV420P }
};

static std::map<int, int> FFMPEGPORTMONITOR_CODECPIXELMAP = {
    { AV_CODEC_ID_H264, AV_PIX_FMT_YUV420P },
    { AV_CODEC_ID_H265, AV_PIX_FMT_YUV420P },
    { AV_CODEC_ID_MPEG2VIDEO, AV_PIX_FMT_YUV420P }
};

#endif  // YARP_FFMPEG_CARRIER_FFMPEGPORTMONITOR_CL_PARAMS_H