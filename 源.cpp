#pragma once
#include <UIlib.h>
using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif


class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const   { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile()                { return _T("duilib.xml"); }
	virtual CDuiString GetSkinFolder()              { return _T(""); }

	//MessageBox(NULL, _T("·��"), _T("����"), IDOK);
	virtual void Notify(TNotifyUI& msg){
		CDuiString sCtrlName = msg.pSender->GetName();
		if (msg.sType == _T("click")) {
			if (sCtrlName == _T("closebtn")) {
				Close();
				return;
			}
			else if (sCtrlName == _T("minbtn")) {
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return;
			}
			else if (sCtrlName == _T("maxbtn")) {
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				return;
			}
			else if (sCtrlName == _T("restorebtn")) {
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				return;
			}
			else if (sCtrlName == _T("Look")) {
				LoadFile();
				return;
			}
			else if (sCtrlName == _T("CreatGif")) {
				CComboBoxUI* pCombox = (CComboBoxUI*)m_PaintManager.FindControl(_T("Combox_Select"));
				int i = pCombox->GetCurSel();
				if (i==0) {		//ѡ��ʽ���ɷ�ʽ����Ƶ or ͼƬ
					GenerateGifWithPic();
				}
				else {
					GenerateGifWithView();
				}
				return;
			}
			else if (sCtrlName == _T("cut")) {
				Cut();
				return;
			}
			return;
		}
	}

	//�����ļ�
	void LoadFile() {
			TCHAR  szPeFileExt[1024] = TEXT("*.*");  //���������͵��ļ�
			TCHAR szPathName[80*MAX_PATH];               //�ļ�·����С
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			//ofn.hwndOwner = hWnd;// ��OR�����ļ��Ի���ĸ�����
			ofn.lpstrFilter = szPeFileExt;
			lstrcpy(szPathName, TEXT(""));
			ofn.lpstrFile = szPathName;
			ofn.nMaxFile = sizeof(szPathName);//����û�ѡ���ļ��� ·�����ļ��� ������
			ofn.lpstrTitle = TEXT("ѡ���ļ�");//ѡ���ļ��Ի������
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;//�����Ҫѡ�����ļ� ��������  OFN_ALLOWMULTISELECT��־
			BOOL bOk = GetOpenFileName(&ofn);
			if (bOk) {
				CEditUI* pPathEdit = (CEditUI*)m_PaintManager.FindControl(_T("Edit"));
				pPathEdit->SetText(szPathName);
			}
	}

	//�ɵ�
	////��cmd������
	//void GenerateGifWithPic() {
	//	//��ȡ��ǰ����·�� --> ffmpeg����·��
	//	CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");  
	//	//1. ��ʼ���ṹ��
	//	SHELLEXECUTEINFO strSEInfo;
	//	memset(&strSEInfo, 0, sizeof(SHELLEXECUTEINFO));
	//	strSEInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	//	strSEInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	//	strSEInfo.lpFile = _T("C:\\Windows\\System32\\cmd.exe");
	//	//��������
	//	CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
	//	CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
	//	CDuiString strCMD = (_T("/c "));
	//	strCMD += strFFmpegPath;
	//	strCMD += _T("-r 1 -i ");
	//	strCMD+= strPictruePath + strOutPath;
	//	strSEInfo.lpParameters = strCMD;
	//	strSEInfo.nShow = SW_HIDE; //����cmd'����
	//	//2. ����cmd����
	//	ShellExecuteEx(&strSEInfo);
	//	WaitForSingleObject(strSEInfo.hProcess, INFINITE);
	//	//D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg ffmpeg -r 1 -i D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif
	//} 

	//�޸�1___�ɹ�
	void GenerateGifWithPic() {
		//CDuiString strCMD(_T(" ffmpeg -r 1 -i D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif"));
		//��������
		CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
		CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
		CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
		CDuiString strCMD = (_T("/c "));
		strCMD += strFFmpegPath;
		strCMD += _T("-r 1 -i ");
		strCMD += strPictruePath + strOutPath;
		SendCmd(strCMD);
		MessageBox(m_hWnd, _T("ͼƬ��ʽ����Gif�ɹ���"), _T("GIFF"), IDOK);

	}

	void Cut() {
		CDuiString strStartTime = ((CEditUI *)m_PaintManager.FindControl(_T("BeginTime")))->GetText();
		if (!IsVaildTime(strStartTime)) {
			MessageBox(m_hWnd, _T("������ʼʱ������"), _T("GIFF"), IDOK);
			return;
		}
		CDuiString strFinishTime = ((CEditUI *)m_PaintManager.FindControl(_T("EndTime")))->GetText();
		if (!IsVaildTime(strFinishTime)) {
			MessageBox(m_hWnd, _T("�������ʱ������"), _T("GIFF"), IDOK);
			return;
		}
		//�����ȡ��Ƶ������
		CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
		CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\output.mp4");
		CDuiString strCMD = (_T("/c "));
		strCMD += strFFmpegPath;
		strCMD += _T(" -ss ");
		strCMD += strStartTime;
		strCMD += _T(" -to ");
		strCMD += strFinishTime;
		strCMD += _T(" -i ");
		CDuiString strViewPath = ((CEditUI *)m_PaintManager.FindControl(_T("Edit")))->GetText();
		strCMD += strViewPath;
		strCMD += _T(" -vcodec copy -acodec copy ");
		strCMD += strOutPath;
		SendCmd(strCMD);
		MessageBox(m_hWnd, _T("��Ƶ��ȡ�ɹ�!"), _T("GIFF"), IDOK);
	}

	void SendCmd(const CDuiString& strCMD) {
		
		//1. ��ʼ���ṹ��
		SHELLEXECUTEINFO strSEInfo;
		memset(&strSEInfo, 0, sizeof(SHELLEXECUTEINFO));
		strSEInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		strSEInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		strSEInfo.lpFile = _T("C:\\Windows\\System32\\cmd.exe");

		////��������
		//CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
		//CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
		//CDuiString strCMD = (_T("/c "));
		//strCMD += strFFmpegPath;
		//strCMD += _T("-r 1 -i ");
		//strCMD += strPictruePath + strOutPath;
		strSEInfo.lpParameters = strCMD;
		strSEInfo.nShow = SW_HIDE; //����cmd'����

		//2. ����cmd����
		ShellExecuteEx(&strSEInfo);
		WaitForSingleObject(strSEInfo.hProcess, INFINITE);
		//D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg ffmpeg -r 1 -i D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\��Ŀ\��Ŀ1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif
	}

	bool IsVaildTime(const CDuiString& strTime) {
		if (strTime.GetLength() != 8) {
			return false;
		}
		for (int i = 0; i < 8;i++) {
			if (':'==strTime[i]) {
				continue;
			}
			else if (isdigit(strTime[i])) {
				continue;
			}
			else {
				return false;
			}
		}
		return true;
	}

	//��Ƶ����
	void GenerateGifWithView() {	
		//��������
		CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
		CDuiString strViewPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\output.mp4 ");
		CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\outView.gif");
		CDuiString strCMD = (_T("/c "));
		strCMD += strFFmpegPath;
		strCMD += _T("-r 50 -i ");
		strCMD += strViewPath + strOutPath;
		SendCmd(strCMD);
		MessageBox(m_hWnd, _T("��Ƶ��ʽ����Gif�ɹ���"), _T("GIFF"), IDOK);
	}

	////����ASS
	//void CreatASS() {
	//	//��������
	//	CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
	//	CDuiString strViewPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\output.mp4 ");
	//	CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\output.ass ");
	//	CDuiString strCMD = (_T("/c "));
	//	strCMD += strFFmpegPath;
	//	strCMD += _T("-i ");
	//	strCMD += strViewPath;
	//	strCMD += _T("-an -vn -scodec copy ");
	//	strCMD += strOutPath;
	//	SendCmd(strCMD);
	//	MessageBox(m_hWnd, _T("��Ƶ��ʽ����Gif�ɹ���"), _T("GIFF"), IDOK);
	//}


private:
	CEditUI* BeginTime;
	CEditUI* EndTime;
};


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();
	return 0;
}