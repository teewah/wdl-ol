#pragma once

#if defined(__LP64__) && !defined(IPLUG_NO_CARBON_SUPPORT)
  #define IPLUG_NO_CARBON_SUPPORT
#endif

#ifdef IGRAPHICS_AGG
  #define IGRAPHICS_DRAW_CLASS IGraphicsAGG
  #include "IGraphicsAGG.h"
#elif defined IGRAPHICS_CAIRO
  #define IGRAPHICS_DRAW_CLASS IGraphicsCairo.h
  #include "IGraphicsCairo.h"
#else
  #define IGRAPHICS_DRAW_CLASS IGraphicsLice
  #include "IGraphicsLice.h"
#endif

#include "swell.h"
#include <Carbon/Carbon.h>

#ifndef DEFAULT_PATH_OSX
  #define DEFAULT_PATH_OSX "~/Desktop"
#endif

#ifndef IPLUG_NO_CARBON_SUPPORT
  class IGraphicsCarbon;
#endif

#ifndef COCOA_PREFIX
  #define COCOA_PREFIX vOliLarkin
#endif

#if defined(VST_API)
  #define API _vst
#elif defined(AU_API)
  #define API _au
#elif defined(AAX_API)
  #define API _aax
#elif defined(VST3_API)
  #define API _vst3
#elif defined(SA_API)
  #define API _sa
#endif

#define CONCAT3(a,b,c) a##b##c
#define CONCAT2(a,b,c) CONCAT3(a,b,c)
#define CONCAT(cname) CONCAT2(cname,COCOA_PREFIX,API)

#define IGRAPHICS_COCOA CONCAT(IGraphicsCocoa_)
#define IGRAPHICS_NSMENU CONCAT(IGraphicsNSMenu_)
#define IGRAPHICS_MENU_RCVR CONCAT(IGraphicsMenuRcvr_)
#define CUSTOM_COCOA_WINDOW CONCAT(CustomCocoaWindow_)
#define COCOA_FORMATTER CONCAT(CocoaFormatter_)

class IGraphicsMac : public IGRAPHICS_DRAW_CLASS
{
public:
  IGraphicsMac(IPlugBase* pPlug, int w, int h, int refreshFPS);
  virtual ~IGraphicsMac();

  void SetBundleID(const char* bundleID) { mBundleID.Set(bundleID); }

  bool DrawScreen(const IRECT& pR) override;
  
  void* OpenWindow(void* pWindow) override;
#ifndef IPLUG_NO_CARBON_SUPPORT
  void* OpenWindow(void* pWindow, void* pControl, short leftOffset = 0, short topOffset = 0);
#endif

  void* OpenCocoaWindow(void* pParentView);
#ifndef IPLUG_NO_CARBON_SUPPORT
  void* OpenCarbonWindow(void* pParentWnd, void* pParentControl, short leftOffset, short topOffset);
#endif

//  void AttachSubWindow(void* hostWindowRef);
//  void RemoveSubWindow();

  void CloseWindow() override;
  bool WindowIsOpen() override;
  void Resize(int w, int h) override;

  void HideMouseCursor() override;
  void ShowMouseCursor() override;

  int ShowMessageBox(const char* pText, const char* pCaption, int type) override;
  void ForceEndUserEdit() override;

  const char* GetGUIAPI() override;
  
  void UpdateTooltips() override;

  void HostPath(WDL_String& pPath) override;
  void PluginPath(WDL_String& pPath) override;
  void DesktopPath(WDL_String& pPath) override;
//  void VST3PresetsPath(WDL_String& pPath, bool isSystem = true);
  void AppSupportPath(WDL_String& pPath, bool isSystem = false) override;
  void SandboxSafeAppSupportPath(WDL_String& pPath) override;

  void PromptForFile(WDL_String& fileName, EFileAction action = kFileOpen, WDL_String* pDir = 0, const char* extensions = "") override;   // extensions = "txt wav" for example.
  bool PromptForColor(IColor& color, const char* pStr) override;

  IPopupMenu* CreateIPopupMenu(IPopupMenu& menu, IRECT& textRect) override;
  void CreateTextEntry(IControl* pControl, const IText& text, const IRECT& textRect, const char* pStr, IParam* pParam) override;

  bool OpenURL(const char* pURL, const char* pMsgWindowTitle = 0, const char* pConfirmMsg = 0, const char* pErrMsgOnFailure = 0) override;

  void* GetWindow() override;

  const char* GetBundleID()  { return mBundleID.Get(); }
  static int GetUserOSVersion();   // Returns a number like 0x1050 (10.5).
  
  bool GetTextFromClipboard(WDL_String& pStr) override;

  bool MeasureIText(const IText& text, const char* pStr, IRECT& destRect) override;
protected:
  void OSLoadBitmap(const char* name, WDL_String& fullPath) override;
//  void OSLoadFont(const char* name, const int size) override;
//  bool LoadSVGFile(const WDL_String & file, WDL_String & fileOut);
  
private:
#ifndef IPLUG_NO_CARBON_SUPPORT
  IGraphicsCarbon* mGraphicsCarbon;
#endif
  void* mGraphicsCocoa;   // Can't forward-declare IGraphicsCocoa because it's an obj-C object.

  WDL_String mBundleID;
  
  friend int GetMouseOver(IGraphicsMac* pGraphics);
  
#ifndef IPLUG_NO_CARBON_SUPPORT
  friend class IGraphicsCarbon;
#endif
  
public:
//  void* mHostNSWindow;
};

inline CFStringRef MakeCFString(const char* cStr)
{
  return CFStringCreateWithCString(0, cStr, kCFStringEncodingUTF8);
}

struct CFStrLocal
{
  CFStringRef mCFStr;
  CFStrLocal(const char* cStr)
  {
    mCFStr = MakeCFString(cStr);
  }
  ~CFStrLocal()
  {
    CFRelease(mCFStr);
  }
};

struct CStrLocal
{
  char* mCStr;
  CStrLocal(CFStringRef cfStr)
  {
    long n = CFStringGetLength(cfStr) + 1;
    mCStr = (char*) malloc(n);
    CFStringGetCString(cfStr, mCStr, n, kCFStringEncodingUTF8);
  }
  ~CStrLocal()
  {
    FREE_NULL(mCStr);
  }
};

inline CGRect ToCGRect(int h, IRECT* pR)
{
  int B = h - pR->B;
  return CGRectMake(pR->L, B, pR->W(), B + pR->H());
}

inline int AdjustFontSize(int size)
{
  return int(0.9 * (double)size);
}