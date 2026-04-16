#!/bin/bash

# 1. Variables de Entorno (SDK y NDK de Android)
export ANDROID_HOME=/home/alfredo/Android/Sdk
export ANDROID_SDK_ROOT=/home/alfredo/Android/Sdk
export ANDROID_NDK_HOME=/home/alfredo/Android/Sdk/ndk/27.2.12479018
export PATH=$PATH:/home/alfredo/dev/mm1/vcpkg

# 2. Definición de Rutas
PROJECT_DIR="/home/alfredo/dev/mm1/MDM_mobile"
VCPKG_ROOT="/home/alfredo/dev/mm1/vcpkg"
BUILD_DIR="$PROJECT_DIR/build_android"

echo "------------------------------------------------"
echo "🧹 Limpiando carpetas para evitar conflictos de Protobuf..."
rm -rf "$BUILD_DIR"
rm -rf "$PROJECT_DIR/vcpkg_installed" 

echo "Preparando carpetas en $BUILD_DIR..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 3. Instalación de dependencias con vcpkg
echo "📦 Instalando dependencias GIS para Android arm64..."
$VCPKG_ROOT/vcpkg install --triplet arm64-android \
  --overlay-ports=$PROJECT_DIR/vcpkg/ports \
  --overlay-triplets=$PROJECT_DIR/vcpkg/triplets \
  --x-install-root=$BUILD_DIR/vcpkg_installed \
  --allow-unsupported

# 4. Configuración de CMake
echo "🛠️ Configurando proyecto Mergin Maps (MDM_mobile)..."
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  -DVCPKG_TARGET_TRIPLET=arm64-android \
  -DCMAKE_SYSTEM_NAME=Android \
  -DANDROID_ABI=arm64-v8a \
  -DVCPKG_MANIFEST_INSTALL=OFF \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
  -DCMAKE_PREFIX_PATH=$BUILD_DIR/vcpkg_installed/arm64-android \
  -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
  -DQT_ANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
  -DQt6_DIR=$BUILD_DIR/vcpkg_installed/arm64-android/share/qt6 \
  -DGeodiff_DIR=$BUILD_DIR/vcpkg_installed/arm64-android/share/geodiff \
  -DOpenSSL_DIR=$BUILD_DIR/vcpkg_installed/arm64-android/share/openssl \
  -DOPENSSL_CRYPTO_LIBRARY=$BUILD_DIR/vcpkg_installed/arm64-android/lib/libcrypto.so \
  -DOPENSSL_SSL_LIBRARY=$BUILD_DIR/vcpkg_installed/arm64-android/lib/libssl.so \
  -DCMAKE_BUILD_TYPE=Release

# 5. Compilación del binario
echo "🚀 Compilando C++ para arm64-v8a..."
cmake --build . --target all -j$(nproc)

echo "------------------------------------------------"
echo "🔐 Iniciando proceso de firma del APK..."

# 6. Variables de Firma
KEYSTORE_FILE="/home/alfredo/dev/mm1/MDM_mobile/mergin_firma.keystore"
KEYSTORE_ALIAS="mergin_key"
KEYSTORE_PASS="INEGI_2020" 

# Rutas de los APKs
APK_DIR="$BUILD_DIR/app/android-build/build/outputs/apk/release"
UNSIGNED_APK="$APK_DIR/android-build-release-unsigned.apk"
ALIGNED_APK="$APK_DIR/android-build-release-aligned.apk"
SIGNED_APK="$APK_DIR/MerginMaps-MDM-Firmado.apk"

# Buscar última versión de build-tools
BUILD_TOOLS_DIR=$(ls -1d $ANDROID_SDK_ROOT/build-tools/* | tail -n 1)

# --- AQUÍ ESTABA EL CORTE ---
if [ -f "$UNSIGNED_APK" ]; then
    echo "1. Alineando APK (zipalign)..."
    $BUILD_TOOLS_DIR/zipalign -f -p 4 "$UNSIGNED_APK"  "$ALIGNED_APK"
    
    echo "2. Firmando APK (apksigner)..."
    $BUILD_TOOLS_DIR/apksigner sign --ks "$KEYSTORE_FILE" --ks-key-alias "$KEYSTORE_ALIAS" --ks-pass pass:"$KEYSTORE_PASS" --out "$SIGNED_APK" "$ALIGNED_APK"

    echo "------------------------------------------------"
    echo "✅ ¡APK FIRMADO Y LISTO!"
    echo "Ruta: $SIGNED_APK"
    echo "------------------------------------------------"
else
    echo "❌ Error: No se encontró el APK sin firmar. Revisa si hubo errores en la compilación."
    echo "Se buscó en: $UNSIGNED_APK"
fi
# --- FIN DEL BLOQUE CERRADO CORRECTAMENTE ---
