#pragma once

class Render
{
private:
	std::uint32_t Base;
	bool DrawingEnabled;

	float PointSize;
	bool PointSmooth;
	bool GLTexture2D;
	bool GLRectangleTexture;

public:
	Render(HDC DC, int FontSize);
	~Render();

	void EnableDrawing();
	void DisableDrawing();
	void EnableTransparency();
	void DisableTransparency();
	void OrthoProjection(int Width, int Height);

	std::uint32_t LoadTexture(GLenum Target, void* Buffer, int width, int height) const;
	void DrawLine(float X1, float Y1, float X2, float Y2) const;
	void DrawRect(float X1, float Y1, float X2, float Y2, bool Fill = true) const;
	void DrawPoint(float X, float Y, float Z, float Radius) const;
	void DrawCircle(float CenterX, float CenterY, float Radius, int Rotations = 360) const;
	void DrawTexture(std::uint32_t Target, std::uint32_t ID, float X1, float Y1, float X2, float Y2, int Width = 1, int Height = 1) const;

	template<typename... Args>
	void Print(float X, float Y, const char* Text, Args... args);

	template<typename... Args>
	void Print(float X, float Y, float R, float G, float B, const char* Text, Args... args);
};

template<typename... Args>
void Render::Print(float X, float Y, const char* Text, Args... args)
{
	this->Print(X, Y, 1, 1, 1, Text, std::forward<Args>(args)...);
}

template<typename... Args>
void Render::Print(float X, float Y, float R, float G, float B, const char* Text, Args... args)
{
	using namespace std;
	std::size_t Position = 0;
	std::string Container(Text);
	std::vector<std::string> Arguments;
	std::initializer_list<int> {(Arguments.push_back(std::to_string(args)), 0)...};

	for (auto it = Arguments.begin(); it != Arguments.end(); ++it)
	{
		if ((Position = Container.find("%")) != std::string::npos)
		{
			Container.replace(Position, 1, *it);
		}
	}

	glColor3f(R, G, B);
	glRasterPos2f(X, Y);
	glPushAttrib(GL_LIST_BIT);
	glListBase(this->Base - 32);
	glCallLists(Container.size(), GL_UNSIGNED_BYTE, Container.data());
	glPopAttrib();
	glFlush();
}

