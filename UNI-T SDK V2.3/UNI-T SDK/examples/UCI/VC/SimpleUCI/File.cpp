#include "stdafx.h"
#include "File.h"

bool Files::ReadFile(const CString& _fileName, CString& _content) {
	_content.Empty();

	try {
		CFile f(_fileName, CFile::modeRead);
	
		size_t nBytes = (UINT)f.GetLength();
		size_t nChars = nBytes / sizeof(TCHAR);

		//从文件读取频点数据
		nBytes = f.Read(_content.GetBuffer(nChars), nBytes);

		_content.ReleaseBuffer(nChars);

	}
	catch (CFileException* e) {
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

void Files::WriteFile(const CString& _fileName, CString& _content) {
	try {
		CFile f(_fileName, CFile::modeWrite | CFile::modeCreate);
		f.Write(_content, _content.GetLength()*sizeof(TCHAR));
	}
	catch (CFileException* e) {
		e->ReportError();
		e->Delete();
	}
}

bool Files::WriteAllBytes(const CString& _name, const void* _b, size_t _len, CString* _path) {
	try {
		CFile f(_name, CFile::modeCreate | CFile::modeWrite);
		f.Write(_b, _len);

		if (_path != nullptr)
			*_path = f.GetFilePath();
	} catch (CException* e) {
		e->ReportError();
		e->Delete();
	}
	return true;
}
