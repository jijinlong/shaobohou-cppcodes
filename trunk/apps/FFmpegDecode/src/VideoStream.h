#ifndef VIDEO_STREAM_H
#define VIDEO_STREAM_H

#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class VideoStream
{
    public:
        VideoStream();
        VideoStream(const std::string &filename);
        VideoStream(const VideoStream &other);
        virtual ~VideoStream();

        VideoStream& operator=(const VideoStream &other);

        bool decode_next_frame(const PixelFormat &desired_pixel_format);
        void open(const std::string &filename);
        void close();

        int frame_width() const;
        int frame_height() const;
        int frame_count() const;

        // note that this function does not check that buffer is big enough
        bool copy2buffer(unsigned char *buffer, const int buffer_width, const int buffer_height, const int buffer_pixel_size) const;
        unsigned char* data() const;

    private:
//         AVFormatParameters format_params;
        AVFormatContext *format_context;

        AVCodecContext *codec_context;
        AVCodec *codec;

        AVFrame *frame;
        AVPicture picture;

        SwsContext *convert_context;
        PixelFormat target_pixel_format;

        int video_index;
        int counter;

        static bool registered_all;

        void init();
        void updateConvertContext(const PixelFormat &desired_pixel_format);
};

#endif
