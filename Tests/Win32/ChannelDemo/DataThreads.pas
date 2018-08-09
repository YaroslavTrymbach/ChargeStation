unit DataThreads;

interface

uses
  Classes, Windows,
  Channel;

type
  TComReadThread = class(TThread)
  private
    fChannelList: TChannelList;
  protected
    procedure Execute; override;
  public
    bStop : boolean;
    property channelList: TChannelList read fChannelList write fChannelList;
  end;

  procedure ComSendString(Str : string);

implementation

uses
  Common,Dialogs,SysUtils;

var
  ComSend : TDconCommand;
  bComSendNeedWrite : Boolean;

{ TComReadThread }
procedure AddCommandToBuffer(Command: TDconCommand);
begin
  ComBuf[ComBufInd] := Command;
  Inc(ComBufInd);
  if (ComBufInd >= ComBufSize) then
    ComBufInd:=0;
end;

procedure ComSendString(Str: string);
var
  Len,sLen: integer;
  i : integer;
  Buf: array[0..63] of byte;
  bytesWritten : Cardinal;
  Res : BOOL;
  CS,OutStr : string;
begin
  Sleep(5);
  EscapeCommFunction(hComPort, SETRTS);
  Len := Length(Str);
  for i := 0 to Len-1 do
    Buf[i] := BYTE(Str[i+1]);
  if (bUseCheckSum) then //Подсчет контрольной суммы
  begin
    CS := DConCalcCS(Str);
    Buf[Len  ] := BYTE(CS[1]);
    Buf[Len+1] := BYTE(CS[2]);
    Inc(Len,2);
  end;
  Buf[Len] := $0D;
  Inc(Len);
  Res := WriteFile(hComPort, Buf, Len, bytesWritten,nil);
  Sleep(5);
  EscapeCommFunction(hComPort, CLRRTS);

  if not Res then
    begin
      ShowMessage(IntToStr(bytesWritten));
      exit;
    end;
  ComSend.Time := GetTickCount();
  ComSend.Command := Str;
  if bUseCheckSum then
  begin
    ComSend.Command := ComSend.Command + ' ' + CS;
    ComSend.CS := TRUE;
  end;
  ComSend.ID := CommandID;
  Inc(CommandID);
  bComSendNeedWrite := TRUE;
end;

procedure TComReadThread.Execute;
const
  PACK_LEN = 64;
var
  BytesRead : cardinal;
  Res : boolean;
  Packet: array[0..PACK_LEN-1] of char;
  PacLen,i,ComStart,ComEnd : integer;
  b : char;
  Command,ComGetDust : TDconCommand;
  bCom,bDust : boolean;
  Str, OutStr : string;
  channel: TChannel;
begin
  inherited;
  bStop := false;
  bComSendNeedWrite := FALSE;
  ComSend.tip := ComTypeSend;
  Command.tip := ComTypeGet;
  ComGetDust.tip := ComTypeGetDust;
  PacLen := 0;
  while (not bStop) do
  begin
    Res := ReadFile(hComPort, b, 1, BytesRead, Nil);
    if ((Res) and (BytesRead=1)) then
    begin
      if (b = #13) then
      begin
        Command.Time := GetTickCount();
        Command.ID := CommandID;
        Inc(CommandID);
        ComStart := -1;
        bCom := False;
        for i:=0 to PacLen-1 do
        if (Packet[i] in ['$','!','@','#','>','~']) then
        begin
          bCom := True;
          Command.LeadingChar := Packet[i];
          ComStart := i;
          break;
        end;

              if (ComStart<>0) then //Попался какой-то мусор
              begin
                ComGetDust.Time := Command.Time;
                ComGetDust.ID := Command.ID;
                Command.ID := CommandID;
                Inc(CommandID);
                if (ComStart=-1) then ComStart := PacLen;
                SetLength(ComGetDust.Command,ComStart);
                MoveMemory(@ComGetDust.Command[1],@Packet[0],ComStart);
                bDust := True;
              end
              else bDust := False;

              if (bCom and bUseCheckSum) then
                begin
                  Str := '';
                  for i:=ComStart to PacLen-1 do Str := Str + Packet[i];
                  Command.CS:=DConCheckCS(Str);
                  ComEnd := PacLen-2;
                end
              else
                begin
                  Command.CS:=TRUE;
                  ComEnd := PacLen;
                end;
              SetLength(Command.Command,PacLen-ComStart);
              MoveMemory(@Command.Command[1],@Packet[ComStart],PacLen-ComStart);
              if (bCom and Command.CS) then
              begin
                Command.Address := StrToIntDef('$' + Packet[ComStart+1]+Packet[ComStart+2],$FF);
                channel := fChannelList.GetChannelByAddress(Command.Address);
                if(channel <> nil) then
                begin
                  if channel.ProcessCommand(Command, OutStr) then
                    ComSendString(OutStr);
                end
              end;
              if bDust then
                AddCommandToBuffer(ComGetDust);
              if bCom then
                AddCommandToBuffer(Command);
              if (bComSendNeedWrite) then
              begin
                AddCommandToBuffer(ComSend);
                bComSendNeedWrite := FALSE;
              end;
              PacLen := 0;
            end
          else //b<>#13 Обычный символ
            begin
              Packet[PacLen]:=b;
              Inc(PacLen);
              if (PacLen>=PACK_LEN) then PacLen:=0;
            end;
    end;
      //Sleep(0);
  end;
end;

end.
