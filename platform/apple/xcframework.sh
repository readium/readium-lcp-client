#######
#xcode-select --install
#sudo xcode-select --switch /Library/Developer/CommandLineTools
#sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
#######

rm -rf "./archives/LCP Client (iOS) DEVICE"
rm -rf "./archives/LCP Client (iOS) SIMULATOR"

xcodebuild archive \
-project "LCP Client.xcodeproj" \
-scheme "LCP Client (iOS)" \
-destination "generic/platform=iOS" \
-archivePath "./archives/LCP Client (iOS) DEVICE" \
SKIP_INSTALL=NO \
BUILD_LIBRARY_FOR_DISTRIBUTION=YES

xcodebuild archive \
-project "LCP Client.xcodeproj" \
-scheme "LCP Client (iOS)" \
-destination "generic/platform=iOS Simulator" \
-archivePath "./archives/LCP Client (iOS) SIMULATOR" \
SKIP_INSTALL=NO \
BUILD_LIBRARY_FOR_DISTRIBUTION=YES

rm -rf "./archives/LCP_iOS.xcframework"

mkdir "./archives/headers"
ls -als "../../src/lcp-content-filter/public" | grep "\\.h[p]*"
ls -als "../../src/lcp-client-lib/public" | grep "\\.h[p]*"
ls -als "./src" | grep "\\.h[p]*"
cp ../../src/lcp-content-filter/public/*.h "./archives/headers/"
cp ../../src/lcp-client-lib/public/*.h "./archives/headers/"
cp ./src/*.h "./archives/headers/"
rm "./archives/headers/lcp.h"
ls -als "./archives/headers"

xcodebuild -create-xcframework  \
-library "./archives/LCP Client (iOS) DEVICE.xcarchive/Products/usr/local/lib/libLCP-client-iOS.a" \
-headers "./archives/headers" \
-library "./archives/LCP Client (iOS) SIMULATOR.xcarchive/Products/usr/local/lib/libLCP-client-iOS.a" \
-headers "./archives/headers" \
-output "./archives/LCP_iOS.xcframework"

ls -alsR "./archives/LCP_iOS.xcframework"
