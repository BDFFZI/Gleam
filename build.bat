@echo off
@cd /d %~dp0

echo ==========�������==========
echo=

if ["%VCPKG_ROOT%"]==[""] (
    echo ����δ����VCPKG_ROOT��������
) else (
    echo ==========��Ŀ����==========
    echo=

    cmake --preset=default

    echo=
)

echo=

pause