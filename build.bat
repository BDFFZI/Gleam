@echo off
@cd /d %~dp0

echo ==========环境检测==========
echo=

if ["%VCPKG_ROOT%"]==[""] (
    echo 错误！未设置VCPKG_ROOT环境变量
) else (
    echo ==========项目构建==========
    echo=

    cmake --preset=default

    echo=
)

echo=

pause