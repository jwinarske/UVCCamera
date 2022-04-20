#pragma once

#include "libUVCCamera.h"
#include <pthread.h>
#include "objectarray.h"

#pragma interface

// for callback to Java object
typedef struct {
    jmethodID onStatus;
} Fields_istatuscallback;

class UVCStatusCallback {
private:
    uvc_device_handle_t *mDeviceHandle;
    pthread_mutex_t status_mutex;
    jobject mStatusCallbackObj;
    Fields_istatuscallback istatuscallback_fields;

    void
    notifyStatusCallback(uvc_status_class status_class, int event, int selector, uvc_status_attribute status_attribute,
                         void *data, size_t data_len);

    static void
    uvc_status_callback(uvc_status_class status_class, int event, int selector, uvc_status_attribute status_attribute,
                        void *data, size_t data_len, void *user_ptr);

public:
    UVCStatusCallback(uvc_device_handle_t *devh);

    ~UVCStatusCallback();

    int setCallback(jobject status_callback_obj);
};
