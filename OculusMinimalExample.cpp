/************************************************************************************

Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "ServerGame.h"
#include "ClientGame.h"
// used for multi-threading
#include <process.h> 

#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

#include<vector>
#include<string>
#include<ctime>
#include<cstdlib>

#include <Windows.h>
#include "Leap.h"

#include "Scene.h"

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/mesh.h>
#include<assimp/postprocess.h>



void serverLoop(void *);
void clientLoop(void);

ServerGame * server;
ClientGame * client;


///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
    GLuint status = glCheckFramebufferStatus(target);
    switch (status) {
    case GL_FRAMEBUFFER_COMPLETE:
        return true;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr << "framebuffer incomplete attachment" << std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr << "framebuffer missing attachment" << std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cerr << "framebuffer incomplete draw buffer" << std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cerr << "framebuffer incomplete read buffer" << std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cerr << "framebuffer incomplete multisample" << std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        std::cerr << "framebuffer incomplete layer targets" << std::endl;
        break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr << "framebuffer unsupported internal format or image" << std::endl;
        break;

    default:
        std::cerr << "other framebuffer error" << std::endl;
        break;
    }

    return false;
}

bool checkGlError() {
    GLenum error = glGetError();
    if (!error) {
        return false;
    } else {
        switch (error) {
        case GL_INVALID_ENUM:
            std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_INVALID_VALUE:
            std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
            break;
        case GL_INVALID_OPERATION:
            std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
            break;
        }
        return true;
    }
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
    OutputDebugStringA(msg);
    std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
    inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
        GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
        if (!window) {
            FAIL("Unable to create rendering window");
        }
        if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
            glfwSetWindowPos(window, position.x, position.y);
        }
        return window;
    }
}


// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
    uvec2 windowSize;
    ivec2 windowPosition;
    GLFWwindow * window { nullptr };
    unsigned int frame { 0 };

public:
    GlfwApp() {
        // Initialize the GLFW system for creating and positioning windows
        if (!glfwInit()) {
            FAIL("Failed to initialize GLFW");
        }
        glfwSetErrorCallback(ErrorCallback);
    }

    virtual ~GlfwApp() {
        if (nullptr != window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

    virtual int run() {

		srand(time(NULL));

        preCreate();

        window = createRenderingTarget(windowSize, windowPosition);

        if (!window) {
            std::cout << "Unable to create OpenGL window" << std::endl;
            return -1;
        }

        postCreate();

        initGl();

		while (!glfwWindowShouldClose(window)) {
            ++frame;
            glfwPollEvents();
            update();
            draw();
            finishFrame();
			//client->update();
        }

        shutdownGl();

        return 0;
    }


protected:
    virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

    virtual void draw() = 0;

    void preCreate() {
        glfwWindowHint(GLFW_DEPTH_BITS, 16);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }

    
    void postCreate() {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwMakeContextCurrent(window);

        // Initialize the OpenGL bindings
        // For some reason we have to set this experminetal flag to properly
        // init GLEW if we use a core context.
        glewExperimental = GL_TRUE;
        if (0 != glewInit()) {
            FAIL("Failed to initialize GLEW");
        }
        glGetError();

        if (GLEW_KHR_debug) {
            GLint v;
            glGetIntegerv(GL_CONTEXT_FLAGS, &v);
            if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
                // glDebugMessageCallback(glDebugCallbackHandler, this);
            }
        }
    }

    virtual void initGl() {
    }

    virtual void shutdownGl() {
    }

    virtual void finishFrame() {
        glfwSwapBuffers(window);
    }

    virtual void destroyWindow() {
        glfwSetKeyCallback(window, nullptr);
        glfwSetMouseButtonCallback(window, nullptr);
        glfwDestroyWindow(window);
    }

    virtual void onKey(int key, int scancode, int action, int mods) {
        if (GLFW_PRESS != action) {
            return;
        }

        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, 1);
            return;
        }
    }

    virtual void update() {}

    virtual void onMouseButton(int button, int action, int mods) {}

protected:
    virtual void viewport(const ivec2 & pos, const uvec2 & size) {
        glViewport(pos.x, pos.y, size.x, size.y);
    }

private:

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
        instance->onKey(key, scancode, action, mods);
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
        instance->onMouseButton(button, action, mods);
    }

    static void ErrorCallback(int error, const char* description) {
        FAIL(description);
    }
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftManagerApp {
protected:
    ovrSession _session;
    ovrHmdDesc _hmdDesc;
    ovrGraphicsLuid _luid;

public:
    RiftManagerApp() {
        if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
            FAIL("Unable to create HMD session");
        }

        _hmdDesc = ovr_GetHmdDesc(_session);
    }

    ~RiftManagerApp() {
        ovr_Destroy(_session);
        _session = nullptr;
    }
};

class RiftApp : public GlfwApp, public RiftManagerApp {

private:
    GLuint _fbo { 0 };
    GLuint _depthBuffer { 0 };
    ovrTextureSwapChain _eyeTexture;

    GLuint _mirrorFbo { 0 };
    ovrMirrorTexture _mirrorTexture;

    ovrEyeRenderDesc _eyeRenderDescs[2];

    mat4 _eyeProjections[2];

    ovrLayerEyeFov _sceneLayer;
    ovrViewScaleDesc _viewScaleDesc;

    uvec2 _renderTargetSize;
    uvec2 _mirrorSize;

public:

    RiftApp() {
        using namespace ovr;
        _viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

        memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
        _sceneLayer.Header.Type = ovrLayerType_EyeFov;
        _sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

        ovr::for_each_eye([&](ovrEyeType eye) {
            ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
            ovrMatrix4f ovrPerspectiveProjection =
                ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
            _eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
            _viewScaleDesc.HmdToEyeOffset[eye] = erd.HmdToEyeOffset;

            ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
            auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
            _sceneLayer.Viewport[eye].Size = eyeSize;
            _sceneLayer.Viewport[eye].Pos = { static_cast<int>(_renderTargetSize.x), 0 };

            _renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
            _renderTargetSize.x += eyeSize.w;
        });
        // Make the on screen window 1/4 the resolution of the render target
        _mirrorSize = _renderTargetSize;
        _mirrorSize /= 4;
    }

protected:
    GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
        return glfw::createWindow(_mirrorSize);
    }

    void initGl() override {
        GlfwApp::initGl();

        // Disable the v-sync for buffer swap
        glfwSwapInterval(0);

        ovrTextureSwapChainDesc desc = {};
        desc.Type = ovrTexture_2D;
        desc.ArraySize = 1;
        desc.Width = _renderTargetSize.x;
        desc.Height = _renderTargetSize.y;
        desc.MipLevels = 1;
        desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.SampleCount = 1;
        desc.StaticImage = ovrFalse;
        ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
        _sceneLayer.ColorTexture[0] = _eyeTexture;
        if (!OVR_SUCCESS(result)) {
            FAIL("Failed to create swap textures");
        }

        int length = 0;
        result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
        if (!OVR_SUCCESS(result) || !length) {
            FAIL("Unable to count swap chain textures");
        }
        for (int i = 0; i < length; ++i) {
            GLuint chainTexId;
            ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
            glBindTexture(GL_TEXTURE_2D, chainTexId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        // Set up the framebuffer object
        glGenFramebuffers(1, &_fbo);
        glGenRenderbuffers(1, &_depthBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        ovrMirrorTextureDesc mirrorDesc;
        memset(&mirrorDesc, 0, sizeof(mirrorDesc));
        mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        mirrorDesc.Width = _mirrorSize.x;
        mirrorDesc.Height = _mirrorSize.y;
        if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
            FAIL("Could not create mirror texture");
        }
        glGenFramebuffers(1, &_mirrorFbo);
    }

    void onKey(int key, int scancode, int action, int mods) override {
        if (GLFW_PRESS == action) switch (key) {
        case GLFW_KEY_R:
            ovr_RecenterTrackingOrigin(_session);
            return;
        }

        GlfwApp::onKey(key, scancode, action, mods);
    }

	virtual void update() {
		ovrTrackingState trackState = ovr_GetTrackingState(_session, ovr_GetPredictedDisplayTime(_session, 0), ovrTrue);
		handPos[0] = ovr::toGlm(trackState.HandPoses[0].ThePose.Position);
		handPos[1] = ovr::toGlm(trackState.HandPoses[1].ThePose.Position);
		handMat[0] = ovr::toGlm(trackState.HandPoses[0].ThePose);
		handMat[1] = ovr::toGlm(trackState.HandPoses[1].ThePose);
		ovrInputState inputState;
		ovr_GetInputState(_session, ovrControllerType_Touch, &inputState);
		indexTrigger[0] = inputState.IndexTrigger[0];
		indexTrigger[1] = inputState.IndexTrigger[1];
		buttonA = inputState.Buttons & ovrButton_A;
		buttonB = inputState.Buttons & ovrButton_B;
	}

    void draw() final override {
        ovrPosef eyePoses[2];
        ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyeOffset, eyePoses, &_sceneLayer.SensorSampleTime);

        int curIndex;
        ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
        GLuint curTexId;
        ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ovr::for_each_eye([&](ovrEyeType eye) {
            const auto& vp = _sceneLayer.Viewport[eye];
            glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
            _sceneLayer.RenderPose[eye] = eyePoses[eye];
            renderScene(_eyeProjections[eye], ovr::toGlm(eyePoses[eye]));
        });
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        ovr_CommitTextureSwapChain(_session, _eyeTexture);
        ovrLayerHeader* headerList = &_sceneLayer.Header;
        ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

        GLuint mirrorTextureId;
        ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
        glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    virtual void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) = 0;
};


//////////////////////////////////////////////////////////////////////
//
// The remainder of this code is specific to the scene we want to 
// render.  I use oglplus to render an array of cubes, but your 
// application would perform whatever rendering you want
//


//////////////////////////////////////////////////////////////////////
//
// OGLplus is a set of wrapper classes for giving OpenGL a more object
// oriented interface
//
#define OGLPLUS_USE_GLCOREARB_H 0
#define OGLPLUS_USE_GLEW 1
#define OGLPLUS_USE_BOOST_CONFIG 1
#define OGLPLUS_NO_SITE_CONFIG 1
#define OGLPLUS_LOW_PROFILE 1

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/basic.hpp>
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#include <oglplus/shapes/cube.hpp>
#include <oglplus/shapes/wrapper.hpp>
#pragma warning( default : 4068 4244 4267 4065)



//
//class Bullet {
//
//public:
//	Model * m;
//	glm::vec3 position;
//	glm::vec3 velocity;
//	glm::mat4 model;
//
//	Bullet(Model * _m, glm::vec3 p, glm::vec3 v) {
//		m = _m;
//		position = p;
//		velocity = v;
//		model = glm::mat4(1.0f);
//		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
//	}
//
//	void update() {
//		position += velocity * 0.005f;
//		model = glm::translate(model, (glm::mat4(1.0f), position));
//	}
//
//	void render(Shader shader, const mat4 & projection, const mat4 & modelview) {
//		update();
//		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
//		m->Draw(shader);
//	}
//
//};
//
//
//class BulletManager {
//
//public:
//
//	Model m;
//	std::vector<Bullet *> bullets;
//
//	BulletManager() : m("./o2/o2.obj") {
//	}
//
//	void createBullet() {
//		glm::mat4 model = ovr::toGlm(RiftApp::handPose[0]);
//		//bullets.push_back(new Bullet(&m, ));
//
//	}
//
//	void renderBullets(Shader shader, const mat4 & projection, const mat4 & modelview) {
//		for (Bullet * b : bullets) {
//			b->render(shader, projection, modelview);
//		}
//	}
//
//};


// An example application that renders a simple cube
class ExampleApp : public RiftApp {
    std::shared_ptr<Scene> cubeScene;

public:
    ExampleApp() { }

protected:
    void initGl() override {
        RiftApp::initGl();
        glClearColor(0.0f, 0.0f, 0.3f, 0.0f);
		//glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
        glEnable(GL_DEPTH_TEST);
		// Enable translucency.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Enable culling.
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

        ovr_RecenterTrackingOrigin(_session);
        cubeScene = std::shared_ptr<Scene>(new Scene());
    }

    void shutdownGl() override {
        cubeScene.reset();
    }

    void renderScene(const glm::mat4 & projection, const glm::mat4 & headPose) override {
        cubeScene->render(projection, glm::inverse(headPose));
    }
};

void serverLoop(void * arg)
{
	while (true)
	{
		server->update();
	}
}

void clientLoop()
{
	while (true)
	{
		//do game stuff
		client->update();
	}
}

// Execute our example class
int main(int argc, char** argv) {
    int result = -1;

	// initialize the server
	server = new ServerGame();

	// create thread with arbitrary argument for the run function
	_beginthread(serverLoop, 0, (void*)12);

	// initialize the client
	client = new ClientGame();

	//clientLoop();

    try {
        if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
            FAIL("Failed to initialize the Oculus SDK");
        }
        result = ExampleApp().run();
    } catch (std::exception & error) {
        OutputDebugStringA(error.what());
        std::cerr << error.what() << std::endl;
    }
    ovr_Shutdown();
    return result;
}


