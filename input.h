#ifndef _INPUT_H
#define _INPUT_H

bool oldKeyboard[GLFW_KEY_LAST];
bool newKeyboard[GLFW_KEY_LAST];
bool oldMouse[GLFW_MOUSE_BUTTON_LAST];
bool newMouse[GLFW_MOUSE_BUTTON_LAST];
float2 oldMousePos;
float2 newMousePos;
float2 oldMouseScroll;
float2 newMouseScroll;

bool keyPressed(int key)
{
	return (newKeyboard[key] && !oldKeyboard[key]);
}
bool keyReleased(int key)
{
	return (oldKeyboard[key] && !newKeyboard[key]);
}
bool keyDown(int key)
{
	return (newKeyboard[key]);
}
bool mousePressed(int button)
{
	return (newMouse[button] && !oldMouse[button]);
}
bool mouseReleased(int button)
{
	return (oldMouse[button] && !newMouse[button]);
}
bool mouseDown(int button)
{
	return (newMouse[button]);
}

void onGlfwMouse(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		newMouse[button] = true;
	}
	if (action == GLFW_RELEASE)
	{
		newMouse[button] = false;
	}
}
void onGlfwMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	newMousePos.x = xpos;
	newMousePos.y = float(height) - ypos;
}
void onGlfwScroll(GLFWwindow* window, double xpos, double ypos)
{
	newMouseScroll.x = xpos;
	newMouseScroll.y = ypos;
}

void onGlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		newKeyboard[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		newKeyboard[key] = false;
	}
}
void inputUpdate()
{
	memcpy(oldKeyboard, newKeyboard, sizeof(bool) * GLFW_KEY_LAST);
	memcpy(oldMouse, newMouse, sizeof(bool) * GLFW_MOUSE_BUTTON_LAST);
	oldMousePos = newMousePos;
	oldMouseScroll = newMouseScroll;

	glfwPollEvents();
}
void inputInit(GLFWwindow* window)
{
	glfwSetKeyCallback(window, onGlfwKey);
	glfwSetMouseButtonCallback(window, onGlfwMouse);
	glfwSetCursorPosCallback(window, onGlfwMouseMove);
	glfwSetScrollCallback(window, onGlfwScroll);
}

#endif