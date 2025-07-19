#pragma once

#include "Matrix.h"

// Helper return value struct for Rect<T>::compute_intersection function
template<typename T>
struct intersection_boundaries
{
	T intersectionLeft;
	T intersectionTop;
	T intersectionRight;
	T intersectionBottom;
};

// Class for manipulating 2D axis aligned rectangles
template <typename T>
class Rect
{
public:
	Vector2<T> topleft; // Topleft point of the rectangle
	Vector2<T> size; // Size of the rectangle

	// Constructors

	Rect()
	{}

	Rect(const Vector2<T>& topleft_position_arg, const Vector2<T>& size_arg) :
		topleft(topleft_position_arg),
		size(size_arg)
	{}

	// Template constructor to copy from a Rect with a different template argument U
	template <typename U>
	Rect(const Rect<U>& other_rect) :
		topleft{static_cast<T>(other_rect.topleft.x),
				static_cast<T>(other_rect.topleft.y)},
		size{static_cast<T>(other_rect.size.x),
			 static_cast<T>(other_rect.size.y)}
	{}

	// Member functions

	bool contains_point(const Vector2<T>& point) const;

	bool intersects_rect(const Rect<T>& other_rect) const;

	std::optional<Rect<T>> get_intersection_with_rect(const Rect<T>& other_rect) const;

	bool intersects_line_segment(Vector2<T> segment_point1,
								 Vector2<T> segment_point2) const;

private:
	intersection_boundaries<T> compute_intersection(const Rect<T>& other_rect) const;
};

// Operators

template <typename T>
inline bool operator ==(const Rect<T>& left, const Rect<T>& right)
{
	return (left.topleft.x == right.topleft.x) and (left.size.x == right.size.x) and
		(left.topleft.y == right.topleft.y) and (left.size.y == right.size.y);
}

template <typename T>
inline bool operator !=(const Rect<T>& left, const Rect<T>& right)
{
	return not(left == right);
}

// Typedefs for the most common types
typedef Rect<int>   IntRect;
typedef Rect<float> FloatRect;
typedef Rect<double> DoubleRect;

template <typename T>
bool Rect<T>::contains_point(const Vector2<T>& point) const
{
	// Rectangles with negative dimensions are allowed, so we must handle them correctly

	// Compute the real min and max of the rectangle on both axes
	T minX = std::min(topleft.x, static_cast<T>(topleft.x + size.x));
	T maxX = std::max(topleft.x, static_cast<T>(topleft.x + size.x));
	T minY = std::min(topleft.y, static_cast<T>(topleft.y + size.y));
	T maxY = std::max(topleft.y, static_cast<T>(topleft.y + size.y));

	return (point.x >= minX) and (point.x < maxX) and (point.y >= minY) and (point.y < maxY);
}

// Helper function for Rect<T>::intersects_rect and Rect<T>::get_intersection_with_rect - compute the intersection boundaries
template<typename T>
inline intersection_boundaries<T> Rect<T>::compute_intersection(const Rect<T>& other_rect) const
{
	// Rectangles with negative dimensions are allowed, so we must handle them correctly

	// Compute the min and max of the first rectangle on both axes
	T rect1MinX = std::min(topleft.x, static_cast<T>(topleft.x + size.x));
	T rect1MaxX = std::max(topleft.x, static_cast<T>(topleft.x + size.x));
	T rect1MinY = std::min(topleft.y, static_cast<T>(topleft.y + size.y));
	T rect1MaxY = std::max(topleft.y, static_cast<T>(topleft.y + size.y));

	// Compute the min and max of the second rectangle on both axes
	T rect2MinX = std::min(other_rect.topleft.x, static_cast<T>(other_rect.topleft.x + other_rect.size.x));
	T rect2MaxX = std::max(other_rect.topleft.x, static_cast<T>(other_rect.topleft.x + other_rect.size.x));
	T rect2MinY = std::min(other_rect.topleft.y, static_cast<T>(other_rect.topleft.y + other_rect.size.y));
	T rect2MaxY = std::max(other_rect.topleft.y, static_cast<T>(other_rect.topleft.y + other_rect.size.y));

	// Compute the intersection boundaries
	T intersectionLeft = std::max(rect1MinX, rect2MinX);
	T intersectionTop = std::max(rect1MinY, rect2MinY);
	T intersectionRight = std::min(rect1MaxX, rect2MaxX);
	T intersectionBottom = std::min(rect1MaxY, rect2MaxY);

	return intersection_boundaries<T>{intersectionLeft, intersectionTop, intersectionRight, intersectionBottom};
}

template <typename T>
bool Rect<T>::intersects_rect(const Rect<T>& other_rect) const
{
	intersection_boundaries<T> boundaries = compute_intersection(other_rect);

	// If the intersection is valid (positive non-zero area), then there is an intersection
	return ((boundaries.intersectionLeft < boundaries.intersectionRight) and (boundaries.intersectionTop < boundaries.intersectionBottom));
}

template <typename T>
std::optional<Rect<T>> Rect<T>::get_intersection_with_rect(const Rect<T>& other_rect) const
{
	intersection_boundaries<T> boundaries = compute_intersection(other_rect);

	// If the intersection is valid (positive non-zero area), then there is an intersection
	if ((boundaries.intersectionLeft < boundaries.intersectionRight) and (boundaries.intersectionTop < boundaries.intersectionBottom))
	{
		return Rect<T>(boundaries.intersectionLeft,
					   boundaries.intersectionTop,
					   boundaries.intersectionRight - boundaries.intersectionLeft,
					   boundaries.intersectionBottom - boundaries.intersectionTop);
	}
	else
	{
		return std::nullopt;
	}
}

template <typename T>
bool Rect<T>::intersects_line_segment(Vector2<T> segment_point1,
									  Vector2<T> segment_point2) const
{
	Vector2d rect_bottomright{topleft.x + size.x,
							  topleft.y + size.y};

	// Find min and max X for the segment

	double minX = segment_point1.x;
	double maxX = segment_point2.x;

	if (segment_point1.x > segment_point2.x)
	{
		minX = segment_point2.x;
		maxX = segment_point1.x;
	}

	// Find the intersection of the segment's and rectangle's x-projections

	if (maxX > rect_bottomright.x)
	{
		maxX = rect_bottomright.x;
	}

	if (minX < topleft.x)
	{
		minX = topleft.x;
	}

	if (minX > maxX) // If their X-projections do not intersect, return false
	{
		return false;
	}

	// Find corresponding min and max Y for min and max X we found before

	double minY = segment_point1.y;
	double maxY = segment_point2.y;

	double dx = segment_point2.x - segment_point1.x;

	if (abs(dx) > 0.0000001)
	{
		double segment_slope = (segment_point2.y - segment_point1.y) / dx;
		double segment_y_intercept = segment_point1.y - segment_slope * segment_point1.x;
		minY = segment_slope * minX + segment_y_intercept;
		maxY = segment_slope * maxX + segment_y_intercept;
	}

	if (minY > maxY)
	{
		double tmp = maxY;
		maxY = minY;
		minY = tmp;
	}

	// Find the intersection of the segment's and rectangle's y-projections

	if (maxY > rect_bottomright.y)
	{
		maxY = rect_bottomright.y;
	}

	if (minY < topleft.y)
	{
		minY = topleft.y;
	}

	if (minY > maxY) // If their Y-projections do not intersect, return false
	{
		return false;
	}

	return true;
}

template <typename T>
Rect<T> get_bounding_box(const std::array<Vector2<T>, 4>& points)
{
	// Compute the bounding rectangle of the 4 points
	T left = points[0].x;
	T top = points[0].y;
	T right = points[0].x;
	T bottom = points[0].y;

	for (int i = 1; i < 4; ++i)
	{
		if (points[i].x < left)   left = points[i].x;
		else if (points[i].x > right)  right = points[i].x;
		if (points[i].y < top)    top = points[i].y;
		else if (points[i].y > bottom) bottom = points[i].y;
	}

	return Rect<T>({left, top}, {right - left, bottom - top});
}