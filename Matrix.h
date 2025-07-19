#pragma once

#include "Vector2.h"

// double precision 3x3 matrix
class Matrix3x3d final
{
public:
	std::array<std::array<double, 3>, 3> matrix;

	Matrix3x3d()
	{}

	Matrix3x3d(const int matrix_type)
	{
		if (matrix_type == 1)	// identity matrix
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					if (i == j)
					{
						matrix[i][j] = 1.0;
					}
					else
					{
						matrix[i][j] = 0.0;
					}
				}
			}
		}
		else if (matrix_type == 0)	// zero matrix
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					matrix[i][j] = 0.0;
				}
			}
		}
	}

	Matrix3x3d(const std::array<int, 9> array_arg)
	{
		int k = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] = array_arg[k];
				++k;
			}
		}
	}

	// matrix - matrix operators

	Matrix3x3d& operator+=(const Matrix3x3d& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] += rhs.matrix[i][j];
			}
		}
		return *this;
	}
	friend Matrix3x3d operator+(Matrix3x3d lhs, const Matrix3x3d& rhs)
	{
		return lhs += rhs;
	}

	Matrix3x3d& operator-=(const Matrix3x3d& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] -= rhs.matrix[i][j];
			}
		}
		return *this;
	}
	friend Matrix3x3d operator-(Matrix3x3d lhs, const Matrix3x3d& rhs)
	{
		return lhs -= rhs;
	}

	Matrix3x3d& operator*=(const Matrix3x3d& rhs)
	{
		Matrix3x3d Result(0);

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					Result.matrix[i][j] += matrix[i][k] * rhs.matrix[k][j];
				}
			}
		}

		*this = Result;
		return *this;
	}
	friend Matrix3x3d operator*(Matrix3x3d lhs, const Matrix3x3d& rhs)
	{
		return lhs *= rhs;
	}

	Matrix3x3d& operator/=(const Matrix3x3d& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] /= rhs.matrix[i][j];
			}
		}
		return *this;
	}
	friend Matrix3x3d operator/(Matrix3x3d lhs, const Matrix3x3d& rhs)
	{
		return lhs /= rhs;
	}

	// matrix - scalar operations

	Matrix3x3d& operator+=(const double& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] += rhs;
			}
		}
		return *this;
	}
	friend Matrix3x3d operator+(Matrix3x3d lhs, const double& rhs)
	{
		return lhs += rhs;
	}
	friend Matrix3x3d operator+(const double& lhs, Matrix3x3d rhs)
	{
		return rhs += lhs;
	}

	Matrix3x3d& operator-=(const double& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] -= rhs;
			}
		}
		return *this;
	}
	friend Matrix3x3d operator-(Matrix3x3d lhs, const double& rhs)
	{
		return lhs -= rhs;
	}
	friend Matrix3x3d operator-(const double& lhs, Matrix3x3d rhs)
	{
		return rhs -= lhs;
	}

	Matrix3x3d& operator*=(const double& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] *= rhs;
			}
		}
		return *this;
	}
	friend Matrix3x3d operator*(Matrix3x3d lhs, const double& rhs)
	{
		return lhs *= rhs;
	}
	friend Matrix3x3d operator*(const double& lhs, Matrix3x3d rhs)
	{
		return rhs *= lhs;
	}

	Matrix3x3d& operator/=(const double& rhs)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				matrix[i][j] /= rhs;
			}
		}
		return *this;
	}
	friend Matrix3x3d operator/(Matrix3x3d lhs, const double& rhs)
	{
		return lhs /= rhs;
	}
	friend Matrix3x3d operator/(const double& lhs, Matrix3x3d rhs)
	{
		return rhs /= lhs;
	}

	// Member functions

	void Print() const
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				print("{}  ", matrix[i][j]);
				if (j == 2)
				{
					print(NEWL);
				}
			}
		}
		print(NEWL);
	}
};

// matrix inverse, transpose, scaling, translation, rotation, vector transform:

Matrix3x3d GetInverse(const Matrix3x3d& matrix_param)
{
	const double det =	matrix_param.matrix[0][0] * (matrix_param.matrix[1][1] * matrix_param.matrix[2][2] - matrix_param.matrix[2][1] * matrix_param.matrix[1][2]) -
						matrix_param.matrix[0][1] * (matrix_param.matrix[1][0] * matrix_param.matrix[2][2] - matrix_param.matrix[1][2] * matrix_param.matrix[2][0]) +
						matrix_param.matrix[0][2] * (matrix_param.matrix[1][0] * matrix_param.matrix[2][1] - matrix_param.matrix[1][1] * matrix_param.matrix[2][0]);

	const double invdet = 1 / det;

	Matrix3x3d Result(0); // inverse of matrix matrix_param
	Result.matrix[0][0] = (matrix_param.matrix[1][1] * matrix_param.matrix[2][2] - matrix_param.matrix[2][1] * matrix_param.matrix[1][2]) * invdet;
	Result.matrix[0][1] = (matrix_param.matrix[0][2] * matrix_param.matrix[2][1] - matrix_param.matrix[0][1] * matrix_param.matrix[2][2]) * invdet;
	Result.matrix[0][2] = (matrix_param.matrix[0][1] * matrix_param.matrix[1][2] - matrix_param.matrix[0][2] * matrix_param.matrix[1][1]) * invdet;
	Result.matrix[1][0] = (matrix_param.matrix[1][2] * matrix_param.matrix[2][0] - matrix_param.matrix[1][0] * matrix_param.matrix[2][2]) * invdet;
	Result.matrix[1][1] = (matrix_param.matrix[0][0] * matrix_param.matrix[2][2] - matrix_param.matrix[0][2] * matrix_param.matrix[2][0]) * invdet;
	Result.matrix[1][2] = (matrix_param.matrix[1][0] * matrix_param.matrix[0][2] - matrix_param.matrix[0][0] * matrix_param.matrix[1][2]) * invdet;
	Result.matrix[2][0] = (matrix_param.matrix[1][0] * matrix_param.matrix[2][1] - matrix_param.matrix[2][0] * matrix_param.matrix[1][1]) * invdet;
	Result.matrix[2][1] = (matrix_param.matrix[2][0] * matrix_param.matrix[0][1] - matrix_param.matrix[0][0] * matrix_param.matrix[2][1]) * invdet;
	Result.matrix[2][2] = (matrix_param.matrix[0][0] * matrix_param.matrix[1][1] - matrix_param.matrix[1][0] * matrix_param.matrix[0][1]) * invdet;

	return Result;
}

Matrix3x3d GetTranspose(const Matrix3x3d& matrix_param)
{
	Matrix3x3d Result(0);
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Result.matrix[j][i] = matrix_param.matrix[i][j];
		}
	}
	return Result;
}

Matrix3x3d GetScalingMatrix(const Vector2d& sizes)
{
	Matrix3x3d Result(1);

	Result.matrix[0][0] = sizes.x;
	Result.matrix[1][1] = sizes.y;

	return Result;
}

Matrix3x3d GetTranslationMatrix(const Vector2d& offsets)
{
	Matrix3x3d Result(1);

	Result.matrix[0][2] = offsets.x;
	Result.matrix[1][2] = offsets.y;

	return Result;
}

Matrix3x3d GetRotationMatrixY(const double angle_rad) // in radians
{
	Matrix3x3d Result(1);

	Result.matrix[0][0] = cos(angle_rad);
	Result.matrix[0][1] = -sin(angle_rad);
	Result.matrix[1][0] = sin(angle_rad);
	Result.matrix[1][1] = cos(angle_rad);

	return Result;
}

Vector2d TransformVector(const Vector2d& vector, const Matrix3x3d& matrix)
{
	return Vector2d(matrix.matrix[0][0] * vector.x + matrix.matrix[0][1] * vector.y + matrix.matrix[0][2],
					matrix.matrix[1][0] * vector.x + matrix.matrix[1][1] * vector.y + matrix.matrix[1][2]);
}