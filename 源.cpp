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

	//MessageBox(NULL, _T("路径"), _T("测试"), IDOK);
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
				if (i==0) {		//选择方式生成方式：视频 or 图片
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

	//加载文件
	void LoadFile() {
			TCHAR  szPeFileExt[1024] = TEXT("*.*");  //打开任意类型的文件
			TCHAR szPathName[80*MAX_PATH];               //文件路径大小
			OPENFILENAME ofn = { sizeof(OPENFILENAME) };
			//ofn.hwndOwner = hWnd;// 打开OR保存文件对话框的父窗口
			ofn.lpstrFilter = szPeFileExt;
			lstrcpy(szPathName, TEXT(""));
			ofn.lpstrFile = szPathName;
			ofn.nMaxFile = sizeof(szPathName);//存放用户选择文件的 路径及文件名 缓冲区
			ofn.lpstrTitle = TEXT("选择文件");//选择文件对话框标题
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;//如果需要选择多个文件 则必须带有  OFN_ALLOWMULTISELECT标志
			BOOL bOk = GetOpenFileName(&ofn);
			if (bOk) {
				CEditUI* pPathEdit = (CEditUI*)m_PaintManager.FindControl(_T("Edit"));
				pPathEdit->SetText(szPathName);
			}
	}

	//旧的
	////用cmd发命令
	//void GenerateGifWithPic() {
	//	//获取当前工程路径 --> ffmpeg完整路径
	//	CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");  
	//	//1. 初始化结构体
	//	SHELLEXECUTEINFO strSEInfo;
	//	memset(&strSEInfo, 0, sizeof(SHELLEXECUTEINFO));
	//	strSEInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	//	strSEInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	//	strSEInfo.lpFile = _T("C:\\Windows\\System32\\cmd.exe");
	//	//构造命令
	//	CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
	//	CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
	//	CDuiString strCMD = (_T("/c "));
	//	strCMD += strFFmpegPath;
	//	strCMD += _T("-r 1 -i ");
	//	strCMD+= strPictruePath + strOutPath;
	//	strSEInfo.lpParameters = strCMD;
	//	strSEInfo.nShow = SW_HIDE; //隐藏cmd'窗口
	//	//2. 发送cmd命令
	//	ShellExecuteEx(&strSEInfo);
	//	WaitForSingleObject(strSEInfo.hProcess, INFINITE);
	//	//D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg ffmpeg -r 1 -i D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif
	//} 

	//修改1___成功
	void GenerateGifWithPic() {
		//CDuiString strCMD(_T(" ffmpeg -r 1 -i D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif"));
		//构造命令
		CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
		CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
		CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
		CDuiString strCMD = (_T("/c "));
		strCMD += strFFmpegPath;
		strCMD += _T("-r 1 -i ");
		strCMD += strPictruePath + strOutPath;
		SendCmd(strCMD);
		MessageBox(m_hWnd, _T("图片方式生成Gif成功！"), _T("GIFF"), IDOK);

	}

	void Cut() {
		CDuiString strStartTime = ((CEditUI *)m_PaintManager.FindControl(_T("BeginTime")))->GetText();
		if (!IsVaildTime(strStartTime)) {
			MessageBox(m_hWnd, _T("输入起始时间有误！"), _T("GIFF"), IDOK);
			return;
		}
		CDuiString strFinishTime = ((CEditUI *)m_PaintManager.FindControl(_T("EndTime")))->GetText();
		if (!IsVaildTime(strFinishTime)) {
			MessageBox(m_hWnd, _T("输入结束时间有误！"), _T("GIFF"), IDOK);
			return;
		}
		//构造截取视频的命令
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
		MessageBox(m_hWnd, _T("视频截取成功!"), _T("GIFF"), IDOK);
	}

	void SendCmd(const CDuiString& strCMD) {
		
		//1. 初始化结构体
		SHELLEXECUTEINFO strSEInfo;
		memset(&strSEInfo, 0, sizeof(SHELLEXECUTEINFO));
		strSEInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		strSEInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		strSEInfo.lpFile = _T("C:\\Windows\\System32\\cmd.exe");

		////构造命令
		//CDuiString strPictruePath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg ");
		//CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\out.gif");
		//CDuiString strCMD = (_T("/c "));
		//strCMD += strFFmpegPath;
		//strCMD += _T("-r 1 -i ");
		//strCMD += strPictruePath + strOutPath;
		strSEInfo.lpParameters = strCMD;
		strSEInfo.nShow = SW_HIDE; //隐藏cmd'窗口

		//2. 发送cmd命令
		ShellExecuteEx(&strSEInfo);
		WaitForSingleObject(strSEInfo.hProcess, INFINITE);
		//D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg ffmpeg -r 1 -i D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\Pictrue\\%d.jpg D:\项目\项目1\ProjectZhou\Debug\\ffmpeg\\ffmpeg\\out.gif
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

	//视频生成
	void GenerateGifWithView() {	
		//构造命令
		CDuiString strFFmpegPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\ffmpeg ");
		CDuiString strViewPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\output.mp4 ");
		CDuiString strOutPath = CPaintManagerUI::GetInstancePath() + _T("\\ffmpeg\\ffmpeg\\outView.gif");
		CDuiString strCMD = (_T("/c "));
		strCMD += strFFmpegPath;
		strCMD += _T("-r 50 -i ");
		strCMD += strViewPath + strOutPath;
		SendCmd(strCMD);
		MessageBox(m_hWnd, _T("视频方式生成Gif成功！"), _T("GIFF"), IDOK);
	}

	////生成ASS
	//void CreatASS() {
	//	//构造命令
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
	//	MessageBox(m_hWnd, _T("视频方式生成Gif成功！"), _T("GIFF"), IDOK);
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