#pragma once
#include <cmath>
#include <algorithm>

#pragma warning(disable : 4834)
#pragma warning (disable : 4099)

#define M_PI       3.14159265358979323846

struct FVector
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	inline FVector()
		: X(0), Y(0), Z(0) {
	}

	inline FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

	__forceinline FVector operator-(const FVector& V) {
		return FVector(X - V.X, Y - V.Y, Z - V.Z);
	}

	__forceinline FVector operator+(const FVector& V) {
		return FVector(X + V.X, Y + V.Y, Z + V.Z);
	}

	__forceinline FVector operator*(float Scale) const {
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	__forceinline FVector operator/(float Scale) const {
		const float RScale = 1.f / Scale;
		return FVector(X * RScale, Y * RScale, Z * RScale);
	}

	__forceinline FVector operator+(float A) const {
		return FVector(X + A, Y + A, Z + A);
	}

	__forceinline FVector operator-(float A) const {
		return FVector(X - A, Y - A, Z - A);
	}

	__forceinline FVector operator*(const FVector& V) const {
		return FVector(X * V.X, Y * V.Y, Z * V.Z);
	}

	__forceinline FVector operator/(const FVector& V) const {
		return FVector(X / V.X, Y / V.Y, Z / V.Z);
	}

	__forceinline float operator|(const FVector& V) const {
		return X * V.X + Y * V.Y + Z * V.Z;
	}

	__forceinline float operator^(const FVector& V) const {
		return X * V.Y - Y * V.X - Z * V.Z;
	}

	__forceinline FVector& operator+=(const FVector& v) {
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	__forceinline FVector& operator-=(const FVector& v) {
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	__forceinline FVector& operator*=(const FVector& v) {
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		return *this;
	}

	__forceinline FVector& operator/=(const FVector& v) {
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		return *this;
	}

	__forceinline bool operator==(const FVector& src) const {
		return (src.X == X) && (src.Y == Y) && (src.Z == Z);
	}

	__forceinline bool operator!=(const FVector& src) const {
		return (src.X != X) || (src.Y != Y) || (src.Z != Z);
	}

	__forceinline float Size() const {
		return sqrt(X * X + Y * Y + Z * Z);
	}

	__forceinline float Size2D() const {
		return sqrt(X * X + Y * Y);
	}

	__forceinline float SizeSquared() const {
		return X * X + Y * Y + Z * Z;
	}

	__forceinline float SizeSquared2D() const {
		return X * X + Y * Y;
	}

	__forceinline float Dot(const FVector& vOther) const {
		const FVector& a = *this;
		return (a.X * vOther.X + a.Y * vOther.Y + a.Z * vOther.Z);
	}

	__forceinline FVector Normalize() {
		FVector vector;
		float length = this->Size();

		if (length != 0) {
			vector.X = X / length;
			vector.Y = Y / length;
			vector.Z = Z / length;
		}
		else
			vector.X = vector.Y = 0.0f;
		vector.Z = 1.0f;

		return vector;
	}

	__forceinline FVector ToRotator() {
		FVector rotator;
		rotator.X = -(float)atan(Z / Size2D()) * (float)(180.0f / M_PI);
		rotator.Y = (float)atan(Y / X) * (float)(180.0f / M_PI);
		rotator.Z = (float)0.f;
		if (X >= 0.f)
			rotator.Y += 180.0f;
		return rotator;
	}

	__forceinline FVector Clamp()
	{
		FVector clamped;
		while (clamped.Y < -180.0f)
			clamped.Y += 360.0f;
		while (clamped.Y > 180.0f)
			clamped.Y -= 360.0f;
		if (clamped.X < -74.0f)
			clamped.X = -74.0f;
		if (clamped.X > 74.0f)
			clamped.X = 74.0f;
		return clamped;
	}
};

struct alignas(16) FPlane : public FVector
{
	float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};

struct alignas(16) FQuat
{
	float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};


struct FMatrix
{
	struct FPlane                                      XPlane;                                                   // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	struct FPlane                                      YPlane;                                                   // 0x0010(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	struct FPlane                                      ZPlane;                                                   // 0x0020(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	struct FPlane                                      WPlane;                                                   // 0x0030(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)

	FMatrix operator*(const FMatrix& pM2)
	{
		FMatrix pOut;
		pOut.XPlane.X = XPlane.X * pM2.XPlane.X + XPlane.Y * pM2.YPlane.X + XPlane.Z * pM2.ZPlane.X + XPlane.W * pM2.WPlane.X;
		pOut.XPlane.Y = XPlane.X * pM2.XPlane.Y + XPlane.Y * pM2.YPlane.Y + XPlane.Z * pM2.ZPlane.Y + XPlane.W * pM2.WPlane.Y;
		pOut.XPlane.Z = XPlane.X * pM2.XPlane.Z + XPlane.Y * pM2.YPlane.Z + XPlane.Z * pM2.ZPlane.Z + XPlane.W * pM2.WPlane.Z;
		pOut.XPlane.W = XPlane.X * pM2.XPlane.W + XPlane.Y * pM2.YPlane.W + XPlane.Z * pM2.ZPlane.W + XPlane.W * pM2.WPlane.W;
		pOut.YPlane.X = YPlane.X * pM2.XPlane.X + YPlane.Y * pM2.YPlane.X + YPlane.Z * pM2.ZPlane.X + YPlane.W * pM2.WPlane.X;
		pOut.YPlane.Y = YPlane.X * pM2.XPlane.Y + YPlane.Y * pM2.YPlane.Y + YPlane.Z * pM2.ZPlane.Y + YPlane.W * pM2.WPlane.Y;
		pOut.YPlane.Z = YPlane.X * pM2.XPlane.Z + YPlane.Y * pM2.YPlane.Z + YPlane.Z * pM2.ZPlane.Z + YPlane.W * pM2.WPlane.Z;
		pOut.YPlane.W = YPlane.X * pM2.XPlane.W + YPlane.Y * pM2.YPlane.W + YPlane.Z * pM2.ZPlane.W + YPlane.W * pM2.WPlane.W;
		pOut.ZPlane.X = ZPlane.X * pM2.XPlane.X + ZPlane.Y * pM2.YPlane.X + ZPlane.Z * pM2.ZPlane.X + ZPlane.W * pM2.WPlane.X;
		pOut.ZPlane.Y = ZPlane.X * pM2.XPlane.Y + ZPlane.Y * pM2.YPlane.Y + ZPlane.Z * pM2.ZPlane.Y + ZPlane.W * pM2.WPlane.Y;
		pOut.ZPlane.Z = ZPlane.X * pM2.XPlane.Z + ZPlane.Y * pM2.YPlane.Z + ZPlane.Z * pM2.ZPlane.Z + ZPlane.W * pM2.WPlane.Z;
		pOut.ZPlane.W = ZPlane.X * pM2.XPlane.W + ZPlane.Y * pM2.YPlane.W + ZPlane.Z * pM2.ZPlane.W + ZPlane.W * pM2.WPlane.W;
		pOut.WPlane.X = WPlane.X * pM2.XPlane.X + WPlane.Y * pM2.YPlane.X + WPlane.Z * pM2.ZPlane.X + WPlane.W * pM2.WPlane.X;
		pOut.WPlane.Y = WPlane.X * pM2.XPlane.Y + WPlane.Y * pM2.YPlane.Y + WPlane.Z * pM2.ZPlane.Y + WPlane.W * pM2.WPlane.Y;
		pOut.WPlane.Z = WPlane.X * pM2.XPlane.Z + WPlane.Y * pM2.YPlane.Z + WPlane.Z * pM2.ZPlane.Z + WPlane.W * pM2.WPlane.Z;
		pOut.WPlane.W = WPlane.X * pM2.XPlane.W + WPlane.Y * pM2.YPlane.W + WPlane.Z * pM2.ZPlane.W + WPlane.W * pM2.WPlane.W;
		return pOut;
	}
};

struct alignas(16) FTransform
{
	struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	struct FVector                                     Translation;                                              // 0x0010(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
	struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET

	FMatrix ToMatrixWithScale()
	{
		FMatrix m;
		m.WPlane.X = Translation.X;
		m.WPlane.Y = Translation.Y;
		m.WPlane.Z = Translation.Z;

		float x2 = Rotation.X + Rotation.X;
		float y2 = Rotation.Y + Rotation.Y;
		float z2 = Rotation.Z + Rotation.Z;

		float xx2 = Rotation.X * x2;
		float yy2 = Rotation.Y * y2;
		float zz2 = Rotation.Z * z2;
		m.XPlane.X = (1.0f - (yy2 + zz2)) * Scale3D.X;
		m.YPlane.Y = (1.0f - (xx2 + zz2)) * Scale3D.Y;
		m.ZPlane.Z = (1.0f - (xx2 + yy2)) * Scale3D.Z;

		float yz2 = Rotation.Y * z2;
		float wx2 = Rotation.W * x2;
		m.ZPlane.Y = (yz2 - wx2) * Scale3D.Z;
		m.YPlane.Z = (yz2 + wx2) * Scale3D.Y;

		float xy2 = Rotation.X * y2;
		float wz2 = Rotation.W * z2;
		m.YPlane.X = (xy2 - wz2) * Scale3D.Y;
		m.XPlane.Y = (xy2 + wz2) * Scale3D.X;

		float xz2 = Rotation.X * z2;
		float wy2 = Rotation.W * y2;
		m.ZPlane.X = (xz2 + wy2) * Scale3D.Z;
		m.XPlane.Z = (xz2 - wy2) * Scale3D.X;

		m.XPlane.W = 0.0f;
		m.YPlane.W = 0.0f;
		m.ZPlane.W = 0.0f;
		m.WPlane.W = 1.0f;

		return m;
	}
};

struct FMinimalViewInfo
{
	class FVector									   Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class FVector                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0024(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              AspectRatio;                                              // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

struct FCameraCacheEntry
{
	float                                          Timestamp;
	unsigned char                                  UnknownData00[0xC];
	struct FMinimalViewInfo                        POV;
};

static FMatrix ToMatrix(FVector rot)
{
	FVector origin = FVector{};
	float radPitch = (rot.X * float(M_PI) / 180.f);
	float radYaw = (rot.Y * float(M_PI) / 180.f);
	float radRoll = (rot.Z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	FMatrix matrix;
	matrix.XPlane.X = CP * CY;
	matrix.XPlane.Y = CP * SY;
	matrix.XPlane.Z = SP;
	matrix.XPlane.W = 0.f;

	matrix.YPlane.X = SR * SP * CY - CR * SY;
	matrix.YPlane.Y = SR * SP * SY + CR * CY;
	matrix.YPlane.Z = -SR * CP;
	matrix.YPlane.W = 0.f;

	matrix.ZPlane.X = -(CR * SP * CY + SR * SY);
	matrix.ZPlane.Y = CY * SR - CR * SP * SY;
	matrix.ZPlane.Z = CR * CP;
	matrix.ZPlane.W = 0.f;

	matrix.WPlane.X = origin.X;
	matrix.WPlane.Y = origin.Y;
	matrix.WPlane.Z = origin.Z;
	matrix.WPlane.W = 1.f;

	return matrix;
}

struct FString
{
	wchar_t szChar[64];
};

struct FText
{
	char szChar[256];
};

typedef enum {
	Root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	cam_bone = 7,
	clavicle_l = 8,
	upperarm_l = 9,
	lowerarm_l = 10,
	hand_l = 11,
	thumb_01_l = 12,
	thumb_02_l = 13,
	thumb_03_l = 14,
	index_01_l = 15,
	index_02_l = 16,
	index_03_l = 17,
	middle_01_l = 18,
	middle_02_l = 19,
	middle_03_l = 20,
	ring_01_l = 21,
	ring_02_l = 22,
	ring_03_l = 23,
	pinky_01_l = 24,
	pinky_02_l = 25,
	pinky_03_l = 26,
	clavicle_r = 27,
	upperarm_r = 28,
	lowerarm_r = 29,
	hand_r = 30,
	thumb_01_r = 31,
	thumb_02_r = 32,
	thumb_03_r = 33,
	index_01_r = 34,
	index_02_r = 35,
	index_03_r = 36,
	middle_01_r = 37,
	middle_02_r = 38,
	middle_03_r = 39,
	ring_01_r = 40,
	ring_02_r = 41,
	ring_03_r = 42,
	pinky_01_r = 43,
	pinky_02_r = 44,
	pinky_03_r = 45,
	thigh_l = 46,
	calf_l = 47,
	foot_l = 48,
	ball_l = 49,
	thigh_r = 50,
	calf_r = 51,
	foot_r = 52,
	ball_r = 53,
	ik_gun = 54,
	ik_target_r = 55,
	ik_target_l = 56,
} BONES;


namespace geo
{
	class vector2
	{
	public:
		vector2()
		{
			x = y = 0.f;
		}

		vector2(float fx, float fy)
		{
			x = fx;
			y = fy;
		}

		float x, y;

		vector2 operator+(const vector2& input) const
		{
			return vector2{ x + input.x, y + input.y };
		}

		vector2 operator-(const vector2& input) const
		{
			return vector2{ x - input.x, y - input.y };
		}

		vector2 operator/(float input) const
		{
			return vector2{ x / input, y / input };
		}

		vector2 operator*(float input) const
		{
			return vector2{ x * input, y * input };
		}

		vector2& operator-=(const vector2& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		vector2& operator/=(float input)
		{
			x /= input;
			y /= input;
			return *this;
		}

		vector2& operator*=(float input)
		{
			x *= input;
			y *= input;
			return *this;
		}

		float length() const
		{
			return std::sqrt((x * x) + (y * y));
		}

		vector2 normalized() const
		{
			return { x / length(), y / length() };
		}

		float dot_product(vector2 input) const
		{
			return (x * input.x) + (y * input.y);
		}

		float distance(vector2 input) const
		{
			return (*this - input).length();
		}

		bool empty() const
		{
			return x == 0.f && y == 0.f;
		}
	};

	using vec2_t = vector2;
}