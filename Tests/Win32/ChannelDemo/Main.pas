unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ComCtrls, ImgList, ToolWin, RXSlider, Menus,
  RXSwitch, IdemFrame, Channel;

const
  TAG_CB_PORT           = $01;
  TAG_CB_RATE           = $02;
  TAG_CB_PARITY         = $04;

  CAPTION_BUTTON_OPEN   = 'Открыть порт';
  CAPTION_BUTTON_CLOSE  = 'Закрыть порт';

type
  TfmMain = class(TForm)
    Panel1: TPanel;
    gbPort: TGroupBox;
    cbPort: TComboBox;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    btOpen: TButton;
    GroupBox1: TGroupBox;
    tProtRedraw: TTimer;
    gbChannels: TGroupBox;
    tpProtocol: TToolBar;
    ToolButton1: TToolButton;
    btCapture: TToolButton;
    ToolButton3: TToolButton;
    btClear: TToolButton;
    ilButtons: TImageList;
    lbProt: TListBox;
    GroupBox4: TGroupBox;
    cbCheckSum: TCheckBox;
    MainMenu1: TMainMenu;
    N1: TMenuItem;
    N2: TMenuItem;
    OpenDialog1: TOpenDialog;
    lComPortUstError: TLabel;
    btTest: TButton;
    sbIdems: TScrollBox;
    edRate: TEdit;
    edParity: TEdit;
    Splitter1: TSplitter;
    tmTest: TTimer;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure btOpenClick(Sender: TObject);
    procedure tProtRedrawTimer(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure edAdrKeyPress(Sender: TObject; var Key: Char);
    procedure edValueChange(Sender: TObject);
    procedure btClearClick(Sender: TObject);
    procedure btCaptureClick(Sender: TObject);
    procedure cbCheckSumClick(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure cbComUstChange(Sender: TObject);
    procedure btTestClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure tmTestTimer(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    DrawStr : TStrings;
    ComUstError : integer;
    fChannelList: TChannelList;
    procedure LoadIdem;
    procedure ReadFromIniFile;
    procedure WriteToIniFile;
    procedure FillControls;
    procedure OpenComPort;
    procedure CloseComPort;
    procedure SetDatParams;
    procedure CalcValue;
    procedure OpenConfig;
    procedure LoadConfig(FileName : string);
    procedure LoadState;
    procedure SaveState;
    procedure EmptyConfig;
    procedure ErrorMessage(Mes : string);
    procedure ShowComPortUstError;
    procedure rwIni(bRead : boolean);
    procedure SetUIComPort;
  public
    { Public declarations }
  end;

var
  fmMain: TfmMain;
  FullIniFileName : string;

implementation

uses
  Common,IniFiles, DataThreads, SimpleXML, yIniFiles;

const
// ------------XML - Кофигурация---------------
  NodeNameRoot          = 'Configuration';
  NodeNamePort          = 'Port';
  NodeNameChannels      = 'Channels';
  NodeNameChannel       = 'Channel';
  NodeNameNumber        = 'Number';
  NodeNameRate          = 'Rate';
  NodeNameParity        = 'Parity';
  NodeNameType          = 'Type';
  NodeNameAdress        = 'Adress';

  NodeNameWork          = 'Work';
  NodeNameTurnOn        = 'TurnOn';
  NodeNameIndex         = 'Index';  

var
  IniFile : TIniFile;
  bOpenPort : boolean;
  ComReadThread : TComReadThread;
  Err : Cardinal;
  bCaptureProt : boolean;
  ComPortUst : TComPortUst;
  IdemFrames : array of TIdemFram;
  ConfName,ConfFileName,StateFileName : string;
  TestVal: Integer;

{$R *.dfm}

//Функция для получения дочернего узла
//Если узел не существует, то он создается
function GetSingleNode(ParentNode: IXmlNode; Name: string): IXmlNode;
var
  Node: IXmlNode;
begin
  Node := ParentNode.SelectSingleNode(Name);
  if (Node=nil) then
    Node := ParentNode.AppendElement(Name);
  Result := Node;
end;

procedure TfmMain.FormCreate(Sender: TObject);
var
  i : integer;
begin
  ProgPath := ExtractFilePath(ParamStr(0));

  EnumSerialPorts(cbPort.Items);

  IniFileName := ProgPath+'\'+ProgName+'.ini';
  FullIniFileName := ProgPath+'\'+ProgName+'.ini';
  rwIni(True);
  ReadFromIniFile;

  fChannelList := TChannelList.Create;

  bOpenPort := FALSE;
  CommandID := 1;
  ComBufInd := 0;
  LastDrawComID := 0;
  LastDrawPos := -1;
  bCaptureProt := FALSE;

  btOpen.Caption := CAPTION_BUTTON_OPEN;
  FillControls;

  DatTypeMin[0]:=0;
  DatTypeMax[0]:=5;
  DatTypeMin[1]:=4;
  DatTypeMax[1]:=20;
  SetDatParams;

  DrawStr := TStringList.Create;

  for i:=0 to ComBufSize-1 do ComBuf[i].ID:=0;

  cbPort.Tag := TAG_CB_PORT;

  EmptyConfig;

  TestVal := 80;
end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  SaveState;
  WriteToIniFile;
  rwIni(False);
  DrawStr.Free;
end;

procedure TfmMain.LoadIdem;
begin
end;

procedure TfmMain.ReadFromIniFile;
begin
  if not FileExists(IniFileName) then Exit;

  IniFile := TIniFile.Create(IniFileName);

  ComPortUst.Port := IniFile.ReadString(IniSectPort, IniKeyPortNum, '');
  ComPortUst.Rate := IniFile.ReadInteger(IniSectPort, IniKeyPortRate, 9600);
  ComPortUst.Parity := IniFile.ReadInteger(IniSectPort, IniKeyPortParity, 0);

  ModuleNameStr := IniFile.ReadString(IniSectModule,IniKeyModuleName,'NONE');
  IdemAddress := IniFile.ReadInteger(IniSectModule,IniKeyModuleAdress,1);
  IdemAddressStr := IntToHex(IdemAddress,2);

  DatType:=IniFile.ReadInteger(IniSectDat,IniKeyDatType,0);
  bPressure:= IniFile.ReadBool(IniSectDat,IniKeyPressure,FALSE);

  bUseCheckSum := IniFile.ReadBool(IniSectDCON,IniKeyCheckSum,FALSE);

  ConfFileName := IniFile.ReadString(IniSectConfig,IniKeyLastLoadConfig,'');

  IniFile.Free;
end;

procedure TfmMain.WriteToIniFile;
var
  hFile : Cardinal;
  Str : string;
begin
  if not FileExists(IniFileName) then
    begin
      hFile := CreateFile(PChar(IniFileName),0,0,Nil,CREATE_ALWAYS,0,0);
      if (hFile=INVALID_HANDLE_VALUE) then
        begin
          Str := 'Не удается создать файл ' + IniFileName + '. Err=' + IntToStr(GetLastError());
          MessageBox(self.Handle,PChar(Str),PChar(ProgName),mb_ok or mb_iconerror);
        end;
      CloseHandle(hFile);
    end;
  IniFile := TIniFile.Create(IniFileName);

  IniFile.WriteString(IniSectPort,IniKeyPortNum, ComPortUst.Port);
  IniFile.WriteInteger(IniSectPort,IniKeyPortRate,ComPortUst.Rate);
  IniFile.WriteInteger(IniSectPort,IniKeyPortParity,ComPortUst.Parity);

  IniFile.WriteString(IniSectModule,IniKeyModuleName,ModuleNameStr);
  IniFile.WriteInteger(IniSectModule,IniKeyModuleAdress,IdemAddress);

  IniFile.WriteInteger(IniSectDat,IniKeyDatType,DatType);
  IniFile.WriteBool(IniSectDat,IniKeyPressure,bPressure);

  IniFile.WriteBool(IniSectDCON,IniKeyCheckSum,bUseCheckSum);

  IniFile.WriteString(IniSectConfig,IniKeyLastLoadConfig,ConfFileName);

  IniFile.Free;
end;

procedure TfmMain.FillControls;
var
  i : integer;
begin
  cbPort.Text := ComPortUst.Port;
//  cbRate.Text := IntToStr(ComPortUst.Rate);
//  cbParity.ItemIndex := ComPortUst.Parity;

  cbCheckSum.Checked := bUseCheckSum;
end;

procedure TfmMain.CloseComPort;
begin
  ComReadThread.bStop := TRUE;
  //if (not ComReadThread.Suspended) then ComReadThread.Suspend;
  WaitForSingleObject(ComReadThread.Handle,5000);
  ComReadThread.Free;
  ComReadThread:=nil;
  CloseHandle(hComPort);
  bOpenPort := FALSE;
end;

procedure TfmMain.OpenComPort;
var
  PortName,ErrStr : string;
  dcb : _DCB;
  cmt : _COMMTIMEOUTS;
begin
  PortName := cbPort.Text;
  hComPort := CreateFile(PChar('\\.\' + PortName),GENERIC_READ or GENERIC_WRITE,0,Nil,OPEN_EXISTING,0,0);
  if (hComPort=INVALID_HANDLE_VALUE) then
    begin
      Err := GetLastError();
      ErrStr := 'Error of opening ' + PortName + #13;
      case Err of
        2 : ErrStr := ErrStr+'Device not found!';
        5 : ErrStr := ErrStr+'Access denied!';
      else ErrStr := ErrStr+'Err=' + IntToStr(Err);
      end;
      MessageBox(self.Handle,PChar(ErrStr),PChar('Error'),MB_OK + MB_ICONERROR);
      Exit;
    end;

  GetCommState(hComPort,dcb);
  dcb.BaudRate := ComPortUst.Rate;
  dcb.ByteSize := 8;
  dcb.Parity := ComPortUst.Parity;
  dcb.Flags := RTS_CONTROL_DISABLE;
  //dcb.Flags := RTS_CONTROL_ENABLE;

  {if cbCTS.Checked then
     dcb.Flags := dcb.Flags or $04
   else }
   dcb.Flags := dcb.Flags and $FFFFFFFB;
  if (not SetCommState(hComPort,dcb)) then
    begin
      Err := GetLastError();
      MessageBox(self.Handle,PChar('Err=' + IntToStr(Err)),PChar(''),MB_OK);
    end;

  GetCommTimeouts(hComPort,cmt);
  cmt.ReadTotalTimeoutConstant := 200;
  cmt.WriteTotalTimeoutConstant := 100;
  cmt.WriteTotalTimeoutMultiplier := 10;
  SetCommTimeouts(hComPort,cmt);

  if (ComReadThread = nil) then
  begin
    ComReadThread := TComReadThread.Create(FALSE);
    ComReadThread.channelList := fChannelList;
    SetThreadPriority(ComReadThread.ThreadID, THREAD_PRIORITY_HIGHEST);
  end
  else
    ComReadThread.Resume;

  bOpenPort := TRUE;
end;

procedure TfmMain.btOpenClick(Sender: TObject);
begin
  if bOpenPort then
    CloseComPort
  else
    OpenComPort;

  if bOpenPort then
  begin
    btOpen.Caption := CAPTION_BUTTON_CLOSE;
  end
  else
    begin
      btOpen.Caption := CAPTION_BUTTON_OPEN;
    end;
end;

procedure TfmMain.tProtRedrawTimer(Sender: TObject);
var
  DrawPos : integer;
  Com : TDconCommand;
  Str : string;
begin
  DrawStr.Clear;
  DrawPos:=LastDrawPos;
  while TRUE do
    begin
      Inc(DrawPos);
      if (DrawPos>=ComBufSize) then DrawPos:=0;
      if (ComBuf[DrawPos].ID<=LastDrawComID) then break; //дошли до номеров, которые уже рисовались
      Com := ComBuf[DrawPos];
      //Формируем строку
      case (Com.tip) of
        ComTypeGet : Str := '< ';
        ComTypeSend : Str :=  '> ';
        ComTypeGetDust: Str := '<.';
      else
        Str := '? ';
      end;
      Str := Str + FormatCurr('000000 ',Com.ID) + FormatCurr('000000 ',Com.Time) +
             {Com.LeadingChar + FormatCurr('00',Com.Address) + }Com.Command;
      if (Com.tip=ComTypeGetDust) then Str := Str + ' [dust]'
      else if (bUseCheckSum and (not Com.CS)) then Str := Str + ' [BAD CS]';
      DrawStr.Add(Str);
      LastDrawComID := Com.ID;
    end;
  LastDrawPos := DrawPos-1;
  if (LastDrawPos<0) then
    LastDrawPos := ComBufSize-1;

  if (DrawStr.Count>0) and (not bCaptureProt) then
  begin
    lbProt.Items.AddStrings(DrawStr);
    lbProt.ItemIndex := lbProt.Items.Count - 1;
  end;
end;

procedure TfmMain.Button1Click(Sender: TObject);
var
  Len : integer;
  bytesWritten : Cardinal;
  Buf : array[0..63] of Byte;
begin
  Len := 2;
  Buf[0] := $31;
  Buf[1] := $32;
  WriteFile(hComPort,Buf,Len,bytesWritten,nil);
{  TransmitCommChar(hComPort,char(Buf[0]));
  Sleep(10);
  TransmitCommChar(hComPort,char(Buf[1]));}
  //ComSendString(ModuleNameStr)
end;

procedure TfmMain.edAdrKeyPress(Sender: TObject; var Key: Char);
begin
  case Key of
    '0'..'9' : ;
    'a'..'f' : ;
    'A'..'F' : ;
    #8 : ;
  else Key:=#0;
  end;  
end;

procedure TfmMain.edValueChange(Sender: TObject);
begin
  CalcValue;
end;

procedure TfmMain.btClearClick(Sender: TObject);
begin
  //Очистка протокола
  lbProt.Items.Clear;
end;

procedure TfmMain.btCaptureClick(Sender: TObject);
begin
  bCaptureProt := btCapture.Down;
  if (btCapture.Down) then btCapture.ImageIndex:=1
  else btCapture.ImageIndex:=2;
end;

procedure TfmMain.SetDatParams;
begin
 
end;

procedure TfmMain.CalcValue;
begin

end;

procedure TfmMain.cbCheckSumClick(Sender: TObject);
begin
  bUseCheckSum := cbCheckSum.Checked;
end;

procedure TfmMain.N2Click(Sender: TObject);
begin
  OpenConfig;
end;

procedure TfmMain.OpenConfig;
var
  FileName : string;
begin
  if not OpenDialog1.Execute then Exit;
  FileName := OpenDialog1.FileName;
  LoadConfig(FileName);
end;

procedure TfmMain.LoadConfig(FileName : string); //Загрузка конфигурации
var
  aDoc: IXmlDocument;
  aElem,aElem2: IXmlNode;
  rootElem : IXmlElement;
  Str : string;
  lComPortUst : TComPortUst;
  i,j,lIdemCount,AInt : integer;
  bItemAdd : boolean;
  channel: TChannel;
begin

  //Создаем пустой документ XML
  aDoc := CreateXmlDocument;

  //Cчитываем из файла
  aDoc.Load(FileName);

  rootElem := aDoc.DocumentElement;
  if (rootElem.NodeName<>NodeNameRoot) then
    begin
      ErrorMessage('Некорекктный файл конфигурации!');
      exit;
    end;

  //Настройки порта
  aElem := rootElem.SelectSingleNode(NodeNamePort);
//  Str := aElem.GetChildText(NodeNameNumber,'1');
//  lComPortUst.Num := StrToIntDef(Str,1);
  lComPortUst.Port := ComPortUst.Port;
  Str := aElem.GetChildText(NodeNameRate,'9600');
  lComPortUst.Rate := StrToIntDef(Str,9600);
  Str := aElem.GetChildText(NodeNameParity,'0');
  lComPortUst.Parity := StrToIntDef(Str,0);

  //Модули
  aElem := rootElem.SelectSingleNode(NodeNameChannels);

  for i:=0 to aElem.ChildNodes.Count-1 do
  begin
    aElem2 := aElem.ChildNodes.Item[i];
    if (aElem2.NodeName = NodeNameChannel) then
    begin
      channel := TChannel.Create;
      if(channel.Init(aElem2)) then
        fChannelList.Add(channel)
      else
        channel.Free;
    end;
  end;    
  aDoc.Save(FileName);

  //Вроде все в порядке
  ComPortUst := lComPortUst;


  ConfFileName := FileName;
  Str := ExtractFileName(FileName);
  ConfName := Copy(Str,1,Pos('.',Str)-1);

  StateFileName := ExtractFileDir(ConfFileName) + '\' + ConfName + '.st';
  LoadState;

  fmMain.Caption := ProgName + ' [' + ConfName + ']';

  SetLength(IdemFrames, fChannelList.Count);
  for i := fChannelList.Count - 1 downto 0 do
  begin
    IdemFrames[i] := TIdemFram.Create(self);
    IdemFrames[i].Name := 'IdemFrame' + IntToStr(i+1);
    IdemFrames[i].Tag := i;
    IdemFrames[i].Parent := sbIdems;
    IdemFrames[i].Align := alTop;
    IdemFrames[i].Height := 120;
    IdemFrames[i].SetupChannel(fChannelList.GetChannelByPos(i));
  end;

  SetUIComPort;
end;

procedure TfmMain.LoadState;
var
  aDoc: IXmlDocument;
  aElem,aElem2: IXmlNode;
  rootElem : IXmlElement;
  Str : string;
  Ind,i : integer;
  Num: integer;
  loadCnt: Integer;
  channel: TChannel;
begin
  if not FileExists(StateFileName) then Exit;

  //Создаем пустой документ XML
  aDoc := CreateXmlDocument;

  //Cчитываем из файла
  aDoc.Load(StateFileName);

  rootElem := aDoc.DocumentElement;

  //Настройки порта
  aElem := rootElem.SelectSingleNode(NodeNamePort);
  if (aElem<>nil) then
  begin
    Str := aElem.GetChildText(NodeNameNumber,'');
    ComPortUst.Port := Str;
  end;

  //Каналы
  loadCnt := 0;

  aElem := GetSingleNode(rootElem, NodeNameChannels);
  for i := 0 to aElem.ChildNodes.Count - 1 do
  begin
    if(loadCnt >= fChannelList.Count) then
      break;

    aElem2 := aElem.ChildNodes.Item[i];
    if (aElem2.NodeName = NodeNameChannel) then
    begin
      channel := fChannelList.GetChannelByPos(loadCnt);
      if(channel <> nil) then
      begin
        channel.Load(aElem2);
        inc(loadCnt);
      end;
    end;
  end;

end;

procedure TfmMain.SaveState;
var
  aDoc: IXmlDocument;
  aElem, aElem2: IXmlNode;
  rootElem : IXmlElement;
  Str : string;
  Ind,i : integer;
  SaveCnt: Integer;
  channel: TChannel;
begin
  if StateFileName = '' then
    Exit;
  if not FileExists(StateFileName) then
  begin
    aDoc := CreateXmlDocument('State');
  end
  else
  begin
    aDoc := CreateXmlDocument;
    //Cчитываем из файла
    aDoc.Load(StateFileName);
  end;

  rootElem := aDoc.DocumentElement;

  //Настройки порта
  //aElem := rootElem.SelectSingleNode(NodeNamePort);
  aElem := GetSingleNode(rootElem,NodeNamePort);
  aElem.SetChildText(NodeNameNumber, ComPortUst.Port);

  //Каналы
  SaveCnt := 0;

  aElem := GetSingleNode(rootElem, NodeNameChannels);
  for i := 0 to aElem.ChildNodes.Count - 1 do
  begin
    if(SaveCnt >= fChannelList.Count) then
      break;

    aElem2 := aElem.ChildNodes.Item[i];
    if (aElem2.NodeName = NodeNameChannel) then
    begin
      channel := fChannelList.GetChannelByPos(SaveCnt);
      if(channel <> nil) then
      begin
        channel.Save(aElem2);
        inc(SaveCnt);
      end;
    end;
  end;

  for i := SaveCnt to fChannelList.Count - 1 do
  begin
    aElem2 := aElem.AppendElement(NodeNameChannel);
    channel := fChannelList.GetChannelByPos(i);
    if(channel <> nil) then
      channel.Save(aElem2);
  end;

  aDoc.Save(StateFileName);
end;

procedure TfmMain.EmptyConfig;
begin
end;

procedure TfmMain.ErrorMessage(Mes: string);
begin
  MessageBox(self.Handle,PCHAR(Mes),
             PCHAR(ProgName),MB_OK or MB_ICONERROR);
end;

procedure TfmMain.ShowComPortUstError;
begin
  lComPortUstError.Visible := (ComUstError>0);
end;

procedure TfmMain.cbComUstChange(Sender: TObject);
var
  lComBox : TComboBox;
  bError : boolean;
  Str : string;
  aI,i : integer;
begin
  lComBox := (Sender as TComboBox);
  Str := lComBox.Text;
  bError := true;
  case lComBox.Tag of
    TAG_CB_PORT :
    begin
      ComPortUst.Port := Str;
      bError := False;
    end;
    TAG_CB_RATE :
      begin
        aI := StrToIntDef(Str,-1);
        if (aI>0) then
          begin
            ComPortUst.Rate := aI;
            bError := FALSE;
          end;
      end;
    TAG_CB_PARITY :
      begin
        aI := -1;
        for i:=0 to ComPortParityCount-1 do
          begin
            if ((Length(ComPortParity[i])=Length(Str)) and
                (Pos(ComPortParity[i],Str)=1)) then
              begin
                aI := i;
                break;
              end;
          end;
        if (aI>=0) then
          begin
            ComPortUst.Parity := aI;
            bError := FALSE;
          end;
      end;
  end;
  if bError then
    ComUstError := ComUstError or lComBox.Tag
  else
    ComUstError := ComUstError and (not lComBox.Tag);
  ShowComPortUstError;
end;

procedure TfmMain.btTestClick(Sender: TObject);
begin

  tmTest.Enabled := True;
end;



procedure TfmMain.FormShow(Sender: TObject);
begin
  if (Length(ConfFileName)>0) then LoadConfig(ConfFileName);
end;

procedure TfmMain.rwIni(bRead: boolean);
var
  IniFile : TMyIniFile;
begin
  OpenIniFile(IniFile,FullIniFileName,TRUE);

  // Установки ком порта
  IniFile.rwString(bRead, IniSectPort, IniKeyPortNum, '', ComPortUst.Port);

  IniFile.Free;
end;

procedure TfmMain.SetUIComPort;
begin
  cbPort.Text := ComPortUst.Port;
  edRate.Text := IntToStr(ComPortUst.Rate);
  edParity.Text := ComPortParity[ComPortUst.Parity];
end;

procedure TfmMain.tmTestTimer(Sender: TObject);
begin
  TestVal := TestVal xor $40;
end;

procedure TfmMain.FormDestroy(Sender: TObject);
begin
  fChannelList.Free;
end;

end.
