#include "PathManager.h"

DEFINITION_SINGLE(CPathManager)

CPathManager::CPathManager()
{
}

CPathManager::~CPathManager()
{
    SAFE_DELETE_MAP(m_mapPath);
}

bool CPathManager::Init()
{
    TCHAR strPath[MAX_PATH] = {};

    //실행파일 주소(현재 실행중인 실행 파일의 경로를 얻는 방법)
    // c:\\abc\\0
    GetModuleFileName(0, strPath, MAX_PATH);
    
    // 실행파일 이름을 제거한다.
    size_t    iLength = lstrlen(strPath);

    for(size_t i = iLength - 1; i > 0; --i)
    {
        if (strPath[i] == '\\')
        {
            memset(&strPath[i + 1], 0, sizeof(TCHAR)* (MAX_PATH - 1 - i));
            break;
        }
    }

    PPath    pPath = new Path;
    lstrcpy(pPath->pPath, strPath);

#ifdef UNICODE
    // 문자열을 multibyte로 변환할 개수가 나온다.
    int iLength1 = WideCharToMultiByte(CP_ACP, 0, strPath, -1, nullptr, 0,
            nullptr, nullptr);
    WideCharToMultiByte(CP_ACP, 0, strPath, -1, pPath->pPathMultibyte,
        iLength1, nullptr, nullptr);
#else
    strcpy_s(pPath->pPathMultibyte, strlen(strPath), strPath);
#endif // UNICODE

    m_mapPath.insert(make_pair(ROOT_PATH, pPath));

    AddPath(DATA_PATH, TEXT("Data\\")); //C:\Users\tmvld\Desktop\Lecture\Network\newNetworkEngien-NEW\GameServer\Bin\

    return true;
}

bool CPathManager::AddPath(const string& strName,
    const TCHAR* pPath, const string& strBasePath) // strBasePath = rootpath
{
    // 같은 이름의 패스가 있을 경우 멈춘다.
    if (FindPath(strName))
        return false;

    PPath    pNewPath = new Path;

    const TCHAR* pBasePath = FindPath(strBasePath);

    if (pBasePath)
        lstrcpy(pNewPath->pPath, pBasePath);

    lstrcat(pNewPath->pPath, pPath);

#ifdef UNICODE
    // 문자열을 multibyte로 변환할 개수가 나온다.
    int iLength = WideCharToMultiByte(CP_ACP, 0, pNewPath->pPath, -1, nullptr, 0,
        nullptr, nullptr);
    WideCharToMultiByte(CP_ACP, 0, pNewPath->pPath, -1, pNewPath->pPathMultibyte,
        iLength, nullptr, nullptr);
#else
    strcpy_s(pPath->pPathMultibyte, strlen(pNewPath->pPath), pNewPath->pPath);
#endif // UNICODE

    m_mapPath.insert(make_pair(strName, pNewPath));
    
    return true;
}

const TCHAR* CPathManager::FindPath(const string& strName)
{
    auto    iter = m_mapPath.find(strName);

    if (iter == m_mapPath.end())
        return nullptr;

    return iter->second->pPath;
}

const char* CPathManager::FindPathMultibyte(const string& strName)
{
    auto    iter = m_mapPath.find(strName);

    if (iter == m_mapPath.end())
        return nullptr;

    return iter->second->pPathMultibyte; //
}

void CPathManager::CreatePath(char* FullPath, const char* FileName, const string& PathName)
{
    const char* Path = FindPathMultibyte(PathName);

    if (Path)
        strcpy_s(FullPath, strlen(Path) + 1, Path);
     
    int Len = strlen(FullPath) + strlen(FileName) + 1;
    int cause = strcat_s(FullPath, Len, FileName);
    // int cause = strcat_s(FullPath, MAX_PATH, FileName);

    // strlen으로 사이즈를 줬을떄 나는 오류가 원본 문자열을 초기화를 안하고 생기는 오류라는데 음....
    // 힙영역 초과했을때도 뜬다. 
    // 근데 왜 숫자로 사이즈를 넣으면 오류가 안나는지?
    // -> 크기를 원본 크기와 소스코드크기 + null 크기까지 포함한 크기를 넣어줘야한다.
    // 즉, 새롭게 다시 목적지 코드에 해당 크기만큼 쓰여지는 것.
    // 그리고 그 크기가 목적지 코드의 크기를 안넘어야한다.
}

void CPathManager::CreatePath(wchar_t* FullPath, const wchar_t* FileName, const string& PathName)
{
    const wchar_t* Path = FindPath(PathName);

    if (Path)
        lstrcpy(FullPath, Path);

    lstrcat(FullPath, FileName);
}
