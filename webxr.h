#ifndef WEBXR_H_
#define WEBXR_H_

/** @file
 * @brief Minimal WebXR Device API wrapper
 */

#ifdef __cplusplus
extern "C"
#endif
{

/** Errors enum */
enum WebXRError {
    WEBXR_ERR_API_UNSUPPORTED = -2, /**< WebXR Device API not supported in this browser */
    WEBXR_ERR_GL_INCAPABLE = -3, /**< GL context cannot render WebXR */
    WEBXR_ERR_SESSION_UNSUPPORTED = -4, /**< given session mode not supported */
};

/** WebXR handedness */
enum WebXRHandedness {
    WEBXR_HANDEDNESS_NONE = -1,
    WEBXR_HANDEDNESS_LEFT = 0,
    WEBXR_HANDEDNESS_RIGHT = 1,
};

/** WebXR target ray mode */
enum WebXRTargetRayMode {
    WEBXR_TARGET_RAY_MODE_GAZE = 0,
    WEBXR_TARGET_RAY_MODE_TRACKED_POINTER = 1,
    WEBXR_TARGET_RAY_MODE_SCREEN = 2,
};

/** WebXR 'XRSessionMode' enum*/
enum WebXRSessionMode {
    WEBXR_SESSION_MODE_INLINE = 0, /** "inline" */
    WEBXR_SESSION_MODE_IMMERSIVE_VR = 1, /** "immersive-vr" */
    WEBXR_SESSION_MODE_IMMERSIVE_AR = 2, /** "immersive-ar" */
};

/** WebXR 'XRSessionMode' enum*/
enum WebXRSessionFeatures {
    WEBXR_SESSION_FEATURE_LOCAL = 0, /** "local" */
    WEBXR_SESSION_FEATURE_LOCAL_FLOOR = 1, /** "local-floor" */
    WEBXR_SESSION_FEATURE_BOUNDED_FLOOR = 2, /** "bounded-floor" */
    WEBXR_SESSION_FEATURE_UNBOUNDED = 3, /** "unbounded" */
    WEBXR_SESSION_FEATURE_HIT_TEST = 4, /** "hit-test" */
};

/** WebXR 'XRSessionMode' enum*/
enum WebXRInputPoseMode {
    WEBXR_INPUT_POSE_GRIP = 0, /** gripSpace */
    WEBXR_INPUT_POSE_TARGET_RAY = 1, /** targetRaySpace */
};

/** WebXR rigid transform */
typedef struct WebXRRigidTransform {
    float matrix[16];
    float position[3];
    float orientation[4];
} WebXRRigidTransform;

/** WebXR view */
typedef struct WebXRView {
    /* view pose */
    WebXRRigidTransform viewPose;
    /* projection matrix */
    float projectionMatrix[16];
    /* x, y, width, height of the eye viewport on target texture */
    int viewport[4];
} WebXRView;

typedef struct WebXRInputSource {
    int id;
    WebXRHandedness handedness;
    WebXRTargetRayMode targetRayMode;
} WebXRInputSource;

/**
Callback for errors

@param userData User pointer passed to init_webxr()
@param error Error code
*/
typedef void (*webxr_error_callback_func)(void* userData, int error);

/**
Callback for frame rendering

@param userData User pointer passed to init_webxr()
@param time Current frame time
@param modelMatrix Transformation of the XR Device to tracking origin
@param views Array of `viewCount` @ref WebXRView "webxr views"
@param viewCount Size of `views`
*/
typedef void (*webxr_frame_callback_func)(void* userData, int framebuffer_id, int time, WebXRRigidTransform* headPose, WebXRView views[2], int viewCount);

/**
Callback for VR session start

@param userData User pointer passed to set_session_start_callback
@param mode The session mode
*/
typedef void (*webxr_session_callback_func)(void* userData, int mode);

/**
Callback for @ref webxr_is_session_supported

@param mode The session mode that was requested
@param supported Whether given mode is supported by this device
*/
typedef void (*webxr_session_supported_callback_func)(int mode, int supported);


/**
Init WebXR rendering

@param frameCallback Callback called every frame
@param sessionStartCallback Callback called when session is started
@param sessionEndCallback Callback called when session ended
@param errorCallback Callback called every frame
@param userData User data passed to the callbacks
*/
extern void webxr_init(
        webxr_frame_callback_func frameCallback,
        webxr_session_callback_func sessionStartCallback,
        webxr_session_callback_func sessionEndCallback,
        webxr_error_callback_func errorCallback,
        void* userData);

extern void webxr_set_session_blur_callback(
        webxr_session_callback_func sessionBlurCallback, void* userData);
extern void webxr_set_session_focus_callback(
        webxr_session_callback_func sessionFocusCallback, void* userData);


/*
Test if session mode is supported

@param mode Session mode to test
@param supportedCallback Callback which will be called once the
        result has become available
*/
extern void webxr_is_session_supported(WebXRSessionMode mode,
        webxr_session_supported_callback_func supportedCallback);
/*
Request session presentation start

@param mode Session mode from @ref WebXRSessionMode.
@param requiredFeatures Required session features from @ref WebXRSessionFeatures
@param optionalFeatures Required session features from @ref WebXRSessionFeatures

Needs to be called from a [user activation event](https://html.spec.whatwg.org/multipage/interaction.html#triggered-by-user-activation).
*/
extern void webxr_request_session(WebXRSessionMode mode,
    WebXRSessionFeatures requiredFeatures,
    WebXRSessionFeatures optionalFeatures);

/*
Request that the webxr presentation exits VR mode
*/
extern void webxr_request_exit();

/**
Set projection matrix parameters for the webxr session

@param near Distance of near clipping plane
@param far Distance of far clipping plane
*/
extern void webxr_set_projection_params(float near, float far);

/**

WebXR Input

*/

/**
Callback for primary input action.

@param userData User pointer passed to @ref webxr_set_select_callback, @ref webxr_set_select_end_callback or @ref webxr_set_select_start_callback.
*/
typedef void (*webxr_input_callback_func)(WebXRInputSource* inputSource, void* userData);


/**
Set callbacks for primary input action.
*/
extern void webxr_set_select_callback(
        webxr_input_callback_func callback, void* userData);
extern void webxr_set_select_start_callback(
        webxr_input_callback_func callback, void* userData);
extern void webxr_set_select_end_callback(
        webxr_input_callback_func callback, void* userData);

/**
Get input sources.

@param outArray @ref WebXRInputSource array to fill.
@param max Size of outArray (in elements).
@param outCount Will receive the number of input sources valid in outArray.
*/
extern void webxr_get_input_sources(
        WebXRInputSource* outArray, int max, int* outCount);

/**
Get input pose. Can only be called during the frame callback.

@param source The source to get the pose for.
@param outPose Where to store the pose.
@returns `false` if updating the pose failed, `true` otherwise.
*/
extern int webxr_get_input_pose(WebXRInputSource* source, WebXRRigidTransform* outPose, WebXRInputPoseMode mode=WEBXR_INPUT_POSE_GRIP);

}

#endif
