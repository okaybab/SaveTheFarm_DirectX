#include "Mathf.h"
#include <algorithm>
#include <cmath>

using namespace GOTOEngine;

float Mathf::Lerp(float a, float b, float t)
{
	t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
	return a + (b - a) * t;


}

float GOTOEngine::Mathf::NormalizeAngle(float angle)
{
	angle = std::fmod(angle, 360.0f);
	if (angle < 0.0f)
		angle += 360.0f;
	return angle;
}

bool GOTOEngine::Mathf::Approximately(float a, float b)
{
	static constexpr float epsilon = std::numeric_limits<float>::epsilon();
	float diff = std::abs(a - b);

	if (diff < epsilon)
		return true;

	return diff < std::max(std::abs(a), std::abs(b)) * epsilon * 8.0f;
}

float GOTOEngine::Mathf::Clamp(float value, float min, float max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

float GOTOEngine::Mathf::Clamp01(float value)
{
	if (value < 0.0f)
		return 0.0f;
	else if (value > 1.0f)
		return 1.0f;
	else
		return value;
}

float GOTOEngine::Mathf::Max(float a, float b)
{
	if (a > b)
		return a;
	else
		return b;
}

float GOTOEngine::Mathf::Min(float a, float b)
{
	if (a < b)
		return a;
	else
		return b;
}

// ----------------------
// Perlin Noise ±¸ÇöºÎ
// ----------------------

void Mathf::InitializePermutation()
{
	if (permutationInitialized)
		return;

	std::array<int, 256> permutation = {
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,
		69,142,8,99,37,240,21,10,23,
		190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,
		168,68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,
		92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
		80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,
		188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,
		124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,
		227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,
		152,2,44,154,163,70,221,153,101,155,167,
		43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
		178,185,112,104,218,246,97,228,251,34,242,193,238,210,
		144,12,191,179,162,241,81,51,145,235,249,14,239,107,
		49,192,214,31,181,199,106,157,184,84,204,176,115,
		121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,
		24,72,243,141,128,195,78,66,215,61,156,180
	};

	for (int i = 0; i < 256; ++i)
	{
		p[i] = permutation[i];
		p[256 + i] = permutation[i];
	}

	permutationInitialized = true;
}

float GOTOEngine::Mathf::Atan2(float x, float y)
{
	return std::atan2(x, y);
}

float Mathf::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float Mathf::LerpInternal(float a, float b, float t)
{
	return a + t * (b - a);
}

float Mathf::Grad(int hash, float x, float y)
{
	int h = hash & 7;
	float u = h < 4 ? x : y;
	float v = h < 4 ? y : x;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float Mathf::PerlinNoise(float x, float y)
{
	InitializePermutation();

	int X = static_cast<int>(std::floor(x)) & 255;
	int Y = static_cast<int>(std::floor(y)) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	float u = Fade(xf);
	float v = Fade(yf);

	int aa = p[p[X] + Y];
	int ab = p[p[X] + Y + 1];
	int ba = p[p[X + 1] + Y];
	int bb = p[p[X + 1] + Y + 1];

	float x1 = LerpInternal(Grad(aa, xf, yf), Grad(ba, xf - 1, yf), u);
	float x2 = LerpInternal(Grad(ab, xf, yf - 1), Grad(bb, xf - 1, yf - 1), u);

	float result = LerpInternal(x1, x2, v);

	return (result + 1.0f) * 0.5f;
}

float Mathf::PI = 3.14159265358979323846f;
float Mathf::Deg2Rad = 3.1415926535f / 180.0f;
float Mathf::Rad2Deg = 180.0f / 3.1415926535f;
std::array<int, 512> Mathf::p;
bool Mathf::permutationInitialized = false;
