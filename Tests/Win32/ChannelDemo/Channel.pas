unit Channel;

interface

uses
  Contnrs,
  Common, SimpleXML;

const
  CHANNEL_NUMBER_OF_STATUS = 10;

  STATUS_UNKNOWN        = 0;
  STATUS_AVAILABLE      = 1;
  STATUS_PREPARING      = 2;
  STATUS_CHARGING       = 3;
  STATUS_SUSPENDED_EVSE = 4;
  STATUS_SUSPENDED_EV   = 5;
  STATUS_FINISHING      = 6;
  STATUS_RESERVED       = 7;
  STATUS_UNAVAILABLE    = 8;
  STATUS_FAULTED        = 9;

  STATUS_STR_UNKNOWN        = 'Unknown';
  STATUS_STR_AVAILABLE      = 'Available';
  STATUS_STR_PREPARING      = 'Preparing';
  STATUS_STR_CHARGING       = 'Charging';
  STATUS_STR_SUSPENDED_EVSE = 'SuspendedEVSE';
  STATUS_STR_SUSPENDED_EV   = 'SuspendedEV';
  STATUS_STR_FINISHING      = 'Finishing';
  STATUS_STR_RESERVED       = 'Reserved';
  STATUS_STR_UNAVAILABLE    = 'Unavailable';
  STATUS_STR_FAULTED        = 'Faulted';

type
  TChannel = class
  private
    fAddress: Integer;
    fAdrStr: String;
    fStateOn: boolean;
    fStatus: Integer;
    procedure setStatus(const Value: Integer);
  public
    property Address: Integer read fAddress;
    property StateOn: Boolean read fStateOn;
    property Status: Integer read fStatus write setStatus;
    function Init(node: IXmlNode): Boolean;
    procedure Load(node: IXmlNode);
    procedure Save(node: IXmlNode);
    procedure setState(state: Boolean);
    function ProcessCommand(Command : TDConCommand; var OutStr : string) : boolean;
  end;

  TChannelList = class
  private
    fList: TObjectList;
    function getCount: Integer;
  public
    constructor Create;
    destructor Destroy; override;
    property Count: Integer read getCount;
    function Add(channel: TChannel): Integer;
    function GetChannelByPos(index: integer): TChannel;
    function GetChannelByAddress(address: integer): TChannel;
  end;

function ChannelGetStatusString(status: Integer): String;

implementation

uses
  SysUtils;

const
  MODULE_NAME = 'ChargePointConnector';

  ATTRIB_ADDRESS = 'address';
  ATTRIB_STATE_ON = 'stateOn';
  ATTRIB_STATUS   = 'status';

function ChannelGetStatusString(status: Integer): String;
begin
  Result := STATUS_STR_UNKNOWN;

  case status of
    STATUS_AVAILABLE:      Result := STATUS_STR_AVAILABLE;
    STATUS_PREPARING:      Result := STATUS_STR_PREPARING;
    STATUS_CHARGING:       Result := STATUS_STR_CHARGING;
    STATUS_SUSPENDED_EVSE: Result := STATUS_STR_SUSPENDED_EVSE;
    STATUS_SUSPENDED_EV:   Result := STATUS_STR_SUSPENDED_EV;
    STATUS_FINISHING:      Result := STATUS_STR_FINISHING;
    STATUS_RESERVED:       Result := STATUS_STR_RESERVED;
    STATUS_UNAVAILABLE:    Result := STATUS_STR_UNAVAILABLE;
    STATUS_FAULTED:        Result := STATUS_STR_FAULTED;
  end;
end;

{ TChannelList }

function TChannelList.Add(channel: TChannel): Integer;
begin
  fList.Add(channel);
  Result := fList.Count;
end;

constructor TChannelList.Create;
begin
  fList := TObjectList.Create;
end;

destructor TChannelList.Destroy;
begin
  fList.Free;
  inherited;
end;

function TChannelList.GetChannelByAddress(address: integer): TChannel;
var
  I: Integer;
  channel: TChannel;
begin
  for I := 0 to fList.Count - 1 do
  begin
    channel := fList.Items[I] as TChannel;
    if(channel.Address = address) then
    begin
      Result := channel;
      Exit;
    end;
  end;
  Result := nil;
end;

function TChannelList.GetChannelByPos(index: integer): TChannel;
begin
  if((index >= 0) and (index < fList.Count)) then
    Result := fList.Items[Index] as TChannel
  else
    Result := nil;
end;

function TChannelList.getCount: Integer;
begin
  Result := fList.Count;
end;

{ TChannel }

function TChannel.Init(node: IXmlNode): Boolean;
var
  s: String;
  iValue: Integer;
begin
  Result := False;

  fAddress := node.GetIntAttr(ATTRIB_ADDRESS, 0);
  fAdrStr := IntToHex(fAddress, 2);

  Result := True;
end;

procedure TChannel.Load(node: IXmlNode);
begin
  fStateOn := node.GetBoolAttr(ATTRIB_STATE_ON);
  fStatus := node.GetIntAttr(ATTRIB_STATUS);
end;

function TChannel.ProcessCommand(Command: TDConCommand;
  var OutStr: string): boolean;
const
  HEAD_LEN = 3;
  CS_LEN = 2;
var
  Str : string;
  ServLen: integer;
begin
  Result := FALSE;
  if not fStateOn then
    Exit; //Модуль выключен

  OutStr := '';
  if (bUseCheckSum) then
    ServLen := HEAD_LEN + CS_LEN
  else
    ServLen := HEAD_LEN;
  Str := Copy(Command.Command,HEAD_LEN+1,Length(Command.Command)-ServLen);

  case (Command.LeadingChar) of
    '$' :
      begin
        if (Str='M') then
          OutStr := '!' + fAdrStr + MODULE_NAME
        else if (Str = 'S') then
          OutStr := '!' + fAdrStr + IntToStr(fStatus)
        else if (Str = 'A') then
          OutStr := '>' + fAdrStr;
          //OutStr := '>' + '22002500260027003831393140314131';
      end;
    '~' :
      begin
        OutStr := '!' + fAdrStr; 
      end;
  end;
  Result := (Length(OutStr)>0);

end;

procedure TChannel.Save(node: IXmlNode);
begin
  node.SetBoolAttr(ATTRIB_STATE_ON, fStateOn);
  node.SetIntAttr(ATTRIB_STATUS, fStatus);
end;

procedure TChannel.setState(state: Boolean);
begin
  fStateOn :=  state;
end;

procedure TChannel.setStatus(const Value: Integer);
begin
  fStatus := Value;
end;

end.
