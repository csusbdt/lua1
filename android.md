Install ant


Run "android list target" to see the available targets.
If you don't see any targets, run "android" to get the GUI version
of the SDK manager and then select and install a recent Android target.

In my case, I saw target named "android-19"

Generate an android project file.

Note: The character '\' designates line continuation on OS X.
      Replace this with '^' on Windows.


android create project            \
        --target android-19       \
        --name cse441             \
        --path ./android          \
        --activity MainActivity   \
        --package cse441.test

Generate a public-private key pair.

keytool -genkey                    \
        -alias dev                 \
        -keyalg RSA                \
        -keysize 2048              \
        -dname CN=localhost        \
        -keypass 123456            \
        -validity 10000            \
        -keystore cse441.keystore  \
        -storepass 123456

Build.

    cd android
    ant debug

Plug in phone and install.

    ant installd

Locate app and run.

To uninstall the app:

    ant uninstall

Now, test app built with the NDK.
See https://developer.android.com/tools/sdk/ndk/index.html


Add following to the <manifest> element of AndroidManifest.xml:

    <uses-sdk android:minSdkVersion="3" />

In the android folder:

    mkdir jni

Create jni/Application.mk with the following contents.

......

Build.

cd into android folder and run "ndk-build"



