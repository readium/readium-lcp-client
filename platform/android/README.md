# Android

## Properties

You can define some properties in local.properties file:

* ndk.dir: Path to the Ndk
* sdk.dir: Path to the Sdk
* readium.ndk_skipX86: If true, skip X86 compilation
* readium.ndk_skipARM: If true, skip ARM compilation
* readium.ndk_clang: If true, compile with clang
* readium.sdk_lib_dir: Path to readium sdk lib (*.so files) directory
* readium.sdk_include_dir: Path to readium include directory

## Clean

```
./gradlew clean
```

## Build

```
./gradlew build
```
