@echo off
@cd /d %~dp0

echo ==========�����Ŀ����==========
echo=

if ["%VCPKG_ROOT%"]==[""] (
    echo ����δ���� VCPKG_ROOT ����������
) else (
    echo ==========������Ŀ�ļ�==========
    echo=

    cmake --preset=default

    echo=
)

echo=

pause