#pragma once

#include <cstdint>

enum class FieldType : uint8_t
{
    FT_ARRAY,
    FT_BINARY,
    FT_BOOLEAN,
    FT_CODE,
    FT_DATE,
    FT_DECIMAL_128,
    FT_DOUBLE,
    FT_INT_32,
    FT_INT_64,
    FT_MAXKEY,
    FT_MINKEY,
    FT_NULL,
    FT_OBJECT,
    FT_OBJECT_ID,
    FT_BSON_REG_EXPR,
    FT_STRING,
    FT_BSON_SYMBOL,
    FT_TIMESTAMP,
    FT_UNDEFINED,
};