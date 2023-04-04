/**

@file window.hpp
Provides a Window class that represents a display area.
*/
#pragma once

#include <optional>
#include <vector>

#include "graphics.hpp"

/** @brief The Window class represents a graphics display area.
 * This includes not only windows with titles and menus, but also display areas
 * for the mouse cursor, etc.
 */

class Window {
 public:
  /* @brief WindowWriter provides the associated PixelWriter of a Window.
   */
  class WindowWriter : public PixelWriter {
   public:
    WindowWriter(Window& window) : window_{window} {}
    /* @brief Draws the specified color at the specified position. */
    virtual void Write(int x, int y, const PixelColor& c) override {
      window_.At(x, y) = c;
    }
    /* @brief Returns the width of the associated Window in pixels. */
    virtual int Width() const override { return window_.Width(); }
    /* @brief Returns the height of the associated Window in pixels. */
    virtual int Height() const override { return window_.Height(); }

   private:
    Window& window_;
  };
  // #@@range_end(windowwriter)

  /** @brief Creates a plain drawing area with the specified number of pixels.
   */
  Window(int width, int height);
  ~Window() = default;
  Window(const Window& rhs) = delete;
  Window& operator=(const Window& rhs) = delete;

  /** @brief Draws the display area of this window to the specified PixelWriter.
   * @param writer The destination of the drawing.
   * @param position The position to draw on the writer, relative to its
   * top-left corner.
   */
  void DrawTo(PixelWriter& writer, Vector2D<int> position);
  /* @brief Sets the transparent color. */
  void SetTransparentColor(std::optional<PixelColor> c);
  /* @brief Gets the WindowWriter associated with this instance. */
  WindowWriter* Writer();
  /** @brief Returns the pixel at the specified position. */
  PixelColor& At(int x, int y);
  /* @brief Returns the pixel at the specified position. */
  const PixelColor& At(int x, int y) const;

  /** @brief Returns the width of the drawing area in pixels. */
  int Width() const;
  /* @brief Returns the height of the drawing area in pixels. */
  int Height() const;

 private:
  int width_, height_;
  std::vector<std::vector<PixelColor>> data_{};
  WindowWriter writer_{*this};
  std::optional<PixelColor> transparent_color_{std::nullopt};
};
