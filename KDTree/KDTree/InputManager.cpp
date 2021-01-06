#include "InputManager.h"

bool InputManager::_firstFrame = true;
float InputManager::_curMouseX = 0;
float InputManager::_curMouseY = 0;
float InputManager::_deltaMouseX = 0;
float InputManager::_deltaMouseY = 0;
float InputManager::_deltaScrollX = 0;
float InputManager::_deltaScrollY = 0;
bool InputManager::_escape = false;
bool InputManager::_escapePressed = false;
bool InputManager::_space = false;
bool InputManager::_spacePressed = false;
bool InputManager::_tab = false;
bool InputManager::_tabPressed = false;
bool InputManager::_w = false;
bool InputManager::_wPressed = false;
bool InputManager::_a = false;
bool InputManager::_aPressed = false;
bool InputManager::_s = false;
bool InputManager::_sPressed = false;
bool InputManager::_d = false;
bool InputManager::_dPressed = false;
bool InputManager::_q = false;
bool InputManager::_qPressed = false;
bool InputManager::_e = false;
bool InputManager::_ePressed = false;
bool InputManager::_rmb = false;
bool InputManager::_rmbPressed = false;
float InputManager::_prevTime = 0;
float InputManager::_deltaTime = 0;
bool InputManager::_currentlyScrolling = false;

void InputManager::Update(double time, GLFWwindow* window)
{
    float curTime = time;
	InputManager::_deltaTime = curTime - _prevTime;
	InputManager::_prevTime = curTime;
    ProcessKeys(window);
    ProcessMouse(window);
    UpdateScroll();
}


void InputManager::ProcessKeys(GLFWwindow* window)
{
    _escapePressed = _escape;
    _tabPressed = _tab;
    _spacePressed = _space;
    _wPressed = _w;
    _aPressed = _a;
    _sPressed = _s;
    _dPressed = _d;
    _qPressed = _q;
    _ePressed = _e;
    _rmbPressed = _rmb;
    
    _escape = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    _tab = (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS);
    _space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    _w = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    _a = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    _s = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    _d = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    _q = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS);
    _e = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
    _rmb = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);
}

void InputManager::ProcessMouse(GLFWwindow* window)
{
    double* xPos = new double();
    double* yPos = new double();
    glfwGetCursorPos(window, xPos, yPos);
    if (_firstFrame)
    {
        _curMouseX = *xPos;
        _curMouseY = *yPos;
        _firstFrame = false;
    }

    _deltaMouseX = *xPos - _curMouseX;
    _deltaMouseY = _curMouseY - *yPos; // reversed since y-coordinates go from bottom to top
    _curMouseX = *xPos;
    _curMouseY = *yPos;
    delete xPos;
    delete yPos;
}

void InputManager::ProcessScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    _currentlyScrolling = true;
    _deltaScrollX = xoffset;
    _deltaScrollY = yoffset;
}

void InputManager::UpdateScroll()
{
    if (_deltaScrollX != 0 || _deltaScrollY != 0) {
        if (!_currentlyScrolling) {
            _deltaScrollX = 0;
            _deltaScrollY = 0;
        }
        else {
            _currentlyScrolling = 0;
        }
    }
}

const bool InputManager::Escape()
{
    return _escape&&!_escapePressed;
}

const bool InputManager::EscapeIsDown()
{
    return _escape;
}

const bool InputManager::Tab()
{
    return _tab && !_tabPressed;
}

const bool InputManager::TabIsDown()
{
    return _tab;
}

const bool InputManager::Space()
{
    return _space&&!_spacePressed;
}

const bool InputManager::SpaceIsDown()
{
    return _space;
}

const bool InputManager::W()
{
    return _w&&!_wPressed;
}

const bool InputManager::WIsDown()
{
    return _w;
}

const bool InputManager::A()
{
    return _a&&!_aPressed;
}

const bool InputManager::AIsDown()
{
    return _a;
}

const bool InputManager::S()
{
    return _s&&!_sPressed;
}

const bool InputManager::SIsDown()
{
    return _s;
}

const bool InputManager::D()
{
    return _d&&!_dPressed;
}

const bool InputManager::DIsDown()
{
    return _d;
}

const bool InputManager::Q()
{
    return _q && !_qPressed;;
}

const bool InputManager::QIsDown()
{
    return _q;
}

const bool InputManager::E()
{
    return _e && !_ePressed;
}

const bool InputManager::EIsDown()
{
    return _e;
}

const bool InputManager::RMB()
{
    return _rmb && !_rmbPressed;
}

const bool InputManager::RMBIsDown()
{
    return _rmb;
}
const float InputManager::DeltaTime()
{
    return _deltaTime;
}

const float InputManager::CurrentTime()
{
    return _prevTime;
}

const float InputManager::DeltaMouseX()
{
    return _deltaMouseX;
}

const float InputManager::DeltaMouseY()
{
    return _deltaMouseY;
}

const float InputManager::DeltaScrollX()
{
    return _deltaScrollX;
}

const float InputManager::DeltaScrollY()
{
    return _deltaScrollY;
}