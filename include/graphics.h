// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Very simple graphics library to do simple things.
//
// Might be useful to consider using Cairo instead and just have an interface
// between that and the Canvas. Well, this is a quick set of things to get
// started (and nicely self-contained).


#ifndef RPI_GRAPHICS_H
#define RPI_GRAPHICS_H

#include "canvas.h"

#include <stdint.h>
#include <stddef.h>

#include <map>

namespace rgb_matrix {
struct Color {
  // usage: e.g Cyan [R=0,G=255,B=255] -> Color color1={0, 255, 255} or now also Color color2=0x00FFFF
  Color() { r = 0, g = 0, b = 0, uu = 0; }      // Note: Order must be changed. Thats why we have to use that way of definition
  Color(uint8_t rr, uint8_t gg, uint8_t bb)  { r=rr, g=gg, b=bb, uu = 0; }
  Color(uint32_t value) : integer(value) {}

  union {                 // GeoGab: Union Extension
    uint32_t integer;     // 4 Bytes (unsigned integer) value (0x00RRGGBB)
    struct {
      uint8_t b;          // Blue
      uint8_t g;          // Green
      uint8_t r;          // Red
      uint8_t uu;         // unused upper byte.
    };
  };

 };

// Font loading bdf files. If this ever becomes more types, just make virtual
// base class.
class Font {
public:
  // Initialize font, but it is only usable after LoadFont() has been called.
  Font();
  ~Font();

  bool LoadFont(const char *path);

  // Return height of font in pixels. Returns -1 if font has not been loaded.
  int height() const { return font_height_; }

  // Return baseline. Pixels from the topline to the baseline.
  int baseline() const { return base_line_; }

  // Return width of given character, or -1 if font is not loaded or character
  // does not exist.
  int CharacterWidth(uint32_t unicode_codepoint) const;

  // Draws the unicode character at position "x","y"
  // with "color" on "background_color" (background_color can be NULL for
  // transparency.
  // The "y" position is the baseline of the font.
  // If we don't have it in the font, draws the replacement character "�" if
  // available.
  // Returns how much we advance on the screen, which is the width of the
  // character or 0 if we didn't draw any chracter.
  int DrawGlyph(Canvas *c, int x, int y,
                const Color &color, const Color *background_color,
                uint32_t unicode_codepoint) const;

  // Same without background. Deprecated, use the one above instead.
  int DrawGlyph(Canvas *c, int x, int y, const Color &color,
                uint32_t unicode_codepoint) const;

  // Create a new font derived from this font, which represents an outline
  // of the original font, essentially pixels tracing around the original
  // letter.
  // This can be used in situations in which it is desirable to frame a letter
  // in a different color to increase contrast.
  // The ownership of the returned pointer is passed to the caller.
  Font *CreateOutlineFont() const;

private:
  Font(const Font& x);  // No copy constructor. Use references or pointer instead.

  struct Glyph;
  typedef std::map<uint32_t, Glyph*> CodepointGlyphMap;

  const Glyph *FindGlyph(uint32_t codepoint) const;

  int font_height_;
  int base_line_;
  CodepointGlyphMap glyphs_;
};

// -- Some utility functions.

// Utility function: set an image from the given buffer containting pixels.
//
// Draw image of size "image_width" and "image_height" from pixel at
// canvas-offset "canvas_offset_x", "canvas_offset_y". Image will be shown
// cropped on the edges if needed.
//
// The canvas offset can be negative, i.e. the image start can be shifted
// outside the image frame on the left/top edge.
//
// The buffer needs to be organized as rows with columns of three bytes
// organized as rgb or bgr. Thus the size of the buffer needs to be exactly
// (3 * image_width * image_height) bytes.
//
// The "image_buffer" parameters contains the data, "buffer_size_bytes" the
// size in bytes.
//
// If "is_bgr" is true, the buffer is treated as BGR pixel arrangement instead
// of RGB.
// Returns 'true' if image was shown within canvas.
bool SetImage(Canvas *c, int canvas_offset_x, int canvas_offset_y,
              const uint8_t *image_buffer, size_t buffer_size_bytes,
              int image_width, int image_height,
              bool is_bgr);

// Draw text, a standard NUL terminated C-string encoded in UTF-8,
// with given "font" at "x","y" with "color".
// "color" always needs to be set (hence it is a reference),
// "background_color" is a pointer to optionally be NULL for transparency.
// "kerning_offset" allows for additional spacing between characters (can be
// negative)
// Returns how many pixels we advanced on the screen.
int DrawText(Canvas *c, const Font &font, int x, int y,
             const Color &color, const Color *background_color,
             const char *utf8_text, int kerning_offset = 0);

// Same without background. Deprecated, use the one above instead.
int DrawText(Canvas *c, const Font &font, int x, int y, const Color &color,
             const char *utf8_text);

//  GEOGAB Extension

// Calculates witdh of a text, 
// with given "font" "kerning_offset". This fuction can be used to caculate the
// offset needed to center a text: 
// Example (text align center): 
//      int x_off = (canvas->width() - TextWidth(font, text_buffer, 2)) / 2;
//      DrawText(rgb_display, font, x_off, y_off, color,NULL, text_buffer, 2);
// Returns the width of the text in pixels

int TextWidth(const Font& font, const char* utf8_text, int kerning_offset = 0);

//  GEOGAB Extension


// Draw text, a standard NUL terminated C-string encoded in UTF-8,
// with given "font" at "x","y" with "color".
// Draw text as above, but vertically (top down).
// The text is a standard NUL terminated C-string encoded in UTF-8.
// "font, "x", "y", "color" and "background_color" are same as DrawText().
// "kerning_offset" allows for additional spacing between characters (can be
// negative).
// Returns font height to advance up on the screen.
int VerticalDrawText(Canvas *c, const Font &font, int x, int y,
                     const Color &color, const Color *background_color,
                     const char *utf8_text, int kerning_offset = 0);

// Draw a circle centered at "x", "y", with a radius of "radius" and with "color"
void DrawCircle(Canvas *c, int x, int y, int radius, const Color &color);

// Draw a line from "x0", "y0" to "x1", "y1" and with "color"
void DrawLine(Canvas *c, int x0, int y0, int x1, int y1, const Color &color);

}  // namespace rgb_matrix

#endif  // RPI_GRAPHICS_H
