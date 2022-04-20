/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 *
 * File name: serenegiant_usb_UVCCamera.cpp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb, jin/libuvc, jni/rapidjson folder may have a different license, see the respective files.
*/

#if 1    // デバッグ情報を出さない時
#ifndef LOG_NDEBUG
#define    LOG_NDEBUG        // LOGV/LOGD/MARKを出力しない時
#endif
#undef USE_LOGALL            // 指定したLOGxだけを出力
#else
#define USE_LOGALL
#undef LOG_NDEBUG
#undef NDEBUG
#endif

#include "libUVCCamera.h"
#include "UVCCamera.h"

/**
 * set the value into the long field
 * @param env: this param should not be null
 * @param bullet_obj: this param should not be null
 * @param field_name
 * @params val
 */
static jlong setField_long(const char *field_name, jlong val) {
#if LOCAL_DEBUG
    LOGV("setField_long:");
#endif

    return val;
}

/**
 * set the value into int field
 * @param env: this param should not be null
 * @param java_obj: this param should not be null
 * @param field_name
 * @params val
 */
int setField_int(const char *field_name, int val) {
    LOGV("setField_int:");

    return val;
}

static ID_TYPE nativeCreate(jobject thiz) {

    ENTER();
    UVCCamera *camera = new UVCCamera();
    setField_long("mNativePtr", reinterpret_cast<ID_TYPE>(camera));
    RETURN(reinterpret_cast<ID_TYPE>(camera), ID_TYPE);
}

// native側のカメラオブジェクトを破棄
static void nativeDestroy(
        ID_TYPE id_camera) {

    ENTER();
    setField_long("mNativePtr", 0);
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        SAFE_DELETE(camera);
    }
    EXIT();
}

//======================================================================
// カメラへ接続
static int nativeConnect(
        ID_TYPE id_camera,
        int vid, int pid, int fd,
        int busNum, int devAddr, jstring usbfs_str) {

    ENTER();
    int result = JNI_ERR;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    const char *c_usbfs = env->GetStringUTFChars(usbfs_str, JNI_FALSE);
    if (LIKELY(camera && (fd > 0))) {
//		libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);
        result = camera->connect(vid, pid, fd, busNum, devAddr, c_usbfs);
    }
    env->ReleaseStringUTFChars(usbfs_str, c_usbfs);
    RETURN(result, int);
}

// カメラとの接続を解除
static int nativeRelease(
        ID_TYPE id_camera) {

    ENTER();
    int result = JNI_ERR;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->release();
    }
    RETURN(result, int);
}

//======================================================================
static int nativeSetStatusCallback(
        ID_TYPE id_camera, jobject jIStatusCallback) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject status_callback_obj = env->NewGlobalRef(jIStatusCallback);
        result = camera->setStatusCallback(status_callback_obj);
    }
    RETURN(result, int);
}

static int nativeSetButtonCallback(
        ID_TYPE id_camera, jobject jIButtonCallback) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject button_callback_obj = env->NewGlobalRef(jIButtonCallback);
        result = camera->setButtonCallback(button_callback_obj);
    }
    RETURN(result, int);
}

static jobject nativeGetSupportedSize(
        ID_TYPE id_camera) {

    ENTER();
    jstring result = NULL;
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        char *c_str = camera->getSupportedSize();
        if (LIKELY(c_str)) {
            result = env->NewStringUTF(c_str);
            free(c_str);
        }
    }
    RETURN(result, jobject);
}

//======================================================================
// プレビュー画面の大きさをセット
static int nativeSetPreviewSize(
        ID_TYPE id_camera, int width, int height, int min_fps, int max_fps, int mode, float bandwidth) {

    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->setPreviewSize(width, height, min_fps, max_fps, mode, bandwidth);
    }
    RETURN(JNI_ERR, int);
}

static int nativeStartPreview(
        ID_TYPE id_camera) {

    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        return camera->startPreview();
    }
    RETURN(JNI_ERR, int);
}

// プレビューを停止
static int nativeStopPreview(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->stopPreview();
    }
    RETURN(result, int);
}

static int nativeSetPreviewDisplay(
        ID_TYPE id_camera, jobject jSurface) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        ANativeWindow *preview_window = jSurface ? ANativeWindow_fromSurface(env, jSurface) : NULL;
        result = camera->setPreviewDisplay(preview_window);
    }
    RETURN(result, int);
}

static int nativeSetFrameCallback(
        ID_TYPE id_camera, jobject jIFrameCallback, int pixel_format) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        jobject frame_callback_obj = env->NewGlobalRef(jIFrameCallback);
        result = camera->setFrameCallback(frame_callback_obj, pixel_format);
    }
    RETURN(result, int);
}

static int nativeSetCaptureDisplay(
        ID_TYPE id_camera, jobject jSurface) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        ANativeWindow *capture_window = jSurface ? ANativeWindow_fromSurface(env, jSurface) : NULL;
        result = camera->setCaptureDisplay(capture_window);
    }
    RETURN(result, int);
}

//======================================================================
// カメラコントロールでサポートしている機能を取得する
static jlong nativeGetCtrlSupports(
        ID_TYPE id_camera) {

    jlong result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        uint64_t supports;
        int r = camera->getCtrlSupports(&supports);
        if (!r)
            result = supports;
    }
    RETURN(result, jlong);
}

// プロセッシングユニットでサポートしている機能を取得する
static jlong nativeGetProcSupports(
        ID_TYPE id_camera) {

    jlong result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        uint64_t supports;
        int r = camera->getProcSupports(&supports);
        if (!r)
            result = supports;
    }
    RETURN(result, jlong);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateScanningModeLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateScanningModeLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mScanningModeMin", min);
            setField_int("mScanningModeMax", max);
            setField_int("mScanningModeDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetScanningMode(
        ID_TYPE id_camera, int scanningMode) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setScanningMode(scanningMode);
    }
    RETURN(result, int);
}

static int nativeGetScanningMode(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getScanningMode();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateExposureModeLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureModeLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mExposureModeMin", min);
            setField_int("mExposureModeMax", max);
            setField_int("mExposureModeDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetExposureMode(
        ID_TYPE id_camera, int exposureMode) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposureMode(exposureMode);
    }
    RETURN(result, int);
}

static int nativeGetExposureMode(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposureMode();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateExposurePriorityLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposurePriorityLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mExposurePriorityMin", min);
            setField_int("mExposurePriorityMax", max);
            setField_int("mExposurePriorityDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetExposurePriority(
        ID_TYPE id_camera, int priority) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposurePriority(priority);
    }
    RETURN(result, int);
}

static int nativeGetExposurePriority(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposurePriority();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateExposureLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mExposureMin", min);
            setField_int("mExposureMax", max);
            setField_int("mExposureDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetExposure(
        ID_TYPE id_camera, int exposure) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposure(exposure);
    }
    RETURN(result, int);
}

static int nativeGetExposure(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposure();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateExposureRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateExposureRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mExposureRelMin", min);
            setField_int("mExposureRelMax", max);
            setField_int("mExposureRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetExposureRel(
        ID_TYPE id_camera, int exposure_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setExposureRel(exposure_rel);
    }
    RETURN(result, int);
}

static int nativeGetExposureRel(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getExposureRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateAutoFocusLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoFocusLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mAutoFocusMin", min);
            setField_int("mAutoFocusMax", max);
            setField_int("mAutoFocusDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAutoFocus(
        ID_TYPE id_camera, bool autofocus) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoFocus(autofocus);
    }
    RETURN(result, int);
}

static int nativeGetAutoFocus(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoFocus();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateAutoWhiteBlanceLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoWhiteBlanceLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mAutoWhiteBlanceMin", min);
            setField_int("mAutoWhiteBlanceMax", max);
            setField_int("mAutoWhiteBlanceDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAutoWhiteBlance(
        ID_TYPE id_camera, bool autofocus) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoWhiteBlance(autofocus);
    }
    RETURN(result, int);
}

static int nativeGetAutoWhiteBlance(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoWhiteBlance();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateAutoWhiteBlanceCompoLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoWhiteBlanceCompoLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mAutoWhiteBlanceCompoMin", min);
            setField_int("mAutoWhiteBlanceCompoMax", max);
            setField_int("mAutoWhiteBlanceCompoDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAutoWhiteBlanceCompo(
        ID_TYPE id_camera, bool autofocus_compo) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoWhiteBlanceCompo(autofocus_compo);
    }
    RETURN(result, int);
}

static int nativeGetAutoWhiteBlanceCompo(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoWhiteBlanceCompo();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateBrightnessLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateBrightnessLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mBrightnessMin", min);
            setField_int("mBrightnessMax", max);
            setField_int("mBrightnessDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetBrightness(
        ID_TYPE id_camera, int brightness) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setBrightness(brightness);
    }
    RETURN(result, int);
}

static int nativeGetBrightness(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getBrightness();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateFocusLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateFocusLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mFocusMin", min);
            setField_int("mFocusMax", max);
            setField_int("mFocusDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetFocus(
        ID_TYPE id_camera, int focus) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setFocus(focus);
    }
    RETURN(result, int);
}

static int nativeGetFocus(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFocus();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateFocusRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateFocusRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mFocusRelMin", min);
            setField_int("mFocusRelMax", max);
            setField_int("mFocusRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetFocusRel(
        ID_TYPE id_camera, int focus_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setFocusRel(focus_rel);
    }
    RETURN(result, int);
}

static int nativeGetFocusRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getFocusRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateIrisLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateIrisLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mIrisMin", min);
            setField_int("mIrisMax", max);
            setField_int("mIrisDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetIris(
        ID_TYPE id_camera, int iris) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setIris(iris);
    }
    RETURN(result, int);
}

static int nativeGetIris(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getIris();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateIrisRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateIrisRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mIrisRelMin", min);
            setField_int("mIrisRelMax", max);
            setField_int("mIrisRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetIrisRel(
        ID_TYPE id_camera, int iris_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setIrisRel(iris_rel);
    }
    RETURN(result, int);
}

static int nativeGetIrisRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getIrisRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdatePanLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePanLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mPanMin", min);
            setField_int("mPanMax", max);
            setField_int("mPanDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetPan(
        ID_TYPE id_camera, int pan) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPan(pan);
    }
    RETURN(result, int);
}

static int nativeGetPan(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPan();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateTiltLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateTiltLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mTiltMin", min);
            setField_int("mTiltMax", max);
            setField_int("mTiltDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetTilt(
        ID_TYPE id_camera, int tilt) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setTilt(tilt);
    }
    RETURN(result, int);
}

static int nativeGetTilt(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getTilt();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateRollLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateRollLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mRollMin", min);
            setField_int("mRollMax", max);
            setField_int("mRollDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetRoll(
        ID_TYPE id_camera, int roll) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setRoll(roll);
    }
    RETURN(result, int);
}

static int nativeGetRoll(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getRoll();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdatePanRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePanRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mPanRelMin", min);
            setField_int("mPanRelMax", max);
            setField_int("mPanRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetPanRel(
        ID_TYPE id_camera, int pan_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPanRel(pan_rel);
    }
    RETURN(result, int);
}

static int nativeGetPanRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPanRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateTiltRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateTiltRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mTiltRelMin", min);
            setField_int("mTiltRelMax", max);
            setField_int("mTiltRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetTiltRel(
        ID_TYPE id_camera, int tilt_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setTiltRel(tilt_rel);
    }
    RETURN(result, int);
}

static int nativeGetTiltRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getTiltRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateRollRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateRollRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mRollRelMin", min);
            setField_int("mRollRelMax", max);
            setField_int("mRollRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetRollRel(
        ID_TYPE id_camera, int roll_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setRollRel(roll_rel);
    }
    RETURN(result, int);
}

static int nativeGetRollRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getRollRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateContrastLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateContrastLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int("mContrastMin", min);
            setField_int(env, thiz, "mContrastMax", max);
            setField_int(env, thiz, "mContrastDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetContrast(
        ID_TYPE id_camera, int contrast) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setContrast(contrast);
    }
    RETURN(result, int);
}

static int nativeGetContrast(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getContrast();
    }
    RETURN(result, int);
}

//======================================================================
// Java method correspond to this function should not be a static method
static int nativeUpdateAutoContrastLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoContrastLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mAutoContrastMin", min);
            setField_int(env, thiz, "mAutoContrastMax", max);
            setField_int(env, thiz, "mAutoContrastDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAutoContrast(
        ID_TYPE id_camera, bool autocontrast) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoContrast(autocontrast);
    }
    RETURN(result, int);
}

static int nativeGetAutoContrast(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoContrast();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateSharpnessLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateSharpnessLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mSharpnessMin", min);
            setField_int(env, thiz, "mSharpnessMax", max);
            setField_int(env, thiz, "mSharpnessDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetSharpness(
        ID_TYPE id_camera, int sharpness) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setSharpness(sharpness);
    }
    RETURN(result, int);
}

static int nativeGetSharpness(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getSharpness();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateGainLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateGainLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mGainMin", min);
            setField_int(env, thiz, "mGainMax", max);
            setField_int(env, thiz, "mGainDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetGain(
        ID_TYPE id_camera, int gain) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setGain(gain);
    }
    RETURN(result, int);
}

static int nativeGetGain(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getGain();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateGammaLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateGammaLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mGammaMin", min);
            setField_int(env, thiz, "mGammaMax", max);
            setField_int(env, thiz, "mGammaDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetGamma(
        ID_TYPE id_camera, int gamma) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setGamma(gamma);
    }
    RETURN(result, int);
}

static int nativeGetGamma(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getGamma();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateWhiteBlanceLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateWhiteBlanceLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mWhiteBlanceMin", min);
            setField_int(env, thiz, "mWhiteBlanceMax", max);
            setField_int(env, thiz, "mWhiteBlanceDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetWhiteBlance(
        ID_TYPE id_camera, int whiteBlance) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setWhiteBlance(whiteBlance);
    }
    RETURN(result, int);
}

static int nativeGetWhiteBlance(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getWhiteBlance();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateWhiteBlanceCompoLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateWhiteBlanceCompoLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mWhiteBlanceCompoMin", min);
            setField_int(env, thiz, "mWhiteBlanceCompoMax", max);
            setField_int(env, thiz, "mWhiteBlanceCompoDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetWhiteBlanceCompo(
        ID_TYPE id_camera, int whiteBlance_compo) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setWhiteBlanceCompo(whiteBlance_compo);
    }
    RETURN(result, int);
}

static int nativeGetWhiteBlanceCompo(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getWhiteBlanceCompo();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateBacklightCompLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateBacklightCompLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mBacklightCompMin", min);
            setField_int(env, thiz, "mBacklightCompMax", max);
            setField_int(env, thiz, "mBacklightCompDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetBacklightComp(
        ID_TYPE id_camera, int backlight_comp) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setBacklightComp(backlight_comp);
    }
    RETURN(result, int);
}

static int nativeGetBacklightComp(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getBacklightComp();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateSaturationLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateSaturationLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mSaturationMin", min);
            setField_int(env, thiz, "mSaturationMax", max);
            setField_int(env, thiz, "mSaturationDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetSaturation(
        ID_TYPE id_camera, int saturation) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setSaturation(saturation);
    }
    RETURN(result, int);
}

static int nativeGetSaturation(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getSaturation();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateHueLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateHueLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mHueMin", min);
            setField_int(env, thiz, "mHueMax", max);
            setField_int(env, thiz, "mHueDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetHue(
        ID_TYPE id_camera, int hue) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setHue(hue);
    }
    RETURN(result, int);
}

static int nativeGetHue(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getHue();
    }
    RETURN(result, int);
}

//======================================================================
// Java method correspond to this function should not be a static method
static int nativeUpdateAutoHueLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAutoHueLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mAutoHueMin", min);
            setField_int(env, thiz, "mAutoHueMax", max);
            setField_int(env, thiz, "mAutoHueDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAutoHue(
        ID_TYPE id_camera, bool autohue) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAutoHue(autohue);
    }
    RETURN(result, int);
}

static int nativeGetAutoHue(
        ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAutoHue();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdatePowerlineFrequencyLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePowerlineFrequencyLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mPowerlineFrequencyMin", min);
            setField_int(env, thiz, "mPowerlineFrequencyMax", max);
            setField_int(env, thiz, "mPowerlineFrequencyDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetPowerlineFrequency(
        ID_TYPE id_camera, int frequency) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPowerlineFrequency(frequency);
    }
    RETURN(result, int);
}

static int nativeGetPowerlineFrequency(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPowerlineFrequency();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateZoomLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateZoomLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mZoomMin", min);
            setField_int(env, thiz, "mZoomMax", max);
            setField_int(env, thiz, "mZoomDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetZoom(
        ID_TYPE id_camera, int zoom) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setZoom(zoom);
    }
    RETURN(result, int);
}

static int nativeGetZoom(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getZoom();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateZoomRelLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateZoomRelLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mZoomRelMin", min);
            setField_int(env, thiz, "mZoomRelMax", max);
            setField_int(env, thiz, "mZoomRelDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetZoomRel(
        ID_TYPE id_camera, int zoom_rel) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setZoomRel(zoom_rel);
    }
    RETURN(result, int);
}

static int nativeGetZoomRel(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getZoomRel();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateDigitalMultiplierLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateDigitalMultiplierLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mDigitalMultiplierMin", min);
            setField_int(env, thiz, "mDigitalMultiplierMax", max);
            setField_int(env, thiz, "mDigitalMultiplierDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetDigitalMultiplier(
        ID_TYPE id_camera, int multiplier) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setDigitalMultiplier(multiplier);
    }
    RETURN(result, int);
}

static int nativeGetDigitalMultiplier(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getDigitalMultiplier();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateDigitalMultiplierLimitLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateDigitalMultiplierLimitLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mDigitalMultiplierLimitMin", min);
            setField_int(env, thiz, "mDigitalMultiplierLimitMax", max);
            setField_int(env, thiz, "mDigitalMultiplierLimitDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetDigitalMultiplierLimit(
        ID_TYPE id_camera, int multiplier_limit) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setDigitalMultiplierLimit(multiplier_limit);
    }
    RETURN(result, int);
}

static int nativeGetDigitalMultiplierLimit(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getDigitalMultiplierLimit();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateAnalogVideoStandardLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAnalogVideoStandardLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mAnalogVideoStandardMin", min);
            setField_int(env, thiz, "mAnalogVideoStandardMax", max);
            setField_int(env, thiz, "mAnalogVideoStandardDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAnalogVideoStandard(
        ID_TYPE id_camera, int standard) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAnalogVideoStandard(standard);
    }
    RETURN(result, int);
}

static int nativeGetAnalogVideoStandard(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAnalogVideoStandard();
    }
    RETURN(result, int);
}

//======================================================================
// Java mnethod correspond to this function should not be a static method
static int nativeUpdateAnalogVideoLockStateLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updateAnalogVideoLockStateLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mAnalogVideoLockStateMin", min);
            setField_int(env, thiz, "mAnalogVideoLockStateMax", max);
            setField_int(env, thiz, "mAnalogVideoLockStateDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetAnalogVideoLockState(
        ID_TYPE id_camera, int state) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setAnalogVideoLockState(state);
    }
    RETURN(result, int);
}

static int nativeGetAnalogVideoLockState(
        ID_TYPE id_camera) {

    int result = 0;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getAnalogVideoLockState();
    }
    RETURN(result, int);
}

//======================================================================
// Java method correspond to this function should not be a static method
static int nativeUpdatePrivacyLimit(
        ID_TYPE id_camera) {
    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        int min, max, def;
        result = camera->updatePrivacyLimit(min, max, def);
        if (!result) {
            // Java側へ書き込む
            setField_int(env, thiz, "mPrivacyMin", min);
            setField_int(env, thiz, "mPrivacyMax", max);
            setField_int(env, thiz, "mPrivacyDef", def);
        }
    }
    RETURN(result, int);
}

static int nativeSetPrivacy(
        ID_TYPE id_camera, bool privacy) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->setPrivacy(privacy ? 1 : 0);
    }
    RETURN(result, int);
}

static int nativeGetPrivacy(ID_TYPE id_camera) {

    int result = JNI_ERR;
    ENTER();
    UVCCamera *camera = reinterpret_cast<UVCCamera *>(id_camera);
    if (LIKELY(camera)) {
        result = camera->getPrivacy();
    }
    RETURN(result, int);
}
