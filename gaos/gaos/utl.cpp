// 辅助函数：将 TCHAR 转为 char
void TCHARToChar(const TCHAR* tcharStr, char* charStr, size_t size) {
#ifdef UNICODE
    WideCharToMultiByte(CP_UTF8, 0, tcharStr, -1, charStr, (int)size, NULL, NULL);
#else
    strncpy_s(charStr, size, tcharStr, _TRUNCATE);
#endif
}

// MultiByte (UTF-8) → WideChar (Unicode) 转换 (适用于 Unicode 编译)
TCHAR* ConvertToTCHAR(const char* str) {
#ifdef UNICODE
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    TCHAR* wideStr = (TCHAR*)malloc(size_needed * sizeof(TCHAR));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr, size_needed);
    return wideStr;
#else
    return _strdup(str);
#endif
}