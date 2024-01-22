/*******************************************************************************
  File Name:
    jason.h

  Summary:
    JSON parser
    
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


#ifndef JSON_H_INCLUDED
#define JSON_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** Max token size of JSON element. Token means that pair of key and value. */
#define JSON_MAX_TOKEN_SIZE 64
/** Max size of token name. */
#define JSON_MAX_NAME_SIZE 16

/**
 * \brief JSON type.
 */
enum json_type
{
    JSON_TYPE_NULL = 0,
    JSON_TYPE_OBJECT,
    JSON_TYPE_STRING,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_INTEGER,
    JSON_TYPE_REAL,
    JSON_TYPE_ARRAY,
    JSON_TYPE_MAX
};

/** \brief JSON data structure. */
struct json_obj
{
    /** Type of this data. */
    enum json_type type;
    /** Name of this data. */
    char name[JSON_MAX_NAME_SIZE];
    /** End pointer of JSON buffer. */
    char *end_ptr;

    /** Value of this JSON token. */
    union
    {
        /* String data. */
        char s[JSON_MAX_TOKEN_SIZE - JSON_MAX_NAME_SIZE];
        /* Boolean data. */
        int b;
        /* Fixed number data. */
        int i;
        /* Real number data. */
        double d;
        /* Object or Array data. */
        char *o; /* Start point of object. */
    } value;
};

/**
 * \brief Create the JSON data from the string buffer.
 *
 * \param[out] obj             Pointer of JSON token which will be stored json informations.
 * \param[in]  data            JSON data represented as a string.
 * \param[in]  data_len        JSON data length.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_create(struct json_obj *obj, const char *data, int data_len);

/**
 * \brief Get child count in the JSON object.
 *
 * The input should be an object or array.
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_get_child_count(struct json_obj *obj);

/**
 * \brief Get child data in the JSON object.
 *
 * The input should be an object or array.
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 * \param[in]  index           Index which is located in the parent object.
 * \param[in]  out             Pointer of JSON token which will be stored child JSON informations.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_get_child(struct json_obj *obj, int index, struct json_obj *out);

/**
 * \brief Find data from the JSON object
 *
 * The input should be an object or array.
 * This function supported colon separated search.
 *
 * If JSON data is as follows
 * {"obj1":{"data1":"value1","data2":"value2","obj2":{"data3":"value3"}}}
 * You can be found value of data3 using the following name variable.
 * "obj1:obj2:data3"
 *
 * \param[in]  obj             Pointer of the parent JSON data.
 * \param[in]  name            The name of the item you are looking for.
 * \param[in]  out             Pointer of JSON token which will be stored child JSON informations.
 *
 * \return     0               Success.
 * \return     otherwise       Failed to create.
 */
int json_find(struct json_obj *obj, const char *name, struct json_obj *out);


#ifdef __cplusplus
}
#endif

#endif /* JSON_H_INCLUDED */

