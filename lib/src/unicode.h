#ifndef TREE_SITTER_UNICODE_H_
#define TREE_SITTER_UNICODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stdint.h>

#define U_EXPORT
#define U_EXPORT2
#include "unicode/utf8.h"
#include "unicode/utf16.h"

static const int32_t TS_DECODE_ERROR = U_SENTINEL;

// These functions read one unicode code point from the given string,
// returning the number of bytes consumed.
typedef uint32_t (*UnicodeDecodeFunction)(
  const uint8_t *string,
  uint32_t length,
  int32_t *code_point
);

static inline uint32_t ts_decode_utf8(
  const uint8_t *string,
  uint32_t length,
  int32_t *code_point
) {
  uint32_t i = 0;
  U8_NEXT(string, i, length, *code_point);
  return i;
}

static inline uint32_t ts_decode_utf16(
  const uint8_t *string,
  uint32_t length,
  int32_t *code_point
) {
  uint32_t i = 0;
  U16_NEXT(((uint16_t *)string), i, length, *code_point);
  return i * 2;
}

static void UTF32ToUTF8(uint32_t codePoint, uint8_t *utf8, uint32_t *out_length) {
    if (codePoint <= 0x7F) {
        utf8[0] = (uint8_t)(codePoint);
        *out_length = 1;
    } else if (codePoint <= 0x7FF) {
        utf8[0] = 0xC0 | ((uint8_t)(codePoint >> 6));
        utf8[1] = 0x80 | ((uint8_t)(codePoint & 0x3F));
        *out_length = 2;
    } else if (codePoint <= 0xFFFF) {
        utf8[0] = 0xE0 | ((uint8_t)(codePoint >> 12));
        utf8[1] = 0x80 | (((uint8_t)(codePoint >> 6)) & 0x3F);
        utf8[2] = 0x80 | ((uint8_t)(codePoint & 0x3F));
        *out_length = 3;
    } else if (codePoint <= 0x10FFFF) {
        utf8[0] = 0xF0 | ((uint8_t)(codePoint >> 18));
        utf8[1] = 0x80 | (((uint8_t)(codePoint >> 12)) & 0x3F);
        utf8[2] = 0x80 | (((uint8_t)(codePoint >> 6)) & 0x3F);
        utf8[3] = 0x80 | ((uint8_t)(codePoint & 0x3F));
        *out_length = 4;
    } else {
        // 使用错误或替代码位
        utf8[0] = 0xEF;
        utf8[1] = 0xBF;
        utf8[2] = 0xBD;  // UTF-8 编码的U+FFFD
        *out_length = 3;
    }
}

static inline uint32_t ts_decode_utf32(
  const uint8_t *string,
  uint32_t length,
  int32_t *code_point
) {
  // Need to ensure the string is properly aligned for a uint32_t access
  // This assumes the string is already aligned; if not, this would require additional checks.
  const uint32_t *aligned_string = (const uint32_t *)string;
  uint32_t i = 0;
  int32_t code = 0;
  if (i >= length) {
      return 0;
  }
  code = aligned_string[i];
  int32_t len = 0;
  UTF32ToUTF8(code, (uint8_t*)code_point, &len);
  return len;
}

#ifdef __cplusplus
}
#endif

#endif  // TREE_SITTER_UNICODE_H_
