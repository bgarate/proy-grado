//
// Created by bruno on 17/05/17.
//

#include "VideoDecoder.h"

#include <stdexcept>
#include <algorithm>
#include <string>

#include <boost/lexical_cast.hpp>
#include <src/logging/Logger.h>


void VideoDecoder::ThrowOnCondition(const bool cond, const std::string &message)
{
    if (!cond) return;
    throw std::runtime_error(message);
}

VideoDecoder::VideoDecoder()
        : codec_initialized_(false),
          first_iframe_recv_(false),
          format_ctx_ptr_(NULL),
          codec_ctx_ptr_(NULL),
          codec_ptr_(NULL),
          frame_ptr_(NULL),
          frame_rgb_ptr_(NULL),
          img_convert_ctx_ptr_(NULL),
          input_format_ptr_(NULL),
          frame_rgb_raw_ptr_(NULL),
          update_codec_params_(false)
{

}

bool VideoDecoder::InitCodec()
{
    if (codec_initialized_)
    {
        return true;
    }

    try
    {
        // Very first init
        avcodec_register_all();
        av_register_all();
        av_log_set_level(AV_LOG_QUIET);

        codec_ptr_ = avcodec_find_decoder(AV_CODEC_ID_H264);
        ThrowOnCondition(codec_ptr_ == NULL, "Codec H264 not found!");

        codec_ctx_ptr_ = avcodec_alloc_context3(codec_ptr_);
        codec_ctx_ptr_->pix_fmt = AV_PIX_FMT_YUV420P;
        codec_ctx_ptr_->skip_frame = AVDISCARD_DEFAULT;
        codec_ctx_ptr_->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
        codec_ctx_ptr_->skip_loop_filter = AVDISCARD_DEFAULT;
        codec_ctx_ptr_->workaround_bugs = AVMEDIA_TYPE_VIDEO;
        codec_ctx_ptr_->codec_id = AV_CODEC_ID_H264;
        codec_ctx_ptr_->skip_idct = AVDISCARD_DEFAULT;
        // At the beginning we have no idea about the frame size
        codec_ctx_ptr_->width = 0;
        codec_ctx_ptr_->height = 0;

        if (codec_ptr_->capabilities & CODEC_CAP_TRUNCATED)
        {
            codec_ctx_ptr_->flags |= CODEC_FLAG_TRUNCATED;
        }
        codec_ctx_ptr_->flags2 |= CODEC_FLAG2_CHUNKS;

        frame_ptr_ = av_frame_alloc();
        ThrowOnCondition(!frame_ptr_ , "Can not allocate memory for frames!");

        ThrowOnCondition(
                avcodec_open2(codec_ctx_ptr_, codec_ptr_, NULL) < 0,
                "Can not open the decoder!");

        av_init_packet(&packet_);
    }
    catch (const std::runtime_error& e)
    {
        Logger::logError(e.what());
        Reset();
        return false;
    }

    codec_initialized_ = true;
    first_iframe_recv_ = false;
    Logger::logError("H264 Codec is partially initialized!");
    return true;
}

bool VideoDecoder::ReallocateBuffers()
{
    Logger::logError("Buffer reallocation request");
    if (!codec_initialized_)
    {
        return false;
    }

    try
    {
        ThrowOnCondition(codec_ctx_ptr_->width == 0 || codec_ctx_ptr_->height == 0,
                         std::string("Invalid frame size:") +
                         boost::lexical_cast<std::string>(codec_ctx_ptr_->width) +
                         " x " + boost::lexical_cast<std::string>(codec_ctx_ptr_->width));

        const uint32_t num_bytes = avpicture_get_size(PIX_FMT_RGB24, codec_ctx_ptr_->width, codec_ctx_ptr_->width);
        frame_rgb_ptr_ = av_frame_alloc();

        ThrowOnCondition(!frame_rgb_ptr_, "Can not allocate memory for frames!");

        frame_rgb_raw_ptr_ = reinterpret_cast<uint8_t*>(av_malloc(num_bytes * sizeof(uint8_t)));
        ThrowOnCondition(frame_rgb_raw_ptr_ == NULL,
                         std::string("Can not allocate memory for the buffer: ") +
                         boost::lexical_cast<std::string>(num_bytes));
        ThrowOnCondition(0 == avpicture_fill(
                reinterpret_cast<AVPicture*>(frame_rgb_ptr_), frame_rgb_raw_ptr_, PIX_FMT_RGB24,
                codec_ctx_ptr_->width, codec_ctx_ptr_->height),
                         "Failed to initialize the picture data structure.");

        img_convert_ctx_ptr_ = sws_getContext(codec_ctx_ptr_->width, codec_ctx_ptr_->height, codec_ctx_ptr_->pix_fmt,
                                              codec_ctx_ptr_->width, codec_ctx_ptr_->height, PIX_FMT_RGB24,
                                              SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
    catch (const std::runtime_error& e)
    {
        Logger::logError(e.what());
        Reset();  // reset() is intentional
        return false;
    }

    return true;
}

void VideoDecoder::CleanupBuffers()
{
    if (frame_rgb_ptr_)
    {
        av_free(frame_rgb_ptr_);
    }

    if (frame_rgb_raw_ptr_)
    {
        av_free(frame_rgb_raw_ptr_);
    }

    if (img_convert_ctx_ptr_)
    {
        sws_freeContext(img_convert_ctx_ptr_);
    }

    Logger::logError("Buffer cleanup!");
}

void VideoDecoder::Reset()
{
    if (codec_ctx_ptr_)
    {
        avcodec_close(codec_ctx_ptr_);
    }

    if (frame_ptr_)
    {
        av_free(frame_ptr_);
    }

    CleanupBuffers();

    codec_initialized_ = false;
    first_iframe_recv_ = false;
    Logger::logError("Reset!");
}

VideoDecoder::~VideoDecoder()
{
    Reset();
    Logger::logError("Dstr!");
}

void VideoDecoder::ConvertFrameToRGB()
{
    if (!codec_ctx_ptr_->width || !codec_ctx_ptr_->height)
        return;

    sws_scale(img_convert_ctx_ptr_, frame_ptr_->data, frame_ptr_->linesize, 0,
              codec_ctx_ptr_->height, frame_rgb_ptr_->data, frame_rgb_ptr_->linesize);
}

bool VideoDecoder::SetH264Params(uint8_t *sps_buffer_ptr, uint32_t sps_buffer_size,
                                 uint8_t *pps_buffer_ptr, uint32_t pps_buffer_size)
{
    // This function is called in the same thread as Decode(), so no sync is necessary
    // TODO: Exact sizes + more error checkings
    update_codec_params_ = (sps_buffer_ptr && pps_buffer_ptr &&
                            sps_buffer_size && pps_buffer_size &&
                            (pps_buffer_size < 32) && (sps_buffer_size < 32));

    if (update_codec_params_)
    {
        codec_data_.resize(sps_buffer_size + pps_buffer_size);
        std::copy(sps_buffer_ptr, sps_buffer_ptr + sps_buffer_size, codec_data_.begin());
        std::copy(pps_buffer_ptr, pps_buffer_ptr + pps_buffer_size, codec_data_.begin() + sps_buffer_size);
    }
    else
    {
        // invalid data
        codec_data_.clear();
    }

    return update_codec_params_;
}

bool VideoDecoder::Decode(const ARCONTROLLER_Frame_t *bebop_frame_ptr_)
{
    if (!codec_initialized_)
    {
        if (!InitCodec())
        {
            Logger::logError("Codec initialization failed!");
            return false;
        }
    }

    /*
     * For VideoStream2, we trick avcodec whenever we receive a new SPS/PPS
     * info from the Bebop. SetH264Params() function will fill a buffer with SPS/PPS
     * data, then these are passed to avcodec_decode_video2() here, once for each SPS/PPS update.
     * Apparently, avcodec_decode_video2() function picks up the changes and apply them to
     * upcoming video packets.
     *
     * More info on VS v2.0: http://developer.parrot.com/blog/2016/ARSDK-3-8-release/
     *
     * */
    if (update_codec_params_ && codec_data_.size())
    {
        Logger::logError("Updating H264 codec parameters (Buffer Size: " + std::to_string(codec_data_.size()) + ") ...");
        packet_.data = &codec_data_[0];
        packet_.size = (int) codec_data_.size();
        int32_t frame_finished = 0;
        const int32_t len = avcodec_decode_video2(codec_ctx_ptr_, frame_ptr_, &frame_finished, &packet_);
        if (len >= 0 && len == packet_.size)
        {
            // success, skip this step until next codec update
            update_codec_params_ = false;
        }
        else
        {
            Logger::logError("Unexpected error while updating H264 parameters.");
            return false;
        }
    }

    if (!bebop_frame_ptr_->data || !bebop_frame_ptr_->used)
    {
        Logger::logError("Invalid frame data. Skipping.");
        return false;
    }

    packet_.data = bebop_frame_ptr_->data;
    packet_.size = bebop_frame_ptr_->used;

    const uint32_t width_prev = GetFrameWidth();
    const uint32_t height_prev = GetFrameHeight();

    int32_t frame_finished = 0;
    while (packet_.size > 0)
    {
        const int32_t len = avcodec_decode_video2(codec_ctx_ptr_, frame_ptr_, &frame_finished, &packet_);
        if (len >= 0)
        {
            if (frame_finished)
            {
                if ((GetFrameWidth() != width_prev) || (GetFrameHeight() != height_prev))
                {
                    Logger::logError("Frame size changed to " +
                                             std::to_string(GetFrameWidth()) + "x" +
                                             std::to_string(GetFrameHeight()));
                    if (!ReallocateBuffers())
                    {
                        Logger::logError("Buffer reallocation failed!");
                    }
                }
                ConvertFrameToRGB();
            }

            if (packet_.data)
            {
                packet_.size -= len;
                packet_.data += len;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}
