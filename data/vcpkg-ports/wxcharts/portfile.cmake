vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO wxIshiko/wxCharts
    REF a1f56c725f8907d7d0a9d149bce7d4db1e1bc84e
    SHA512 efcda9634c9947476b330b6add8d1ce38810aa2480c04504a5f8b53b641fc659b8cd77c710553093a2037593761232501ada242094865a13c792a838752b27d5
    PATCHES
        add_unit.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/${PORT}")

file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)