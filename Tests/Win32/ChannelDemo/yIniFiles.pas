unit yIniFiles;

interface

uses
  IniFiles,SysUtils,Windows;

const
  chIniParamDiv= ';';  

type
  TMyIniFile = class(TIniFile)
  private
    { Private declarations }
  public
    procedure rwString(bRead : boolean; Section,Ident,Default : string; var Res : string);
    procedure rwInt(bRead : boolean; Section,Ident: string; Default : integer; var Res : integer);
    procedure rwBool(bRead : boolean; Section,Ident: string; Default : boolean; var Res : boolean);
  end;

  function OpenIniFile(var iFile : TMyIniFile;
                        FileName : string;
                        CreateIfNotExists : boolean) : boolean;

  function GetParam(var inst:string):string;

implementation

function OpenIniFile(var iFile : TMyIniFile;
                        FileName : string;
                        CreateIfNotExists : boolean) : boolean;
var
  hFile : Cardinal;
begin
  if not FileExists(FileName) then
    if not CreateIfNotExists then
      begin //Файла нету и не надо его создавать
        Result := FALSE;
        exit;
      end
    else
      begin
        //Создадим новый файл
        hFile := CreateFile(PChar(FileName),0,0,Nil,CREATE_ALWAYS,0,0);
        if (hFile=INVALID_HANDLE_VALUE) then
          begin
            Result := FALSE;
            exit;
          end
        else CloseHandle(hFile);
      end;
  iFile := TMyIniFile.Create(FileName);    
  Result := TRUE;
end;

{ TMyIniFile }

procedure TMyIniFile.rwInt(bRead: boolean; Section, Ident: string;
  Default: integer; var Res: integer);
begin
  if bRead then
    Res := ReadInteger(Section, Ident, Default)
  else
    WriteInteger(Section, Ident, Res);
end;

procedure TMyIniFile.rwString(bRead : boolean; Section, Ident, Default: string;
  var Res: string);
begin
  if bRead then
    Res := ReadString(Section, Ident, Default)
  else
    WriteString(Section, Ident, Res);
end;

procedure TMyIniFile.rwBool(bRead : boolean; Section,Ident: string; Default : boolean; var Res : boolean);
begin
  if bRead then
    Res := ReadBool(Section, Ident, Default)
  else
    WriteBool(Section, Ident, Res);
end;


function GetParam(var inst:string):string;
var chCnt,dCnt:integer;
begin
 chCnt:=pos(chIniParamDiv,inst)-1;
 if chCnt<{=}0 then begin chCnt:=length(inst);dCnt:=chCnt;end else dCnt:=chCnt+1;
 Result:=copy(inst,1,chCnt);
 system.delete(inst,1,dCnt);
end;

end.
