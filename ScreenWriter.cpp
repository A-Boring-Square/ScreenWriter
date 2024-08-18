#define SCREENWRITER_EXPORTS
#include "ScreenWriter.hpp"

// Constructor
ScreenWriter::ScreenWriter() {
    hdc = GetDC(NULL); // Get the device context for the screen
}

// Destructor
ScreenWriter::~ScreenWriter() {
    ReleaseDC(NULL, hdc); // Release the device context
}

// Draw a rectangle
void ScreenWriter::DrawRectangle(int left, int top, int right, int bottom, COLORREF color) {
    Shape shape;
    shape.type = RECTANGLE;
    shape.color = color;
    shape.rect = { left, top, right, bottom };
    shapes.push_back(shape);
    DrawShape(shape);
}

// Draw a line
void ScreenWriter::DrawLine(int x1, int y1, int x2, int y2, COLORREF color) {
    Shape shape;
    shape.type = LINE;
    shape.color = color;
    shape.x1 = x1;
    shape.y1 = y1;
    shape.x2 = x2;
    shape.y2 = y2;
    shapes.push_back(shape);
    DrawShape(shape);
}

// Draw a polygon
void ScreenWriter::DrawPolygon(POINT* points, int pointCount, COLORREF color) {
    Shape shape;
    shape.type = POLYGON;
    shape.color = color;
    shape.points = new POINT[pointCount];
    shape.pointCount = pointCount;
    memcpy(shape.points, points, sizeof(POINT) * pointCount);
    shapes.push_back(shape);
    DrawShape(shape);
}

// Draw an icon
void ScreenWriter::DrawIcon(HICON hIcon, int x, int y) {
    Shape shape;
    shape.type = ICON;
    shape.color = 0; // Not used for icons
    shape.hIcon = hIcon;
    shape.x = x;
    shape.y = y;
    shapes.push_back(shape);
    DrawShape(shape);
}

// Draw a bitmap
void ScreenWriter::DrawBitmap(HBITMAP hBitmap, int x, int y, int width, int height) {
    Shape shape;
    shape.type = BITMAP_IMAGE; // Use the renamed type
    shape.color = 0; // Not used for bitmaps
    shape.hBitmap = hBitmap;
    shape.x = x;
    shape.y = y;
    shape.width = width;
    shape.height = height;
    shapes.push_back(shape);
    DrawShape(shape);
}

// Move a shape
void ScreenWriter::MoveShape(int shapeId, int newX, int newY) {
    if (shapeId < 0 || shapeId >= shapes.size()) return;

    Shape& shape = shapes[shapeId];

    switch (shape.type) {
    case RECTANGLE:
        shape.rect.left += newX;
        shape.rect.top += newY;
        shape.rect.right += newX;
        shape.rect.bottom += newY;
        break;
    case LINE:
        shape.x1 += newX;
        shape.y1 += newY;
        shape.x2 += newX;
        shape.y2 += newY;
        break;
    case POLYGON:
        for (int i = 0; i < shape.pointCount; ++i) {
            shape.points[i].x += newX;
            shape.points[i].y += newY;
        }
        break;
    case ICON:
        shape.x += newX;
        shape.y += newY;
        break;
    case BITMAP_IMAGE:
        shape.x += newX;
        shape.y += newY;
        break;
    }

    RedrawScreen();
}

// Delete a shape
void ScreenWriter::DeleteShape(int shapeId) {
    if (shapeId < 0 || shapeId >= shapes.size()) return;

    // Clean up resources
    Shape& shape = shapes[shapeId];
    if (shape.type == POLYGON) {
        delete[] shape.points;
    }

    shapes.erase(shapes.begin() + shapeId);
    RedrawScreen();
}

// Redraw all shapes
void ScreenWriter::RedrawScreen() {
    // Clear the screen (or use a background color)
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // White color
    RECT screenRect;
    screenRect.left = 0;
    screenRect.top = 0;
    screenRect.right = GetDeviceCaps(hdc, HORZRES);
    screenRect.bottom = GetDeviceCaps(hdc, VERTRES);
    FillRect(hdc, &screenRect, hBrush);
    DeleteObject(hBrush);

    // Redraw all shapes
    for (const Shape& shape : shapes) {
        DrawShape(shape);
    }
}

// Draw a specific shape
void ScreenWriter::DrawShape(const Shape& shape) {
    switch (shape.type) {
    case RECTANGLE:
    {
        HBRUSH hBrush = CreateSolidBrush(shape.color);
        FillRect(hdc, &shape.rect, hBrush);
        DeleteObject(hBrush);
    }
    break;
    case LINE:
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, shape.color);
        SelectObject(hdc, hPen);
        MoveToEx(hdc, shape.x1, shape.y1, NULL);
        LineTo(hdc, shape.x2, shape.y2);
        DeleteObject(hPen);
    }
    break;
    case POLYGON:
    {
        HPEN hPolygonPen = CreatePen(PS_SOLID, 2, shape.color);
        SelectObject(hdc, hPolygonPen);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));  // No brush for interior
        Polygon(hdc, shape.points, shape.pointCount);
        DeleteObject(hPolygonPen);
    }
    break;
    case ICON:
        DrawIconEx(hdc, shape.x, shape.y, shape.hIcon, 32, 32, 0, NULL, DI_NORMAL);
        break;
    case BITMAP_IMAGE:
    {
        HDC memDC = CreateCompatibleDC(hdc);
        SelectObject(memDC, shape.hBitmap);
        BitBlt(hdc, shape.x, shape.y, shape.width, shape.height, memDC, 0, 0, SRCCOPY);
        DeleteDC(memDC);
    }
    break;
    }
}
