#pragma once

#include "Utilities.h"

///  Class template for manipulating 2-dimensional vectors
template <typename T>
class Vector2
{
public:
	T x;
	T y;

	// Constructors

	constexpr Vector2()
	{}

	constexpr Vector2(T X, T Y) :
		x(X),
		y(Y)
	{}

	// Template constructor to copy from a Vector2 with a different template argument U
	template <typename U>
	constexpr Vector2(const Vector2<U>& other) :
		x(static_cast<T>(other.x)),
		y(static_cast<T>(other.y))
	{}

	// Member functions

	string get_string() const
	{
		return "x: " + to_string(x) + " | " + "y: " + to_string(y);
	}

	T get_length() const
	{
		return sqrt((x * x) + (y * y));
	}

	// more efficient, use for comparisons
	T get_squared_length() const
	{
		return (x * x) + (y * y);
	}

	Vector2<T> get_normalized() const
	{
		const T length = get_length();
		return Vector2<T>(x / length, y / length);
	}

	void normalize()
	{
		const T length = get_length();
		x = x / length;
		y = y / length;
	}

	Vector2<T> get_resized(T new_size)
	{
		Vector2<T> norm = get_normalized();
		norm.x = norm.x * new_size;
		norm.y = norm.y * new_size;
		return norm;
	}

	void resize(T new_size)
	{
		normalize();
		x = x * new_size;
		y = y * new_size;
	}

	//angle in degrees
	Vector2<T> get_rotated(const T angle) const
	{
		T ang_rad = deg_to_rad(angle);

		return Vector2<T>{cos(ang_rad)* x - sin(ang_rad) * y,
			sin(ang_rad)* x + cos(ang_rad) * y};
	}

	//angle in degrees
	void rotate(const T angle)
	{
		T ang_rad = deg_to_rad(angle);

		T x_rotated = cos(ang_rad) * x - sin(ang_rad) * y;
		T y_rotated = sin(ang_rad) * x + cos(ang_rad) * y;

		x = x_rotated;
		y = y_rotated;
	}

	Vector2<T> get_perpendicular_right() const
	{
		return Vector2<T>(y, -x);
	}

	Vector2<T> get_perpendicular_left() const
	{
		return Vector2<T>(-y, x);
	}

	Vector2<T> get_negated() const
	{
		return Vector2<T>(-x, -y);
	}

	void negate()
	{
		x = -x;
		y = -y;
	}
};

// Operator definitions

/// \relates Vector2
/// \brief Overload of unary operator -
///
/// \param right Vector to negate
///
/// \return Memberwise opposite of the vector
template <typename T>
Vector2<T> operator -(const Vector2<T>& right)
{
	return Vector2<T>(-right.x, -right.y);
}

/// \relates Vector2
/// \brief Overload of binary operator +=
///
/// This operator performs a memberwise addition of both vectors,
/// and assigns the result to \a left.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Reference to \a left
template <typename T>
Vector2<T>& operator +=(Vector2<T>& left, const Vector2<T>& right)
{
	left.x += right.x;
	left.y += right.y;

	return left;
}

/// \relates Vector2
/// \brief Overload of binary operator -=
///
/// This operator performs a memberwise subtraction of both vectors,
/// and assigns the result to \a left.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Reference to \a left
template <typename T>
Vector2<T>& operator -=(Vector2<T>& left, const Vector2<T>& right)
{
	left.x -= right.x;
	left.y -= right.y;

	return left;
}

/// \relates Vector2
/// \brief Overload of binary operator +
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Memberwise addition of both vectors
template <typename T>
Vector2<T> operator +(const Vector2<T>& left, const Vector2<T>& right)
{
	return Vector2<T>(left.x + right.x, left.y + right.y);
}

/// \relates Vector2
/// \brief Overload of binary operator -
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return Memberwise subtraction of both vectors
template <typename T>
Vector2<T> operator -(const Vector2<T>& left, const Vector2<T>& right)
{
	return Vector2<T>(left.x - right.x, left.y - right.y);
}

/// \relates Vector2
/// \brief Overload of binary operator *
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Memberwise multiplication by \a right
template <typename T>
Vector2<T> operator *(const Vector2<T>& left, T right)
{
	return Vector2<T>(left.x * right, left.y * right);
}

/// \relates Vector2
/// \brief Overload of binary operator *
///
/// \param left  Left operand (a scalar value)
/// \param right Right operand (a vector)
///
/// \return Memberwise multiplication by \a left
template <typename T>
Vector2<T> operator *(T left, const Vector2<T>& right)
{
	return Vector2<T>(right.x * left, right.y * left);
}

/// \relates Vector2
/// \brief Overload of binary operator *=
///
/// This operator performs a memberwise multiplication by \a right,
/// and assigns the result to \a left.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Reference to \a left
template <typename T>
Vector2<T>& operator *=(Vector2<T>& left, T right)
{
	left.x *= right;
	left.y *= right;

	return left;
}

/// \relates Vector2
/// \brief Overload of binary operator /
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Memberwise division by \a right
template <typename T>
Vector2<T> operator /(const Vector2<T>& left, T right)
{
	return Vector2<T>(left.x / right, left.y / right);
}

/// \relates Vector2
/// \brief Overload of binary operator /=
///
/// This operator performs a memberwise division by \a right,
/// and assigns the result to \a left.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a scalar value)
///
/// \return Reference to \a left
template <typename T>
Vector2<T>& operator /=(Vector2<T>& left, T right)
{
	left.x /= right;
	left.y /= right;

	return left;
}

/// \relates Vector2
/// \brief Overload of binary operator ==
///
/// This operator compares strict equality between two vectors.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return True if \a left is equal to \a right
template <typename T>
bool operator ==(const Vector2<T>& left, const Vector2<T>& right)
{
	return (left.x == right.x) and (left.y == right.y);
}

/// \relates Vector2
/// \brief Overload of binary operator !=
///
/// This operator compares strict difference between two vectors.
///
/// \param left  Left operand (a vector)
/// \param right Right operand (a vector)
///
/// \return True if \a left is not equal to \a right
template <typename T>
bool operator !=(const Vector2<T>& left, const Vector2<T>& right)
{
	return (left.x != right.x) or (left.y != right.y);
}

// Define the most common types
typedef Vector2<int>          Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<float>        Vector2f;
typedef Vector2<double>       Vector2d;

// Vector functions:

//angle in radians to vector
template <typename T>
Vector2<T> angle_rad_to_vector(T angle)
{
	Vector2<T> result{cos(angle), sin(angle)};
	return result;
}

//vector to angle in radians
template <typename T>
T vector_to_angle_rad(Vector2<T> vect)
{
	T angle = atan2(vect.y, vect.x);
	return angle;
}

// vector_dot_product product: returns positive or negative T, 0.0 for perpendicular
template <typename T>
T vector_dot_product(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return (vector1.x * vector2.x) + (vector1.y * vector2.y);
}

// vector_cross_product product: returns positive or negative T, 0.0 for parallel / antiparallel
template <typename T>
T vector_cross_product(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return (vector1.x * vector2.y) - (vector1.y * vector2.x);
}

// returns smallest angle between two 2d vectors in radians, it lies between 0 and pi radians (0-180°)
template <typename T>
T angle_between_vectors_rad(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return std::atan2(vector_cross_product(vector1, vector2), vector_dot_product(vector1, vector2));
}

// returns smallest angle between two 2d vectors in degrees, it lies between 0 and 180° (0-pi rad)
template <typename T>
T angle_between_vectors_deg(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return rad_to_deg(std::atan2(vector_cross_product(vector1, vector2), vector_dot_product(vector1, vector2)));
}

template <typename T>
T angle_between_vectors_signed_rad(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return std::atan2(vector1.x * vector2.y - vector1.y * vector2.x, vector1.x * vector2.x + vector1.y * vector2.y);
}

template <typename T>
T angle_between_vectors_signed_deg(const Vector2<T>& vector1, const Vector2<T>& vector2)
{
	return rad_to_deg(std::atan2(vector1.x * vector2.y - vector1.y * vector2.x, vector1.x * vector2.x + vector1.y * vector2.y));
}

// Calculate the distance between two points
template <typename T>
T point_distance(const Vector2<T>& point1, const Vector2<T>& point2)
{
	return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
}

//squared distance between 2 points - more efficient, use for comparisons
template <typename T>
T point_squared_distance(const Vector2<T>& point1, const Vector2<T>& point2)
{
	return (point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y);
}

//vector projected on an axis. Axis must never be zero!!! If it is, random axis is generated
template <typename T>
Vector2<T> vector_projected(const Vector2<T>& vect, Vector2<T> axis)
{
	if (axis.x == 0 and axis.y == 0)
	{
		axis = Vector2<T>(get_random_real_in_range(-2.0, 2.0), get_random_real_in_range(-2.0, 2.0));
	}

	return vector_dot_product(vect, axis) / (axis).get_squared_length() * axis;
}

template <typename T>
bool line_intersects_circle(const Vector2<T>& line_point1, const Vector2<T>& line_point2, const Vector2<T>& circle_center, const T circle_radius)
{
	const Vector2<T> line_point1_norm = line_point1 - circle_center;
	const Vector2<T> line_point2_norm = line_point2 - circle_center;

	const T dx = line_point2_norm.x - line_point1_norm.x;
	const T dy = line_point2_norm.y - line_point1_norm.y;
	const T D = (line_point1_norm.x * line_point2_norm.y) - (line_point2_norm.x * line_point1_norm.y);

	const T incidence = circle_radius * circle_radius * (dx * dx + dy * dy) - D * D;

	return (incidence < 0);
}

template <typename T>
bool line_segment_intersects_circle(Vector2<T> line_point1, Vector2<T> line_point2, Vector2<T> circle_center, T circle_radius)
{
	Vector2<T> segment_vector = line_point2 - line_point1;
	T segment_length = segment_vector.get_length();
	Vector2<T> segment_normalized = segment_vector.get_normalized();
	Vector2<T> segment_start_to_circle = circle_center - line_point1;
	T closest_point_on_segment = vector_dot_product(segment_start_to_circle, segment_vector) / segment_length;

	Vector2<T> closest;
	if (closest_point_on_segment < 0)
	{
		closest = line_point1;
	}
	else if (closest_point_on_segment > segment_length)
	{
		closest = line_point2;
	}
	else
	{
		closest = line_point1 + (closest_point_on_segment * segment_normalized);
	}

	Vector2<T> circle_to_closest = circle_center - closest;
	if (circle_to_closest.get_length() > circle_radius)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// Calculates the closest point on a line segment to a given point.
template <typename T>
Vector2<T> findClosestPointOnLineSegment(const Vector2<T>& segmentStart, const Vector2<T>& segmentEnd, const Vector2<T>& externalPoint)
{
	// Vector representing the direction of the line segment
	Vector2<T> segmentDirection;
	segmentDirection.x = segmentEnd.x - segmentStart.x;
	segmentDirection.y = segmentEnd.y - segmentStart.y;

	// Vector from the segment start to the external point
	Vector2<T> startToPoint;
	startToPoint.x = externalPoint.x - segmentStart.x;
	startToPoint.y = externalPoint.y - segmentStart.y;

	// Dot product of the segment direction and the start-to-point vector
	T dotProduct = segmentDirection.x * startToPoint.x + segmentDirection.y * startToPoint.y;

	// If the dot product is negative, the closest point is the segment start
	if (dotProduct <= 0.0)
	{
		return segmentStart;
	}

	// Squared magnitude of the segment direction vector
	T segmentMagnitudeSquared = segmentDirection.x * segmentDirection.x + segmentDirection.y * segmentDirection.y;

	// If the dot product is greater than the squared magnitude, the closest point is the segment end
	if (segmentMagnitudeSquared <= dotProduct)
	{
		return segmentEnd;
	}

	// Calculate the interpolation ratio for the closest point
	T interpolationRatio = dotProduct / segmentMagnitudeSquared;

	// Calculate the coordinates of the closest point
	Vector2<T> closestPoint;
	closestPoint.x = segmentStart.x + interpolationRatio * segmentDirection.x;
	closestPoint.y = segmentStart.y + interpolationRatio * segmentDirection.y;

	return closestPoint;
}

// Calculates the closest point on an infinite line to a given point.
template <typename T>
Vector2<T> findClosestPointOnLine(const Vector2<T>& linePoint1, const Vector2<T>& linePoint2, const Vector2<T>& externalPoint)
{
	// Vector representing the direction of the line
	Vector2<T> lineDirection;
	lineDirection.x = linePoint2.x - linePoint1.x;
	lineDirection.y = linePoint2.y - linePoint1.y;

	// Vector from the first line point to the external point
	Vector2<T> point1ToExternal;
	point1ToExternal.x = externalPoint.x - linePoint1.x;
	point1ToExternal.y = externalPoint.y - linePoint1.y;

	// Dot product of the line direction and the point-to-external vector
	T dotProduct = lineDirection.x * point1ToExternal.x + lineDirection.y * point1ToExternal.y;

	// Squared magnitude of the line direction vector
	T lineMagnitudeSquared = lineDirection.x * lineDirection.x + lineDirection.y * lineDirection.y;

	// Calculate the projection ratio along the line
	T projectionRatio = dotProduct / lineMagnitudeSquared;

	// Calculate the coordinates of the closest point
	Vector2<T> closestPoint;
	closestPoint.x = linePoint1.x + projectionRatio * lineDirection.x;
	closestPoint.y = linePoint1.y + projectionRatio * lineDirection.y;

	return closestPoint;
}

// Interpolates a point along a line segment based on a given ratio
template <typename T>
Vector2<T> interpolatePointOnLineSegment(const Vector2<T>& segmentStart, const Vector2<T>& segmentEnd, const T interpolationRatio)
{
	Vector2<T> interpolatedPoint;
	interpolatedPoint.x = segmentStart.x + interpolationRatio * (segmentEnd.x - segmentStart.x);
	interpolatedPoint.y = segmentStart.y + interpolationRatio * (segmentEnd.y - segmentStart.y);
	return interpolatedPoint;
}

// Finds a point along a line segment at a specific distance from the start point
template <typename T>
Vector2<T> projectPointAlongLineSegment(const Vector2<T>& segmentStart, const Vector2<T>& segmentEnd, const T distanceFromStart)
{
	T segmentLength = point_distance<T>(segmentStart, segmentEnd);

	if (segmentLength == 0.0)
	{
		return segmentStart;
	}

	T interpolationRatio = distanceFromStart / segmentLength;
	return interpolatePointOnLineSegment<T>(segmentStart, segmentEnd, interpolationRatio);
}

// calculates whether a given point is inside the given circle
template<typename T>
inline bool point_in_circle(Vector2<T> point, Vector2<T> circle_center, T circle_radius)
{
	return point_distance(point, circle_center) <= circle_radius;
}

// Helper function for line_segment_circle_overlap_points.
// Calculates overlap points when one endpoint is inside the circle.
vector<Vector2d> calculate_overlap_points_one_endpoint_inside(const Vector2d& segment_inside_point,
															  const Vector2d& segment_outside_point,
															  const Vector2d& closest_point_on_infinite_line,
															  const Vector2d& circle_center,
															  double radius)
{
	vector<Vector2d> overlap_points;
	overlap_points.push_back(segment_inside_point); // we include the segment endpoint that is inside the circle in the return value

	double center_to_line_dist = point_distance(closest_point_on_infinite_line, circle_center);
	double half_chord_length = sqrt((radius * radius) - (center_to_line_dist * center_to_line_dist));

	Vector2d intersection_point = projectPointAlongLineSegment(closest_point_on_infinite_line,
															   segment_outside_point,
															   half_chord_length);

	// Check to ensure that the calculated intersection point lies on the line segment
	if (point_distance(segment_inside_point, segment_outside_point) >= point_distance(segment_inside_point, intersection_point))
		overlap_points.push_back(intersection_point); // we also return the intersection point

	return overlap_points;
}

// Helper function for line_segment_circle_overlap_points.
// Calculates overlap points when no endpoints are inside the circle.
vector<Vector2d> calculate_overlap_points_no_endpoints_inside(const Vector2d& line_segment_point_1,
															  const Vector2d& line_segment_point_2,
															  const Vector2d& closest_point_on_segment,
															  const Vector2d& circle_center,
															  double radius)
{
	vector<Vector2d> overlap_points;

	if ((line_segment_point_1.x == closest_point_on_segment.x and line_segment_point_1.y == closest_point_on_segment.y) or
		(line_segment_point_2.x == closest_point_on_segment.x and line_segment_point_2.y == closest_point_on_segment.y))
	{
		return overlap_points; // No intersections if the closest point is an endpoint of the segment
	}

	double center_to_segment_dist = point_distance(closest_point_on_segment, circle_center);

	if (center_to_segment_dist > radius)
	{
		return overlap_points; // No overlap
	}
	else if (center_to_segment_dist == radius)
	{
		overlap_points.push_back(closest_point_on_segment); // One intersection (tangent) - point overlap
		return overlap_points;
	}
	else
	{
		double half_chord_length = sqrt((radius * radius) - (center_to_segment_dist * center_to_segment_dist));

		Vector2d intersection1 = projectPointAlongLineSegment(closest_point_on_segment,
															  line_segment_point_1,
															  half_chord_length);
		Vector2d intersection2 = projectPointAlongLineSegment(closest_point_on_segment,
															  line_segment_point_2,
															  half_chord_length);

		overlap_points.push_back(intersection1); // Two intersections - line segment overlap
		overlap_points.push_back(intersection2);
	}

	return overlap_points;
}

// Returns the endpoints of the portion of a line segment that overlaps a circle.
// This includes:
//   - Intersection points between the segment and the circle's circumference.
//   - Endpoints of the segment that lie within the circle.
// Returns:
//   A vector containing the points defining the segment's overlap with the circle.
//   These can be either intersection points or endpoints of the segment that lie
//   within the circle.
vector<Vector2d> line_segment_circle_overlap_points(const Vector2d& line_segment_point_1,
													const Vector2d& line_segment_point_2,
													const Vector2d& circle_center,
													double circle_radius)
{
	vector<Vector2d> overlap_points;

	bool p1_inside = point_in_circle(line_segment_point_1, circle_center, circle_radius);
	bool p2_inside = point_in_circle(line_segment_point_2, circle_center, circle_radius);

	if (p1_inside and p2_inside)
	{
		overlap_points.push_back(line_segment_point_1);
		overlap_points.push_back(line_segment_point_2);
	}
	else if (p1_inside)
	{
		Vector2d closest_point_on_infinite_line = findClosestPointOnLine(line_segment_point_1, line_segment_point_2, circle_center);
		overlap_points = calculate_overlap_points_one_endpoint_inside(line_segment_point_1, line_segment_point_2, closest_point_on_infinite_line, circle_center, circle_radius);
	}
	else if (p2_inside)
	{
		Vector2d closest_point_on_infinite_line = findClosestPointOnLine(line_segment_point_1, line_segment_point_2, circle_center);
		overlap_points = calculate_overlap_points_one_endpoint_inside(line_segment_point_2, line_segment_point_1, closest_point_on_infinite_line, circle_center, circle_radius);
	}
	else
	{
		Vector2d closest_point_on_segment = findClosestPointOnLineSegment(line_segment_point_1, line_segment_point_2, circle_center);
		overlap_points = calculate_overlap_points_no_endpoints_inside(line_segment_point_1, line_segment_point_2, closest_point_on_segment, circle_center, circle_radius);
	}

	return overlap_points;
}

template <typename T>
bool point_in_polygon(const vector<Vector2<T>>& collision_polygon_arg, const Vector2<T> point_arg)
{
	// This function uses the "ray casting" algorithm to determine if a point is inside a polygon.

	const int polygon_side_count = collision_polygon_arg.size();
	int   i, j = polygon_side_count - 1;
	bool  is_inside = false;

	for (i = 0; i < polygon_side_count; ++i)
	{
		if ((collision_polygon_arg[i].y < point_arg.y and collision_polygon_arg[j].y >= point_arg.y
			 or collision_polygon_arg[j].y < point_arg.y and collision_polygon_arg[i].y >= point_arg.y)
			and (collision_polygon_arg[i].x <= point_arg.x or collision_polygon_arg[j].x <= point_arg.x))
		{
			if (collision_polygon_arg[i].x + (point_arg.y - collision_polygon_arg[i].y) / (collision_polygon_arg[j].y - collision_polygon_arg[i].y) * (collision_polygon_arg[j].x - collision_polygon_arg[i].x)
				< point_arg.x)
			{
				is_inside = not is_inside;
			}
		}
		j = i;
	}

	return is_inside;
}

template <typename T>
Vector2<T> random_point_inside_circle(Vector2<T> circle_position, T radius, T inner_exclusion_radius)
{
	Vector2<T> random_offset{get_random_real_in_range(-1.0, 1.0), get_random_real_in_range(-1.0, 1.0)};
	random_offset.resize(get_random_real_in_range(inner_exclusion_radius, radius));

	return circle_position + random_offset;
}

//interpolate from scalar start_value to end_value, according to factor in range 0-1
template <typename T>
T lerp_scalars(T start_value, T end_value, T factor)
{
	return start_value * (1.0 - factor) + end_value * factor;
}

//interpolate from start_vector to end_vector, according to factor in range 0-1
template <typename T>
Vector2<T> lerp_vectors(const Vector2<T> start_vector, const Vector2<T>& end_vector, T factor)
{
	return start_vector + (end_vector - start_vector) * factor;
}

//expects normalized angle in radians
void rotate_point_around_pivot(Vector2d& point, Vector2d pivot, double angle_rad)
{
	const double sin_value = sin(angle_rad);
	const double cos_value = cos(angle_rad);

	// translate point back to origin:
	point.x -= pivot.x;
	point.y -= pivot.y;

	// rotate point
	const double rotated_x = point.x * cos_value - point.y * sin_value;
	const double rotated_y = point.x * sin_value + point.y * cos_value;

	// translate point back:
	point.x = rotated_x + pivot.x;
	point.y = rotated_y + pivot.y;
}

template<typename T>
T point_to_line_segment_distance(Vector2<T> segment_start, Vector2<T> segment_end, Vector2<T> point)
{
	// Return minimum distance between the given line segment and point

	Vector2<T> segment_position_vect = segment_end - segment_start;
	const T segment_length_squared = segment_position_vect.get_squared_length();  // avoid a sqrt

	// Consider the line extending the segment, parameterized as segment_start + projection_factor (segment_end - segment_start).
	// We find projection of the point onto the line. 
	// It falls where projection_factor = [(point - segment_start) . (segment_end - segment_start)] / |segment_end - segment_start|^2
	// We clamp projection_factor from [0,1] to handle points outside the segment.

	const T projection_factor = clamp<T>(vector_dot_product(point - segment_start, segment_position_vect) / segment_length_squared, T(0), T(1));

	const Vector2<T> projected_point = segment_start + projection_factor * segment_position_vect;  // Projected point falls on the segment
	return point_distance(point, projected_point);
}