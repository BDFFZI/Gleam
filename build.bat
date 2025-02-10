@echo off
@cd /d %~dp0

echo ==========检查项目环境==========
echo=

if ["%VCPKG_ROOT%"]==[""] (
    echo 错误！未设置 VCPKG_ROOT 环境变量。
) else (
    echo ==========生成项目文件==========
    echo=

    cmake --preset=default

    echo=
)

echo=

pause