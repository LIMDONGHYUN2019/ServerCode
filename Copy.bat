
cd..
cd..
:: �ּ��� -> ::
:: ������ ������Ʈ������ ������ ������ �ַ�������� �̵��ϱ����� ���ܰ������� �̵�  �װ��� cd..
:: .\ ���� ���� ����
:: ������ �̺�Ʈ �ݵ�� ������ ������.
:: call "$Solution" �ַ�� ���������� ���Ѵ�.

xcopy .\NetworkEngine\Include\*.h .\Engine\Include\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\Engine\Bin\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\GameServer\Bin\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\LoginServer\Bin\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\MoveSyncServer\Bin\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\MonitoringServer\Bin\ /s /d /y
xcopy .\NetworkEngine\Bin\*.lib .\MonsterAIServer\Bin\ /s /d /y

xcopy .\DataBase\Include\*.h .\DBEngine\Include\ /s /d /y
xcopy .\DataBase\Bin\*.lib .\DBEngine\Bin\ /s /d /y
xcopy .\DataBase\Bin\*.dll .\DBEngine\Bin\ /s /d /y

xcopy .\Math\Include\*.h .\Engine\Include\ /s /d /y
xcopy .\Math\Bin\*.lib .\Engine\Bin\ /s /d /y
xcopy .\Math\Bin\*.lib .\GameServer\Bin\ /s /d /y
xcopy .\Math\Bin\*.lib .\MoveSyncServer\Bin\ /s /d /y
xcopy .\Math\Bin\*.lib .\MonsterAIServer\Bin\ /s /d /y

xcopy .\Monitor\Include\*.h .\Engine\Include\ /s /d /y
xcopy .\Monitor\Bin\*.lib .\Engine\Bin\ /s /d /y
xcopy .\Monitor\Bin\*.lib .\LoginServer\Bin\ /s /d /y
xcopy .\Monitor\Bin\*.lib .\GameServer\Bin\ /s /d /y
xcopy .\Monitor\Bin\*.lib .\MonitoringServer\Bin\ /s /d /y
xcopy .\Monitor\Bin\*.lib .\MonsterAIServer\Bin\ /s /d /y

xcopy .\DBEngine\Bin\*.* .\LoginServer\Bin\ /s /d /y
xcopy .\DBEngine\Bin\*.* .\GameServer\Bin\ /s /d /y
xcopy .\DBEngine\Bin\*.* .\MoveSyncServer\Bin\ /s /d /y
xcopy .\DBEngine\Bin\*.* .\MonsterAIServer\Bin\ /s /d /y

:: xcopy�� ���Ϻ��� xcopy a b a���� b�� �����ϰڴ�.
:: ./ ���� ����
:: s -> �Ⱥ���ִ� �� + �������丮.
:: d -> ������ ��¥���Ŀ� �ٲ�͸�.
:: y -> ������� ���°��� ���ѰԲ�.