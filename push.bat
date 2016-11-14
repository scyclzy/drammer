cmd /c ndk-build
adb push %cd%\libs\arm64-v8a\rh-test /data/local/tmp/rh-test
adb shell chmod 755 /data/local/tmp/rh-test


adb push %cd%\libs\arm64-v8a\pagemap_info /data/local/tmp/pagemap_info
adb shell chmod 755 /data/local/tmp/pagemap_info
adb shell /data/local/tmp/pagemap_info