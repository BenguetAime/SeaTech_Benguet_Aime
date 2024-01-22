/*******************************************************************************
  File Name:
    stream_writer.h

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


#ifndef STREAM_WRITTER_H_INCLUDED
#define STREAM_WRITTER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*stream_writer_write_func_t)(void *module, char *buffer, size_t buffer_len);

struct stream_writer {
    size_t max_size;
    size_t written;
    stream_writer_write_func_t write_func;
    void *priv_data;
    char *buffer;
};

/**
 * \brief Initialize the Stream writer module.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  buffer          Buffer which will be used for the storing the data.
 * \param[in]  max_length      Maximum size of buffer.
 * \param[in]  func            Function to be called when the buffer is full.
 * \param[in]  priv_data       Private data. It is passed along when callback was called.
 */
void stream_writer_init(struct stream_writer * writer, char *buffer, size_t max_length, stream_writer_write_func_t func, void *priv_data);

/**
 * \brief Write 8bit to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  value           Value will be written.
 */
void stream_writer_send_8(struct stream_writer * writer, int8_t value);

/**
 * \brief Write 16bit big endian value to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  value           Value will be written.
 */
void stream_writer_send_16BE(struct stream_writer * writer, int16_t value);

/**
 * \brief Write 16bit little endian value to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  value           Value will be written.
 */
void stream_writer_send_16LE(struct stream_writer * writer, int16_t value);

/**
 * \brief Write 32bit big endian value to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  value           Value will be written.
 */
void stream_writer_send_32BE(struct stream_writer * writer, int32_t value);

/**
 * \brief Write 32bit little endian value to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  value           Value will be written.
 */
void stream_writer_send_32LE(struct stream_writer * writer, int32_t value);

/**
 * \brief Write buffer to the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 * \param[in]  buffer          Buffer will be written.
 * \param[in]  length          Size of the buffer.
 */
void stream_writer_send_buffer(struct stream_writer * writer, const char *buffer, size_t length);

/**
 * \brief Process remain data in the writer.
 *
 * \param[in]  writer          Pointer of stream writer.
 */
void stream_writer_send_remain(struct stream_writer * writer);


#ifdef __cplusplus
}
#endif

#endif /* STREAM_WRITTER_H_INCLUDED */