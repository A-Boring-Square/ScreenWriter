#ifndef SCREENWRITER_HPP
#define SCREENWRITER_HPP

#include <windows.h>
#include <vector>

#ifdef SCREENWRITER_EXPORTS
#define SCREENWRITER_API __declspec(dllexport)
#else
#define SCREENWRITER_API __declspec(dllimport)
#endif

enum ShapeType {
    RECTANGLE,
    LINE,
    POLYGON,
    ICON,
    BITMAP_IMAGE // Renamed from BITMAP
};

struct Shape {
    ShapeType type;
    COLORREF color;
    union {
        RECT rect;
        struct { int x1, y1, x2, y2; };
        struct { POINT* points; int pointCount; };
        struct { HICON hIcon; int x, y; };
        struct { HBITMAP hBitmap; int x, y, width, height; }; // Using HBITMAP directly
    };
};

class SCREENWRITER_API ScreenWriter {
public:
    ScreenWriter();
    ~ScreenWriter();

    void DrawRectangle(int left, int top, int right, int bottom, COLORREF color);
    void DrawLine(int x1, int y1, int x2, int y2, COLORREF color);
    void DrawPolygon(POINT* points, int pointCount, COLORREF color);
    void DrawIcon(HICON hIcon, int x, int y);
    void DrawBitmap(HBITMAP hBitmap, int x, int y, int width, int height);

    void MoveShape(int shapeId, int newX, int newY);
    void DeleteShape(int shapeId);
    void RedrawScreen(); // Redraw all shapes

private:
    HDC hdc;
    std::vector<Shape> shapes; // Vector to store shapes
    void DrawShape(const Shape& shape); // Helper to draw a specific shape
};

#endif // SCREENWRITER_HPP
