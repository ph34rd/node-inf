#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Setupapi.h>
#include "nan.h"

#define BUF_SIZE_DATA 4096
#define BUF_SIZE_NAME 512

#define IC(ucs2, utf8) \
WideCharToMultiByte(CP_UTF8, 0, ucs2, -1, utf8, sizeof(utf8), NULL, NULL)

#define IC2(utf8, ucs2) \
MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ucs2, sizeof(ucs2) / sizeof(ucs2[0]))

NAN_METHOD(parse) {
  auto retObj = Nan::New<v8::Object>();

  // set return value
  info.GetReturnValue().Set(retObj);

  HINF hInf;

  if (info[0]->IsString()) {
    wchar_t wPath[MAX_PATH];
    auto filePath = info[0]->ToString();

    v8::String::Utf8Value val(filePath);
    IC2(*val, wPath);

    hInf = SetupOpenInfFileW(wPath, NULL, INF_STYLE_WIN4, NULL);
  } else {
    return;
  }

  if (hInf == INVALID_HANDLE_VALUE) {
    return;
  }

  wchar_t wSectionBuf[BUF_SIZE_NAME];
  char uSectionBuf[BUF_SIZE_NAME];
  wchar_t wDataBuf[BUF_SIZE_DATA];
  char uDataBuf[BUF_SIZE_DATA];
  INFCONTEXT infContext;

  auto sectionIndex = 0;

  while (SetupEnumInfSectionsW(hInf, sectionIndex++, wSectionBuf, sizeof(wSectionBuf), NULL)) {
    auto sectionArray = Nan::New<v8::Array>();
    auto arrayIndex = 0;

    auto linesCount = SetupGetLineCountW(hInf, wSectionBuf);

    for (auto i = 0; i < linesCount; i++) {
      BOOL found = SetupGetLineByIndexW(hInf, wSectionBuf, i, &infContext);

      if (found) {
        BOOL keyFound = SetupGetStringFieldW(&infContext, 0, wDataBuf, sizeof(wDataBuf), NULL);

        if (!keyFound) { // null string
          wDataBuf[0] = 0;
        }

        auto valueObj = Nan::New<v8::Object>();

        IC(wDataBuf, uDataBuf);
        Nan::Set(valueObj, Nan::New<v8::String>("key").ToLocalChecked(), Nan::New<v8::String>(uDataBuf).ToLocalChecked());

        BOOL textFound = SetupGetLineTextW(&infContext, NULL, NULL, NULL, wDataBuf, sizeof(wDataBuf), NULL);

        if (!textFound) { // null string
          wDataBuf[0] = 0;
        }

        IC(wDataBuf, uDataBuf);
        Nan::Set(valueObj, Nan::New<v8::String>("payload").ToLocalChecked(), Nan::New<v8::String>(uDataBuf).ToLocalChecked());

        sectionArray->Set(arrayIndex++, valueObj);
      }
    }

    IC(wSectionBuf, uSectionBuf);
    Nan::Set(retObj, Nan::New<v8::String>(uSectionBuf).ToLocalChecked(), sectionArray);
  }

  SetupCloseInfFile(hInf);
}

NAN_METHOD(verify) {
  BOOL result = FALSE;

  if (info[0]->IsString()) {
    wchar_t wPath[MAX_PATH];
    auto filePath = info[0]->ToString();

    v8::String::Utf8Value val(filePath);
    IC2(*val, wPath);

    SP_INF_SIGNER_INFO_W signerInfo;
    signerInfo.cbSize = sizeof(SP_INF_SIGNER_INFO_W);

    SP_ALTPLATFORM_INFO_V2 altPlatInfo;
    altPlatInfo.cbSize = 0;

    if (info[1]->IsObject()) {
      auto params = info[1]->ToObject();

      auto major = Nan::Get(params, Nan::New<v8::String>("major").ToLocalChecked()).ToLocalChecked();
      auto minor = Nan::Get(params, Nan::New<v8::String>("minor").ToLocalChecked()).ToLocalChecked();
      auto arch = Nan::Get(params, Nan::New<v8::String>("arch").ToLocalChecked()).ToLocalChecked();

      altPlatInfo.cbSize = sizeof(SP_ALTPLATFORM_INFO_V2);
      altPlatInfo.Platform = VER_PLATFORM_WIN32_NT;
      altPlatInfo.Flags = SP_ALTPLATFORM_FLAGS_VERSION_RANGE;
      altPlatInfo.MajorVersion = major->Uint32Value();
      altPlatInfo.MinorVersion = minor->Uint32Value();
      altPlatInfo.FirstValidatedMajorVersion = major->Uint32Value();
      altPlatInfo.FirstValidatedMinorVersion = minor->Uint32Value();
      altPlatInfo.ProcessorArchitecture = arch->Uint32Value();
    }

    result = SetupVerifyInfFileW(wPath, (altPlatInfo.cbSize == 0) ? NULL : &altPlatInfo, &signerInfo);
  }

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}
