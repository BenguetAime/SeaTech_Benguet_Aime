/*******************************************************************************
  File Name:
    stream_writer.c

  Summary:
    Stream utility for the IoT service.

*******************************************************************************/ 

//DOM-IGNORE-BEGIN
/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#include <string.h>
#include "winc1500_api.h"
#include "demo_config.h"

#if defined(USING_SUPPORT_IOT)
#include "demo_support/iot/stream_writer.h"

void stream_writer_init(struct stream_writer * writer, char *buffer, size_t max_length, stream_writer_write_func_t func, void *priv_data)
{
    writer->max_size = max_length;
    writer->buffer = buffer;
    writer->written = 0;
    writer->write_func = func;
    writer->priv_data = priv_data;
}

void stream_writer_send_8(struct stream_writer * writer, int8_t value)
{
    int remain = writer->max_size - writer->written;
    
    if (remain < 1)
    {
        stream_writer_send_remain(writer);
    }
    
    writer->buffer[writer->written++] = (char)value;
}

void stream_writer_send_16BE(struct stream_writer * writer, int16_t value)
{
    stream_writer_send_8(writer, (value >> 8) & 0xFF);
    stream_writer_send_8(writer, value & 0xFF);
}

void stream_writer_send_16LE(struct stream_writer * writer, int16_t value)
{
    stream_writer_send_8(writer, value & 0xFF);
    stream_writer_send_8(writer, (value >> 8) & 0xFF);
}    

void stream_writer_send_32BE(struct stream_writer * writer, int32_t value)
{
    stream_writer_send_8(writer, (value >> 24) & 0xFF);
    stream_writer_send_8(writer, (value >> 16) & 0xFF);
    stream_writer_send_8(writer, (value >> 8) & 0xFF);
    stream_writer_send_8(writer, value & 0xFF);
}

void stream_writer_send_32LE(struct stream_writer * writer, int32_t value)
{
    stream_writer_send_8(writer, value & 0xFF);
    stream_writer_send_8(writer, (value >> 8) & 0xFF);
    stream_writer_send_8(writer, (value >> 16) & 0xFF);
    stream_writer_send_8(writer, (value >> 24) & 0xFF);
}

void stream_writer_send_buffer(struct stream_writer * writer, const char *buffer, size_t length)
{
    for (; length > 0; length--, buffer++)
    {
        stream_writer_send_8(writer, *buffer);
    }
}

void stream_writer_send_remain(struct stream_writer * writer)
{
    if (writer->written > 0)
    {
        writer->write_func(writer->priv_data, writer->buffer, writer->written);
        writer->written = 0;
    }
}

#endif