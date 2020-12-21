#pragma once
#include <GLFW\glfw3.h>

class InputManager
{
private:
    static bool _firstFrame;
    static float _curMouseX;
    static float _curMouseY;
    static float _deltaMouseX;
    static float _deltaMouseY;
    static float _deltaScrollX;
    static float _deltaScrollY;
    static bool _escape;
    static bool _escapePressed;
    static bool _space;
    static bool _spacePressed;
    static bool _tab;
    static bool _tabPressed;
    static bool _w;
    static bool _wPressed;
    static bool _a;
    static bool _aPressed;
    static bool _s;
    static bool _sPressed;
    static bool _d;
    static bool _dPressed;
    static bool _q;
    static bool _qPressed;
    static bool _e;
    static bool _ePressed;
    static bool _n;
    static bool _nPressed;
    static bool _m;
    static bool _mPressed;
    static bool _t;
    static bool _tPressed;
    static bool _z;
    static bool _zPressed;
    static bool _u;
    static bool _uPressed;
	static float _prevTime;
	static float _deltaTime;

    static bool _currentlyScrolling;

public:
    static void Update(double time, GLFWwindow* window);

    // settings
    static const unsigned int SCR_WIDTH = 800;
    static const unsigned int SCR_HEIGHT = 600;

    static void ProcessKeys(GLFWwindow* window);
    static void ProcessMouse(GLFWwindow* window);
    static void ProcessScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void UpdateScroll();
    const static bool Escape();
    const static bool EscapeIsDown();
    const static bool Tab();
    const static bool TabIsDown();
	const static bool Space();
	const static bool SpaceIsDown();
	const static bool W();
	const static bool WIsDown();
	const static bool A();
	const static bool AIsDown();
	const static bool S();
	const static bool SIsDown();
	const static bool D();
	const static bool DIsDown();
	const static bool Q();
	const static bool QIsDown();
	const static bool E();
	const static bool EIsDown();
    const static bool N();
    const static bool NIsDown();
    const static bool M();
    const static bool MIsDown();
    const static bool T();
    const static bool TIsDown();
    const static bool Z();
    const static bool ZIsDown();
    const static bool U();
    const static bool UIsDown();
	const static float DeltaTime();
	const static float CurrentTime();
	const static float DeltaMouseX();
	const static float DeltaMouseY();
	const static float DeltaScrollX();
	const static float DeltaScrollY();

};