vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO corrosion-rs/corrosion
        REF v${VERSION}
        SHA512 2510d4d0484fc12a6b429244b283515fda650b52ea74fbfdcc141298b452b20e2bef800b8f8a573a2bf509f4147ecb2d68e795cbd86cc8edd092f57ccff8b86b
        HEAD_REF master
        PATCHES
            minimal.patch
)

# Configure and install only CMake files, not building any binaries
vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS
            -DINCLUDE_CORROSION=ON
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/Corrosion)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib")

# Handle copyright
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)