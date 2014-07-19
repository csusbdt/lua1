# Android Notes

# Development System Setup

Install ant.

Run "android list target" to see the available targets. If you don't see 
any targets, run "android" to get the GUI version of the SDK manager and 
then select and install a recent Android target.  In my case, I saw target 
named "android-19"

Generate an android project file as follows.

Note: The character '\' designates line continuation on OS X.
      Replace this with '^' on Windows.

android create project            \
        --target android-20       \
        --name lua1test           \
        --path ./proj             \
        --activity MainActivity   \
        --package lua1.test

Generate a public-private key pair.

keytool -genkey                     \
        -alias dev                  \
        -keyalg RSA                 \
        -keysize 2048               \
        -dname CN=localhost         \
        -keypass 123456             \
        -validity 10000             \
        -keystore lua1test.keystore \
        -storepass 123456

Build.

    cd android
    ant debug

Plug in phone and install.

    ant installd

Locate app and run.

To uninstall the app:

    ant uninstall

Install the Android NDK.
See https://developer.android.com/tools/sdk/ndk/index.html

Add following to the <manifest> element of AndroidManifest.xml:
(From SDL readme.)

    <uses-sdk android:minSdkVersion="10" />

In the proj folder:

    mkdir jni

Create jni/Android.mk with the following contents.

~~~~
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hello-jni
LOCAL_SRC_FILES := hello-jni.c

include $(BUILD_SHARED_LIBRARY)
~~~~

Create jni/Android.mk with the following contents.
(Not sure about this.)

~~~~
APP_ABI := all
~~~~

Create jni/hello-jni.c with the following contents.

~~~~
#include <string.h>
#include <jni.h>

jstring
Java_lua1_test_MainActivity_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
    return (*env)->NewStringUTF(env, "Hola!");
}
~~~~

Add the following to the manifest element of AndroidManifest.xml:

~~~~
<uses-sdk android:minSdkVersion="10" />
~~~~

Create src/lua1/test/MainActivity.java with the following:

~~~~
package lua1.test;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity
{
    static {
        System.loadLibrary("hello-jni");
    }

    public native String stringFromJNI();

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {  
        super.onCreate(savedInstanceState);
        TextView textView = new TextView(this);
        textView.setTextSize(40);
        textView.setText(stringFromJNI());
        setContentView(textView);
    }
}
~~~~

Build.

cd into android folder and run "ndk-build"

run ant debug

Plug in device.  Run ant installd and then run.




