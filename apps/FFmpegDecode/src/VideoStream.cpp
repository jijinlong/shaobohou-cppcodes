#include "VideoStream.h"

#include <iostream>
#include <cassert>

using std::string;
using std::cout;
using std::endl;

bool VideoStream::registered_all = false;

VideoStream::VideoStream()
{
    init();
}

VideoStream::VideoStream(const string &filename)
{
    init();
    open(filename);
}

VideoStream::VideoStream(const VideoStream &other)
{
    if (this != &other)
        assert(false);
}

VideoStream::~VideoStream()
{
    close();
}

VideoStream& VideoStream::operator=(const VideoStream &other)
{
    if (this != &other)
        assert(false);

    return *this;
}

// NB. converting to different format takes different amount of time, i.e. YUV420P is much faster than RGB24
bool VideoStream::decode_next_frame(const PixelFormat &desired_pixel_format)
{
    assert(format_context != 0);
    assert(codec_context != 0);
    assert(codec != 0);
    assert(frame != 0);
    assert(video_index >= 0);

    // Read a frame/packet.
    AVPacket packet;
    if (av_read_frame(format_context, &packet) < 0) // if no packet is read, return false
        return false;

    // If it is NOT a video packet from our video stream, return false
    if (packet.stream_index != video_index)
    {
        av_free_packet(&packet);
        return false;
    }

    // Decode the packet
    int got_picture;
    int len = avcodec_decode_video(codec_context, frame, &got_picture, packet.data, packet.size);
    av_free_packet(&packet);
    assert(len >= 0);

    if (got_picture)
    {
        if(target_pixel_format != desired_pixel_format) updateConvertContext(desired_pixel_format);

        sws_scale(convert_context, frame->data, frame->linesize, 0, codec_context->height, picture.data, picture.linesize);
//         img_convert(&picture, target_pixel_format, reinterpret_cast<AVPicture *>(frame), codec_context->pix_fmt, codec_context->width, codec_context->height);

        counter++;
    }
    else
        return false;   // if frame did not contain a picture, return false;

    return true;
}

int VideoStream::frame_width() const
{
    assert(codec_context != 0);
    return codec_context->width;
}

int VideoStream::frame_height() const
{
    assert(codec_context != 0);
    return codec_context->height;
}

int VideoStream::frame_count() const
{
    return counter;
}

bool VideoStream::copy2buffer(unsigned char *buffer, const int buffer_width, const int buffer_height, const int buffer_pixel_size) const
{
    assert(target_pixel_format == PIX_FMT_RGB24);
    assert(buffer_width >= frame_width());
    assert(buffer_height >= frame_height());
    assert(buffer_pixel_size == 3);

    if(counter <= 0)
    {
        cout << "No video frame has been decoded yet." << endl;
        return false;
    }

    unsigned char *psource = picture.data[0];;
    unsigned char *pdest = buffer;

    for (int y = 0; y < frame_height(); y++)
    {
        memcpy(pdest, psource, frame_width()*buffer_pixel_size*sizeof(unsigned char));
        psource += frame_width()*buffer_pixel_size;
        pdest += buffer_width*buffer_pixel_size;
    }

    return true;
}

unsigned char* VideoStream::data() const
{
    if(counter > 0)
        return picture.data[0]; // need to have decoded at least one video frame
    else
        return NULL;
}

void VideoStream::open(const std::string &filename)
{
    close();     // close first

    // Register - only once.
    if(!registered_all)
    {
        av_register_all();
        registered_all = true;
    }

//     int err = av_open_input_file(&fcx, input_filename, NULL, 0, format_params);
    int err = av_open_input_file(&format_context, filename.c_str(), NULL, 0, NULL);
    if (err < 0)
    {
        cout << "Can't open file: " << filename << endl;
        exit(1);
    }

    // Find the stream info.
    err = av_find_stream_info(format_context);
    if (err < 0)
    {
        cout << "Can't find stream info for file: " << filename << endl;
        exit(1);
    }

    // Find the first video stream.
    for (unsigned int i = 0; i < format_context->nb_streams; i++)
    {
        codec_context = format_context->streams[i]->codec;
        if (codec_context->codec_type == CODEC_TYPE_VIDEO)
        {
            video_index = i;
            break;
        }
    }
    if(video_index < 0)
    {
        cout << "Can't find stream for file: " << filename << endl;
        exit(1);
    }

    // open stream FIXME: proper closing of av before exit.
    if (video_index >= 0)
    {
        codec = avcodec_find_decoder(codec_context->codec_id);
        if (codec)
        {
            err = avcodec_open(codec_context, codec);
            if (err < 0)
            {
                cout << "Can't open codec for " << filename << endl;
                exit(1);
            }
        }
        else
        {
            cout << "Can't find codec for " << filename << endl;
            exit(1);
        }
    }
    else{
        cout << "Video stream not found for " << filename << endl;
        exit(1);
    }

    // allocate memory for frame
    frame = avcodec_alloc_frame();
}

void VideoStream::close()
{
    if(format_context) av_close_input_file(format_context);
    if(codec_context) avcodec_close(codec_context);
    if(frame) av_free(frame);
    if(convert_context) avpicture_free(&picture);
    if(convert_context) sws_freeContext(convert_context);
    init();
}

void VideoStream::init()
{
    format_context = 0;
    codec_context = 0;
    codec = 0;
    frame = 0;
    convert_context = 0;
    target_pixel_format = PIX_FMT_NONE;
    video_index = -1;
    counter = 0;
}

void VideoStream::updateConvertContext(const PixelFormat &desired_pixel_format)
{
    assert(codec_context != 0);

    if(target_pixel_format == desired_pixel_format)
        return;

    if(target_pixel_format != PIX_FMT_NONE)
    {
        assert(convert_context != 0);
        avpicture_free(&picture);
        sws_freeContext(convert_context);
        convert_context = 0;
    }

    target_pixel_format = desired_pixel_format;
    avpicture_alloc(&picture, target_pixel_format, codec_context->width, codec_context->height);

    assert(convert_context == 0);
    convert_context = sws_getContext(codec_context->width, codec_context->height, codec_context->pix_fmt,
                                     codec_context->width, codec_context->height, target_pixel_format, SWS_BICUBIC, NULL, NULL, NULL);
    assert(convert_context != 0);
}
