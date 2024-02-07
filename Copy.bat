
cd..
cd..
:: 주석은 -> ::
:: 기준이 프로젝트폴더로 잡히기 떄문에 솔루션폴더로 이동하기위해 윗단계폴더로 이동  그것이 cd..
:: .\ 지금 현재 폴더
:: 빌드후 이벤트 반드시 설정을 해주자.
:: call "$Solution" 솔루션 폴더에서를 뜻한다.

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

:: xcopy는 파일복사 xcopy a b a에서 b로 복사하겠다.
:: ./ 현재 폴더
:: s -> 안비어있는 디렉 + 하위디렉토리.
:: d -> 지정된 날짜이후에 바뀐것만.
:: y -> 덮어쓸지를 묻는것을 안한게끔.